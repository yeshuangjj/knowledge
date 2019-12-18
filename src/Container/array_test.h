#pragma once
#include <string>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#ifndef DISABLE_STD
#include <array>
#include <memory>
#else
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif


namespace stdcxx
{
#ifndef DISABLE_STD
	using ::std::array;
	using ::std::shared_ptr;
	using ::std::make_shared;
#else
	using ::boost::array;
	using ::boost::shared_ptr;
	using ::boost::make_shared;
#endif
}

namespace array_test
{
	class test_case
	{

	private:
		static void test1()
		{
			enum { count = 10 };
			//stdcxx::array<int, count> arr = { 1,2,3,4,5,6,7,8,9,10 };
			stdcxx::array<int, count> arr = { 1,2,3,4 };
			//for (int i = 0; i < count; ++i)
			//{
			//	arr[i] = i + 11;
			//}

			arr.empty();
			arr.front();
			arr.back();

			for (auto itr = arr.begin(); itr != arr.end(); ++itr)
			{
				cout << *itr << endl;
			}

			cout << "*****************************************" << endl;

			for (auto &elem : arr)
			{
				cout << elem << endl;
			}
		}

		static void test2()
		{
			class Student
			{
			public:
				Student(int age, const std::string &name) :age_(age), name_(name) {}
				~Student() {}

				int age()const { return age_; }
				const std::string &name()const { return name_; }
				void show()
				{
					cout << "age:" << age_ << "  name:" << name_ << endl;
				}
			private:
				int age_;
				std::string name_;
			};
			typedef stdcxx::shared_ptr<Student> Student_sptr;

			enum { count = 10 };
			stdcxx::array<Student_sptr, count> arr;

			for (int i = 0; i < count; ++i)
			{
				arr[i] = stdcxx::make_shared<Student>(i, "a");
			}

			for (auto itr = arr.begin(); itr != arr.end(); ++itr)
			{
				if (*itr)
				{
					(*itr)->show();
				}
			}


		}
	public:
		static void test_all()
		{
			test1();
			test2();
		}
	};




}