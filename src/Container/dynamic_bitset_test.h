#pragma once
#include <string>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;


#include <boost/dynamic_bitset.hpp>
#include <boost/utility.hpp> //BOOST_BINARYºê

namespace dynamic_bitset_test
{
	class test_case
	{
	private:
		static void test1()
		{
			enum { bit_count = 8 };
			boost::dynamic_bitset<> db(bit_count, BOOST_BINARY(11110100));
			cout << db << endl;
			for (int i = 0; i < bit_count; ++i)
			{
				const bool b = db.test(i);
				cout << b << endl;
			}						
		}

		static void test2()
		{

		}

	public:
		static void test_all()
		{
			test1();
			test2();
		}
	};
}