#pragma once
#include <string>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#ifndef DISABLE_STD
#include <memory>
#include <xhash>
#else
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif

#include <boost/container_hash/hash.hpp>



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

#include "linked_hash_map.h"

namespace linked_hash_map_test
{
	class Student
	{
	public:
		Student() :id_(-1), age_(-1) { cout << __FUNCTION__ << "default construct id:" << id_ << "age:" << age_ << "  name:" << name_ << endl; }
		Student(int id, int age, const std::string &name) :id_(id), age_(age), name_(name) { cout << __FUNCTION__ << "construct id:" << id_ << "age:" << age_ << "  name:" << name_ << endl; }
		~Student() { cout << __FUNCTION__ << "destroy id:" << id_ << "age:" << age_ << "  name:" << name_ << endl; }

		int id()const { return id_; }
		int age()const { return age_; }
		const std::string& name()const { return name_; }

		void show()
		{
			cout << __FUNCTION__ <<"id:" << id_ << "age:" << age_ << "  name:" << name_ << endl;
		}
	public:
		int id_;
		int age_;
		std::string name_;
	};
	typedef stdcxx::shared_ptr<Student> Student_sptr;

	struct StudentEqualCompare
	{
		bool operator()(const Student &l, const Student &r)const
		{
			return l.id() == r.id() && l.age() == r.age() && l.name() == r.name();
		}
	};

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student &student)
	{
		strm << "id:" << student.id() << "\t age:" << student.age() << "\t name:" << student.name();
		return strm;
	}

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student_sptr spStudent)
	{
		strm << "id:" << spStudent->id() << "\t age:" << spStudent->age() << "\t name:" << spStudent->name();
		return strm;
	}

	struct StudentHash
	{
		std::size_t operator()(const Student &l)const
		{
			std::size_t seed = 0u;
			boost::hash_combine(seed, boost::hash_value(l.id()));
			boost::hash_combine(seed, boost::hash_value(l.age()));
			boost::hash_combine(seed, boost::hash_value(l.name()));
			return seed;
		}
	};
	

	class test_case
	{
	private:
		//普通对象
		static void test1()
		{
			cout <<__FUNCTION__ <<"**********************************************************" << endl;
			linked_hash_map<int, Student> linked;
			typedef linked_hash_map<int, Student>::list_value_type list_value_type;
			//Student s0(0, 20, "a0");		
			//Student s1(1, 21, "a1");
			//Student s2(2, 22, "a2");
			//Student s3(3, 23, "a3");
			//Student s4(4, 24, "a4");
			//Student s5(5, 25, "a5");
			//Student s6(6, 26, "a6");
			//linked.push_back(std::make_pair(s0.id(), s0));
			//linked.push_back(std::make_pair(s1.id(), s1));
			//linked.push_back(std::make_pair(s2.id(), s2));
			//linked.push_back(std::make_pair(s3.id(), s3));
			//linked.push_back(std::make_pair(s4.id(), s4));
			//linked.push_back(std::make_pair(s5.id(), s5));
			//linked.push_back(std::make_pair(s6.id(), s6));

			int id = 0;
			//linked.push_back(id, Student(0, 0, "a0"));
			linked.push_back(1, Student(1, 1, "a1"));

			linked.push_back(std::make_pair(2, Student(2, 2, "a2")));
			linked.push_back(list_value_type(3, Student(3, 3, "a3")));

			linked.insert(1, 11, Student(11, 11, "a11"));

			id = 2;
			linked.insert(id, 12, Student(12, 12, "a12"));

			

			for (auto &elem : linked)
			{
				cout << elem.second << endl;
			}

			cout << __FUNCTION__ << "**********************************************************" << endl;

			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << itr->second << endl;
			}
		}

		//智能指针
		static void test2()
		{
			cout << __FUNCTION__ << "**********************************************************" << endl;
			linked_hash_map<int, Student_sptr> linked;

			


		}
	public:
		static void test_all()
		{
			test1();
			test2();
		}
	};
}
