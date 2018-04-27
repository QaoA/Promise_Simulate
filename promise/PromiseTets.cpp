#include "promise.h"
#include "NameSpace.h"
#include <gtest/gtest.h>
#include <iostream>

using namespace std;

USE_NAMESPACE;

TEST(PromiseTester, test1)
{
	Promise p = Promise([](Resolver r, Rejector j) {
		cout << "test1: promise accept: void f(void)" << endl;
		j(5);
	}).then([](Any a) {
		cout << "test1: then get a value : " << any_cast<int>(a) << endl;
	}).then([]() {
		cout << "test1: then last func return void" << endl;
		throw "test1: 1";
	}).Catch([](std::exception_ptr ptr){
		cout << "test1: catch a exception" << endl;
		try
		{
			std::rethrow_exception(ptr);
		}
		catch (int v)
		{
			cout << "value is " << v << endl;
		}
		throw "test1: 2";
	}).Catch([](std::exception_ptr ptr) {
		cout << "test1: catch a exception" << endl;
		try
		{
			std::rethrow_exception(ptr);
		}
		catch (const char * p)
		{
			cout << p << endl;
		}
		catch (...)
		{
			cout << "test1: catch a unkonwn exception" << endl;
		}
	}).then([]() {
		cout << "test1: have dealed with exception" << endl;
	});
}

TEST(PromiseTester, test2)
{
	Promise p = Promise([](Resolver r) {
		cout << "test2: promise accept: void f(resolve)" << endl;
		r(5);
	}).then([](Any a) {
		return Promise([](Resolver r) {
			cout << "test2: second promise" << endl;
			r("hello world");
		});
	}).then([](Any a) {
		cout << "test2: get a value : " << any_cast<const char *>(a) << endl;
	}).Catch([](std::exception_ptr ptr) {
		cout << "test2: catch a exception" << endl;
	});
}

TEST(PromiseTester, test3)
{
	Promise p([](Resolver r, Rejector j) {
		cout << "test3: promise accept: void f(resolve, reject)" << endl;
		j(3);
	});
}