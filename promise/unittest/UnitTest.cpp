#include <gtest/gtest.h>
#include "../ThreadPool.h"
#include <vector>
#include <numeric>
#include <iostream>
#include <chrono>
#include <string>

using namespace std;
using namespace std::chrono;

//TEST(ThreadPoolTest, test)
//{
//	ThreadPool p(100, 4);
//	p.start();
//	
//	std::vector<int> v;
//	v.reserve(10000);
//	for (int i = 0; i < 10000; ++i)
//	{
//		v.push_back(i);
//	}
//	for (int i = 0; i < 200; ++i)
//	{
//		p.submit([&v] {
//			int sum = std::accumulate(v.begin(), v.end(), 0);
//			std::cout << sum << std::endl;
//		});
//	}
//
//	std::this_thread::sleep_for(5s);
//}