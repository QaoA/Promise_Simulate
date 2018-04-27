#ifndef ANY_H
#define ANY_H

#include <type_traits>
#include <typeinfo>
#include <initializer_list>
#include <utility>
#include <exception>
#include "Any_help.h"
#include "NameSpace.h"

NAME_BEGIN

class BadAnyCast:public std::exception
{
public:
	virtual const char * what() const noexcept
	{
		return "bad any cast";
	}
};

class Any
{
private:
	union Storage
	{
		constexpr Storage() : m_ptr(nullptr){}
		Storage(const Storage &) = delete;
		Storage & operator=(const Storage &) = delete;

		void * m_ptr;
		std::aligned_storage<sizeof(m_ptr), alignof(void *)>::type m_buffer;
	};

	template<typename Tp, typename Safe = std::is_nothrow_move_constructible<Tp>,
		bool Fits = (sizeof(Tp) <= sizeof(Storage)) && (alignof(Tp) <= alignof(Storage))>
		using Internal = std::integral_constant<bool, Safe::value&&Fits>;

	template<typename Tp>
	struct ManagerInternal;

	template<typename Tp>
	struct ManagerExternal;

	template<typename Tp>
	using Manager = std::conditional_t<Internal<Tp>::value,
									ManagerInternal<Tp>,
									ManagerExternal<Tp>>;

	template<typename Tp, typename Decayed = std::decay_t<Tp>>
	using Decay = std::enable_if_t<!std::is_same<Decayed,Any>::value, Decayed>;

	template<typename Tp, typename...Args, typename manager=Manager<Tp>>
	void do_emplace(Args && ...args)
	{
		reset();
		manager::Create(m_storage,std::forward<Args>(args)...);
		m_manager = &manager::Manage;
	}

	template<typename Tp, typename Up, typename ...Args, typename manager = Manager<Tp>>
	void do_emplace(std::initializer_list<Up> l, Args&&...args)
	{
		reset();
		manager::Create(m_storage,l,std::forward<Args>(args)...);
		m_manager = &manager::Manage;
	}

public:

	bool hasValue() const
	{
		return m_manager != nullptr;
	}

	void reset() noexcept
	{
		if(hasValue())
		{
			m_manager(Op::Destroy,this, nullptr);
			m_manager = nullptr;
		}
	}

public:
	constexpr Any() noexcept : m_manager(nullptr){}
	Any(const Any & rhs)
	{
		if(rhs.hasValue())
		{
			Arg arg;
			arg.thisObj = this;
			rhs.m_manager(Op::Clone,&rhs,&arg);
		}
		else
		{	m_manager = nullptr;	}
	}

	Any(Any && rhs) noexcept
	{
		if(rhs.hasValue())
		{
			Arg arg;
			arg.thisObj = this;
			rhs.m_manager(Op::Move,&rhs,&arg);
		}
		else
		{	m_manager = nullptr;		}
	}

	template<typename Res, typename Tp, typename ...Args>
	using any_constructible = std::enable_if<std::is_copy_constructible<Tp>::value && std::is_constructible<Tp, Args...>::value, Res>;

	template<typename Tp, typename ...Args>
	using any_constructible_t = typename any_constructible<bool, Tp, Args...>::type;

	template<typename valueType,
			typename Tp = Decay<valueType>,
			typename manager = Manager<Tp>,
			any_constructible_t<Tp, valueType&&> = true>
	explicit Any(valueType && v):
	m_manager(&manager::Manage)
	{
		manager::Create(m_storage,std::forward<valueType>(v));
	}

	template<typename valueType,
			typename Tp = Decay<valueType>,
			typename manager = Manager<Tp>,
			std::enable_if_t<std::is_copy_constructible<Tp>::value &&
							 !std::is_constructible<Tp,valueType&&>::value, bool> = false>
	explicit Any(valueType && v):
	m_manager(&manager::Manage)
	{
		manager::Create(m_storage, v);
	}

	template<typename valueType,
			typename ...Args,
			typename Tp = Decay<valueType>,
			typename manager = Manager<Tp>,
			any_constructible_t<Tp, Args&&...> = false>
	explicit Any(in_place_type_t<valueType>, Args&&...args):
	m_manager(&manager::Manage)
	{
		manager::Create(m_storage, std::forward<Args&&>(args)...);
	}

