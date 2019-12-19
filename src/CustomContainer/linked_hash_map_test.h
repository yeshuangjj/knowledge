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
#define STUDENT_PARAMS(id) id,id,#id
	class Student
	{
	public:
		Student() :id_(-1), age_(-1) {
			//cout << __FUNCTION__ << "default construct id:" << id_ << "age:" << age_ << "  name:" << name_ << endl; 
		}
		Student(int id, int age, const std::string &name) :id_(id), age_(age), name_(name) {
			//cout << __FUNCTION__ << "construct id:" << id_ << "age:" << age_ << "  name:" << name_ << endl; 
		}
		~Student() {
			//cout << __FUNCTION__ << "destroy id:" << id_ << "age:" << age_ << "  name:" << name_ << endl; 
		}

		int id()const { return id_; }
		int age()const { return age_; }
		const std::string& name()const { return name_; }

		void show()
		{
			cout << __FUNCTION__ << "id:" << id_ << "age:" << age_ << "  name:" << name_ << endl;
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
			cout << __FUNCTION__ << "**********************************************************" << endl;
			linked_hash_map<int, Student, true> linked;
			typedef linked_hash_map<int, Student>::list_value_type list_value_type;
			bool bExisted = true;
			Student s1(1, 1, "a1");
			Student s2(2, 2, "a2");
			Student s3(3, 3, "a3");
			Student s4(4, 4, "a4");

			//push_back
			//std::make_pair
			linked.push_back(std::make_pair(s1.id(), s1));
			linked.push_back(std::make_pair(s2.id(), s2));
			linked.push_back(std::make_pair(s3.id(), s3), bExisted);
			linked.push_back(std::make_pair(s4.id(), s4), bExisted);
			//list_value_type
			auto itr5 = linked.push_back(list_value_type(5, Student(5, 5, "a5")));
			auto itr6 = linked.push_back(list_value_type(6, Student(6, 6, "a6")));
			auto itr7 = linked.push_back(list_value_type(7, Student(7, 7, "a7")), bExisted);
			linked.push_back(list_value_type(8, Student(8, 8, "a8")), bExisted);

			auto itr21 = linked.push_back(21, Student(21, 21, "a21"));
			auto itr22 = linked.push_back(22, Student(22, 22, "a22"));
			auto itr23 = linked.push_back(23, Student(23, 23, "a23"), bExisted);
			auto itr24 = linked.push_back(24, Student(24, 24, "a24"), bExisted);

			
			itr24 = linked.push_back(24, Student(STUDENT_PARAMS(2444)), bExisted); //重复

			//push_front
			linked.push_front(std::make_pair(-1, Student(STUDENT_PARAMS(-1))));
			linked.push_front(list_value_type(-2, Student(STUDENT_PARAMS(-2))));
			linked.push_front(std::make_pair(-3, Student(STUDENT_PARAMS(-3))), bExisted);
			linked.push_front(list_value_type(-4, Student(STUDENT_PARAMS(-4))), bExisted);

			linked.push_front(-5, Student(STUDENT_PARAMS(-5)));
			linked.push_front(-6, Student(STUDENT_PARAMS(-6)), bExisted);
			linked.push_front(-6, Student(STUDENT_PARAMS(-66666666)), bExisted); //重复

			//特殊测试			
			//linked.erase(itr23);  修正：push_back不返回迭代器，返回的迭代器容易造成误用，最好还是和 标准库的行为一致
			//linked.erase(23);
			itr23 = linked.insert(itr23, 23, Student(STUDENT_PARAMS(2333333))); //重复

			//insert itr
			linked.insert(itr23, 33, Student(STUDENT_PARAMS(33)));
			linked.insert(itr24, 34, Student(STUDENT_PARAMS(34)));
			linked.insert(itr21, 31, Student(STUDENT_PARAMS(31)), bExisted);
			linked.insert(itr22, 32, Student(STUDENT_PARAMS(32)), bExisted);

			linked.insert(itr23, list_value_type(43, Student(STUDENT_PARAMS(43))));
			linked.insert(itr23, std::make_pair(53, Student(STUDENT_PARAMS(53))));
			linked.insert(itr24, list_value_type(44, Student(STUDENT_PARAMS(44))), bExisted);
			linked.insert(itr24, std::make_pair(54, Student(STUDENT_PARAMS(54))), bExisted);

			//insert key
			linked.insert(23, 63, Student(STUDENT_PARAMS(63)));
			linked.insert(23, 73, Student(STUDENT_PARAMS(73)));
			linked.insert(24, 64, Student(STUDENT_PARAMS(64)), bExisted);
			linked.insert(24, 74, Student(STUDENT_PARAMS(74)), bExisted);

			linked.insert(23, list_value_type(83, Student(STUDENT_PARAMS(83))));
			linked.insert(23, std::make_pair(93, Student(STUDENT_PARAMS(93))));
			linked.insert(24, list_value_type(84, Student(STUDENT_PARAMS(84))), bExisted);
			linked.insert(24, std::make_pair(94, Student(STUDENT_PARAMS(94))), bExisted);

			for (auto &elem : linked)
			{
				cout << elem.second << endl;
			}

			cout << __FUNCTION__ << "**********************************************************" << endl;

			//erase
			decltype(itr23._Getcont()) t = itr23._Getcont();
			linked.erase(23);
			t = itr23._Getcont();
			linked.erase(itr24);
			t = itr24._Getcont();

			//find
			auto itrFind = linked.find(23);
			assert(itrFind == linked.end());

			itrFind = linked.find(24);
			assert(itrFind == linked.end());

			itrFind = linked.find(101);
			assert(itrFind == linked.end());

			itrFind = linked.find(5);
			assert(itrFind != linked.end());

			itrFind = linked.find(2);
			assert(itrFind != linked.end());
			auto itr102 = linked.insert(itrFind, 102, Student(STUDENT_PARAMS(102)));

			linked.insert(itr102, 112, Student(STUDENT_PARAMS(112)));

			//insert same key
			auto itr8 = linked.insert(itr102, 8, Student(STUDENT_PARAMS(888)));

			linked.insert(itr8, 108, Student(STUDENT_PARAMS(108)));

			//遍历
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << itr->second << endl;
			}

			cout << __FUNCTION__ << "**********************************************************" << endl;

			//pop
			
			linked.pop_back();
			linked.pop_back();
			linked.pop_front();
			linked.pop_front();

			auto &elem_back = linked.back();
			elem_back.second = Student(STUDENT_PARAMS(99999));
			auto &elem_front = linked.front();
			elem_front.second = Student(STUDENT_PARAMS(11111111));

			for (auto rItr = linked.rbegin(); rItr != linked.rend(); ++rItr)
			{
				cout << rItr->second << endl;
			}
		}

		static void test2()
		{
			cout << __FUNCTION__ << "**********************************************************" << endl;
			linked_hash_map<int, Student> linked;
		}

		//智能指针
		static void test3()
		{
			cout << __FUNCTION__ << "**********************************************************" << endl;
			linked_hash_map<int, Student_sptr> linked;

		}
	public:
		static void test_all()
		{
			test1();
			//test2();
			test3();
		}
	};
}
