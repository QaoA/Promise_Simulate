#include "Any.h"
#include "NameSpace.h"
#include <gtest/gtest.h>
#include <string>

using namespace std;

USE_NAMESPACE;

class Foo1
{
public:
	Foo1(int v1, int v2, const char * str) :
		v1(v1), v2(v2), str(str)
	{}

	void operator()()
	{
		cout << v1 << v2 << str << endl;
	}

	void set(int v)
	{
		v1 = v;
	}

private:
	int v1;
	int v2;
	const char * str;
};

class Foo2
{
public:
	Foo2(initializer_list<int> l, const char * pstr) :
		v(l), s(pstr)
	{}

	void operator()()
	{
		for (auto it = v.begin(); it != v.end(); ++it)
		{
			cout << *it << endl;
		}
		cout << s << endl;
	}

private:
	vector<int> v;
	string s;
};


TEST(AnyTester, test1)
{
	Any a;
}

TEST(AnyTester, test2)
{
	Any a(5);
	Any b(std::move(a));

	cout << any_cast<int>(b) << endl;
}

TEST(AnyTester, test3)
{
	Any a(in_place_type(string), "hello world");
	Any b(a);

	cout << any_cast<string>(b) << endl;
}

TEST(AnyTester, test4)
{
	Any a(in_place_type(Foo1), 1, 2, "hello world");
	any_cast<Foo1&>(a).set(0xff);
	any_cast<Foo1&>(a)();
}

TEST(AnyTester, test5)
{
	Any a;
	a.emplace<int>(6);
	cout << any_cast<int>(a) << endl;
}

TEST(AnyTester, test6)
{
	Any a(5);
	a.emplace<float>(3.14f);
	cout << any_cast<float>(a) << endl;
}

//TEST(AnyTester, test5)
//{
//	//Any a(in_place_type(Foo2), { 1,2,3,4,5 }, "hello world");
//}