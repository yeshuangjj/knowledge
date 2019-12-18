#pragma once
#include <string>
#include <iostream>
#include <vector>
using std::cout;
using std::cin;
using std::endl;

#include <boost/circular_buffer.hpp>
//#include <boost/circular_buffer_fwd.hpp>  //模板前向声明，值得借鉴

namespace circular_buffer_test
{
	class test_case
	{
	private:
		template<typename T>
		static void show(const boost::circular_buffer<T> &cb)
		{
			cout << "*******************************" << endl;
			for (auto &elem : cb)
			{
				cout << elem << endl;
			}
		}

		static void test1()
		{
			boost::circular_buffer<int> cb(3);
			cb.push_back(1);
			cb.push_back(2);
			cb.push_back(3);
			show(cb);

			cb.push_back(4);
			show(cb);
			cb.push_back(5);
			show(cb);
			cb.push_back(6);
			show(cb);
			cb.push_back(7);
			show(cb);
		}
	public:
		static void test_all()
		{
			test1();
		}
	};
}
