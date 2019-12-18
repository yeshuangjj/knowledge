#pragma once

#include <string>
#include <iostream>
#include <vector>
using std::cout;
using std::cin;
using std::endl;

#ifndef DISABLE_STD
#include <tuple>
#include <memory>
#else
#include <boost/tuple/tuple.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif

#include <boost/any.hpp>

namespace stdcxx
{
#ifndef DISABLE_STD
	using ::std::tuple;
	using ::std::shared_ptr;
	using ::std::make_shared;
#else
	using ::boost::tuple;
	using ::boost::shared_ptr;
	using ::boost::make_shared;
#endif

	using ::boost::any;
	using ::boost::any_cast;
	using ::boost::bad_any_cast;
}


//¿‡À∆”ÎQtµƒ QVariant
namespace any_test
{
	class Student
	{
	public:
		Student(int age, const std::string &name) :age_(age), name_(name) 
		{
			cout <<__FUNCTION__ <<" age:" << age_ << "  name:" << name_ << endl;
		}
		~Student() { cout << __FUNCTION__ << " age:" << age_ << "  name:" << name_ << endl; }

		int age()const { return age_; }
		const std::string &name()const { return name_; }
		void show()
		{
			cout << __FUNCTION__ << " age:" << age_ << "  name:" << name_ << endl;
		}
	private:
		int age_;
		std::string name_;
	};
	typedef stdcxx::shared_ptr<Student> Student_sptr;

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student &student)
	{
		strm << "age:" << student.age() << "\t" << student.name();
		return strm;
	}


	template<typename CharT, typename TraitsT>
	inline std::basic_ostream<CharT, TraitsT> &operator<<(std::basic_ostream< CharT, TraitsT >& strm, const Student_sptr &spStudent)
	{
		strm << "age:" << spStudent->age() << "\t" << spStudent->name();
		return strm;
	}

	class test_case
	{
	private:
		static void test1()
		{
			//stdcxx::any a;
			//a = 1;
			//a = "hello";
		}

		static void test2()
		{
			std::vector<stdcxx::any> vec;
			vec.push_back(1);
			vec.push_back(std::string("tiankong"));
			vec.push_back(Student(5, "xiaoming"));
			vec.push_back(Student_sptr(new Student(28, "yeshuang")));

			stdcxx::any_cast<int>(vec.at(0));
			stdcxx::any_cast<std::string>(vec.at(1));
			stdcxx::any_cast<Student>(vec.at(2));
			stdcxx::any_cast<Student_sptr>(vec.at(3));


			for (auto &elem : vec)
			{
				cout << elem.type().hash_code() << "\t" << elem.type().name() << "\t" << elem.type().raw_name() << endl;;
			}

		}

	public:
		static void test_all()
		{
			test1();
			test2();
		}
	private:

	};
}

