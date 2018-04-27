#include "NameSpace.h"

NAME_BEGIN

template <class T> struct in_place_type_t {
	explicit in_place_type_t() = default;
};

//template <class T>
//inline constexpr in_place_type_t<T> in_place_type{};

#define in_place_type(T) in_place_type_t<T>{}

template<bool>
struct BoolDispatch;

template<>
struct BoolDispatch<true>
{
	typedef std::true_type type;
};

template<>
struct BoolDispatch<false>
{
	typedef std::false_type type;
};

NAME_END