#pragma once
#include <iostream>
#include <string>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#ifndef DISABLE_STD
#include <memory>
#else
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif

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

#include "priority_linked_hash_map.h"

namespace priority_linked_hash_map_test
{
#ifdef STUDENT_PARAM
#undef STUDENT_PARAM
#endif
#define STUDENT_PARAM(id) id,#id
	class Student
	{
	public:
		Student() :id_(-1) {
			cout << __FUNCTION__ << ":default construct id:" << id_ << "  name:" << name_ << endl;
		}
		Student(int id, const std::string &name) :id_(id), name_(name) {
			cout << __FUNCTION__ << ":custom construct id:" << id_ << "  name:" << name_ << endl;
		}
		~Student() {
			cout << __FUNCTION__ << ":destroy id:" << id_ << "  name:" << name_ << endl;
		}
		inline const int &id()const { return id_; }
		inline const int &key()const { return id_; }
		const std::string &name()const { return  name_; }
	private:
		int id_;
		std::string name_;
	};
	typedef stdcxx::shared_ptr<Student> Student_sptr;

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student &student)
	{
		strm << "id:" << student.id() << "\t name:" << student.name();
		return strm;
	}

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student_sptr spStudent)
	{
		strm << "id:" << spStudent->id() << "\t name:" << spStudent->name();
		return strm;
	}

	struct ObtainKeyOfStudent
	{
		inline const int& operator()(const Student&l)const
		{
			return l.id();
		}
	};

	struct ObtainKeyOfStudentPtr
	{
		inline const int& operator()(const Student_sptr &sp)const
		{
			assert(sp);
			return sp->id();
		}
	};

	class test_case
	{
	private:
		static void test1()
		{
			priority_linked_hash_map<> linked
		}
	public:
		static void test_all()
		{

		}
	};
}