	// template<typename valueType, typename Up, typename ...Args,
	// 		typename Tp = Decay<valueType>,
	// 		typename manager = Manager<Tp>,
	// 		any_constructible_t<Tp, std::initializer_list<Up>, Args &&...>=false>
	// explicit Any(in_place_type_t<valueType>, std::initializer_list<Up> list, Args &&...args):
	// m_manager(&manager::Manage)
	// {
	// 	manager::Create(m_storage,list, std::forward<Args>(args)...);
	// }

	~Any()
	{	reset();	}
	
	Any & operator=(const Any & rhs)
	{
		*this = Any(rhs);
		return *this;
	}

	Any & operator=(Any && rhs) noexcept
	{
		if(!rhs.hasValue())
		{
			reset();
		}
		else if(this != &rhs)
		{
			reset();
			Arg arg;
			arg.thisObj = this;
			rhs.m_manager(Op::Move, &rhs, &arg);
		}
		return *this;
	}

	template<typename valueType>
	std::enable_if_t<std::is_copy_constructible<Decay<valueType>>::value, Any&>
	operator=(valueType && v)
	{
		*this = Any(std::forward<valueType>(v));
		return *this;
	}

	template<typename valueType, typename...Args>
	typename any_constructible<Decay<valueType> &,Decay<valueType>, Args&&...>::type
		emplace(Args&&...args)
	{
		do_emplace<Decay<valueType>>(std::forward<Args>(args)...);
		Arg arg;
		this->m_manager(Op::Access, this, &arg);
		return *static_cast<Decay<valueType>*>(arg.m_obj);
	}

	template<typename valueType, typename Up, typename...Args>
	typename any_constructible<Decay<valueType>&,
								Decay<valueType>,
								std::initializer_list<Up>,
								Args&&...>::type
		emplace(std::initializer_list<Up> list, Args&&...args)
	{
		do_emplace<Decay<valueType>, Up>(list, std::forward<Args>(args)...);
		Arg arg;
		this->m_manager(Op::Access, this, &arg);
		return *static_cast<Decay<valueType>*>(arg.m_obj);
	}
	//todo swap

	const std::type_info & type() const noexcept
	{
		if(!hasValue())
		{
			return typeid(void);
		}
		Arg arg;
		m_manager(Op::GetTypeInfo,this, &arg);
		return *arg.m_typeinfo;
	}

	template<typename Tp>
	static constexpr bool isValidCast()
	{
		return std::is_reference<Tp>::value || std::is_copy_constructible<Tp>::value;
	}

private:
	enum class Op
	{
		Access,
		GetTypeInfo,
		Clone,
		Destroy,
		Move
	};

	union Arg
	{
		void * m_obj;
		const std::type_info * m_typeinfo;
		Any * thisObj;
	};

	template<typename T>
	struct ManagerInternal
	{
		template<typename Up>
		static void Create(Storage & s, Up && value)
		{
			void * addr = &s.m_buffer;
			new(addr) T(std::forward<Up>(value));
		}
		template<typename...Args>
		static void Create(Storage & s, Args&&... args)
		{
			void * addr = &s.m_buffer;
			new(addr) T(std::forward<Args>(args)...);
		}
		static void Manage(Op op,const Any * pAny, Arg * arg)
		{
			auto ptr = reinterpret_cast<const T *>(&pAny->m_storage.m_buffer);
			switch(op)
			{
				case Op::Access:
				arg->m_obj = const_cast<T*>(ptr);
				break;
				case Op::GetTypeInfo:
				arg->m_typeinfo = &typeid(T);
				break;
				case Op::Clone:
				::new(&arg->thisObj->m_storage.m_buffer) T(*ptr);
				arg->thisObj->m_manager = pAny->m_manager;
				break;
				case Op::Destroy:
				ptr->~T();
				break;
				case Op::Move:
				::new(&arg->thisObj->m_storage.m_buffer) T(std::move(*const_cast<T *>(ptr)));
				ptr->~T();
				arg->thisObj->m_manager = pAny->m_manager;
				const_cast<Any *>(pAny)->m_manager = nullptr;
				break;
			}
		}
	};

