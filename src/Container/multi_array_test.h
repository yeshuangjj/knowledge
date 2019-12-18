#pragma once
#include <string>
#include <iostream>
#include <vector>
using std::cout;
using std::cin;
using std::endl;

#ifndef DISABLE_STD
#include <memory>
#else
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif

#include <boost/multi_array.hpp>

namespace stdcxx
{
#ifndef DISABLE_STD
	using ::std::shared_ptr;
	using ::std::make_shared;
#else
	using ::boost::shared_ptr;
	using ::boost::make_shared;
#endif

	
}

using ::boost::multi_array;

namespace multi_array_test
{
	class test_case
	{
	private:
	public:
	};
}