	template<typename T>
	struct ManagerExternal
	{
		template<typename Up>
		static void Create(Storage & s, Up && value)
		{
			s.m_ptr = new T(std::forward<Up>(value));
		}
		template<typename ...Args>
		static void Create(Storage & s, Args && ...args)
		{
			s.m_ptr = new T(std::forward<Args>(args)...);
		}

		static void Manage(Op op,const Any * pAny, Arg * arg)
		{
			auto ptr = reinterpret_cast<const T *>(pAny->m_storage.m_ptr);
			switch(op)
			{
				case Op::Access:
				arg->m_obj = const_cast<T*>(ptr);
				break;
				case Op::GetTypeInfo:
				arg->m_typeinfo = &typeid(T);
				break;
				case Op::Clone:
				arg->thisObj->m_storage.m_ptr = new T(*ptr);
				arg->thisObj->m_manager = pAny->m_manager;
				break;
				case Op::Destroy:
				delete ptr;
				break;
				case Op::Move:
				arg->thisObj->m_storage.m_ptr = pAny->m_storage.m_ptr;
				arg->thisObj->m_manager = pAny->m_manager;
				const_cast<Any *>(pAny)->m_manager = nullptr;
				break;
			}
		}
	};

private:
	template<typename valueType, bool>
	struct any_caster_impl;

	template<typename valueType>
	struct any_caster_impl<valueType, true>
	{
		inline static void * cast(const Any * pAny)
		{
			if (pAny->m_manager == &Manager<valueType>::Manage)
			{
				Arg arg;
				pAny->m_manager(Op::Access, pAny, &arg);
				return arg.m_obj;
			}
			return nullptr;
		}
	};

	template<typename valueType>
	struct any_caster_impl<valueType, false>
	{
		inline static void * cast(const Any * pAny)
		{
			return nullptr;
		}
	};

public:
	template<typename valueType>
	using any_caster = any_caster_impl<std::decay_t<valueType>, std::is_copy_constructible<std::decay_t<valueType>>::value>;

private:
	Storage m_storage;
	void (*m_manager)(Op,const Any *,Arg *);
};

//template<typename valueType, >
//void * any_caster(const Any * any)

template<typename valueType>
inline const valueType * any_cast(const Any * any) noexcept
{
	if (any)
	{
		return static_cast<valueType*>(Any::any_caster<valueType>::cast(any));
	}
	return nullptr;
}

template<typename valueType>
inline valueType * any_cast(Any * any) noexcept
{
	if (any)
	{
		return static_cast<valueType*>(Any::any_caster<valueType>::cast(any));
	}
	return nullptr;
}

template<typename valueType>
inline valueType any_cast(const Any & any)
{
	using Up = std::remove_cv_t<std::remove_reference_t<valueType>>;
	static_assert(Any::isValidCast<valueType>(),
		"template argument must be a reference or copy_constructible type");
	static_assert(std::is_constructible<valueType, const Up&>::value,
		"template argument must be constructible from a const value");

	auto p = any_cast<Up>(&any);
	if (p)
	{
		return static_cast<valueType>(*p);
	}
	throw BadAnyCast{};
}

template<typename valueType>
inline valueType any_cast(Any & any)
{
	using Up = std::remove_cv_t<std::remove_reference_t<valueType>>;
	static_assert(Any::isValidCast<valueType>(),
		"template argument must be a reference or copy_constructible type");
	static_assert(std::is_constructible<valueType, Up&>::value,
		"template argument must be constructible from a lvalue");

	auto p = any_cast<Up>(&any);
	if (p)
	{
		return static_cast<valueType>(*p);
	}
	throw BadAnyCast{};
}

template<typename valueType>
inline valueType any_cast(Any && any)
{
	using Up = std::remove_cv_t<std::remove_reference_t<valueType>>;
	static_assert(Any::isValidCast<valueType>(),
		"template argument must be a reference or copy_constructible type");
//here, different from libstdc++, Up&& --- Up
	static_assert(std::is_constructible<valueType, Up&&>::value,
		"template argument must be constructible from a lvalue");
	auto p = any_cast<Up>(&any);
	if (p)
	{
		return static_cast<valueType>(std::move(*p));
	}
	throw BadAnyCast{};
}

NAME_END

#endif // !ANY_H

