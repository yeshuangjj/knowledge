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

		Student(const Student &other)
		{
			this->id_ = other.id_;
			this->name_ = other.name_;
			cout << __FUNCTION__ << ":copy construct id:" << id_ << "  name:" << name_ << endl;
		}

		Student &operator=(const Student &other)
		{
			this->id_ = other.id_;
			this->name_ = other.name_;
			cout << __FUNCTION__ << ":assign id:" << id_ << "  name:" << name_ << endl;
			return *this;
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
		static void show(const Student&obj)
		{
			cout << obj << endl;
		}

		static void showPtr(const Student_sptr &spStudent)
		{
			cout << spStudent << endl;
		}

		static void test1()
		{
			bool bExisted = false;
			//typedef priority_linked_hash_map<int, Student> linked_type;
			typedef priority_linked_hash_map<int, Student, ObtainKeyOfStudent, true> linked_type;
			linked_type linked;
			linked.push_back(1, Student(STUDENT_PARAM(11)));
			linked.push_back(1, Student(STUDENT_PARAM(12)));
			linked.push_back(1, Student(STUDENT_PARAM(13)), bExisted);
			linked.push_back(1, Student(STUDENT_PARAM(12 * 1)), bExisted);

			linked.push_back(0, Student(STUDENT_PARAM(1)));
			linked.push_back(0, Student(STUDENT_PARAM(2)));
			linked.push_back(0, Student(STUDENT_PARAM(3)), bExisted);
			linked.push_back(0, Student(STUDENT_PARAM(1 * 1)), bExisted);

			linked.push_back(2, Student(STUDENT_PARAM(20)));
			linked.push_back(2, Student(STUDENT_PARAM(21)));
			linked.push_back(2, Student(STUDENT_PARAM(22)));
			linked.push_back(2, Student(STUDENT_PARAM(23)), bExisted);
			linked.push_back(2, Student(STUDENT_PARAM(23 + 0)), bExisted);
			linked.push_back(2, Student(STUDENT_PARAM(21 + 0)), bExisted);

			cout << __FUNCTION__ << "*************************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			cout << "is empty:" << linked.empty() << endl;
			linked.traverse(show);

			//remove
			cout << __FUNCTION__ << "**remove***********************************************************************" << endl;
			bool wasExisted = linked.remove(20);
			wasExisted = linked.remove(9999);
			cout << "size:" << linked.size() << endl;
			cout << "is empty:" << linked.empty() << endl;
			linked.traverse(show);

			//pop
			if (linked.empty() == false)
			{
				auto &front = linked.front();
				auto &back = linked.back();

				linked.pop_back();
				linked.pop_front();
			}
			cout << __FUNCTION__ << "**pop***********************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			linked.traverse(show);

			//clear(priority)
			linked.clear(1);
			cout << __FUNCTION__ << "**clear(priority)***********************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			linked.traverse(show);

			//find
			do
			{
				cout << __FUNCTION__ << "**find***********************************************************************" << endl;
				auto pFind = linked.find(3);
				if (pFind)
				{
					cout << *pFind << endl;
				}
				pFind = linked.find(99);
				assert(pFind == nullptr);

				Student s;
				if (linked.find_elem(3, s))
				{
					cout << s << endl;
				}

				assert(linked.find_elem(99999, s) == false);

			} while (false);



			linked_type otherLinked;
			linked.swap(otherLinked);
			cout << __FUNCTION__ << "**linked***********************************************************************" << endl;
			linked.traverse_reverse(show);
			cout << __FUNCTION__ << "**otherLinked***********************************************************************" << endl;
			otherLinked.traverse_reverse(show);

			//const
			do {
				const auto &const_ref = otherLinked;
				auto &const_back = const_ref.back();
				auto &const_front = const_ref.front();
				cout << "size:" << const_ref.size() << endl;
				cout << "is empty:" << const_ref.empty() << endl;
				cout << __FUNCTION__ << "**const_ref traverse***********************************************************************" << endl;
				const_ref.traverse(show);
				cout << __FUNCTION__ << "**const_ref traverse_reverse***********************************************************************" << endl;
				const_ref.traverse_reverse(show);

				//find
				do
				{
					cout << __FUNCTION__ << "**find const***********************************************************************" << endl;
					auto pFind = const_ref.find(3);
					if (pFind)
					{
						cout << *pFind << endl;
					}
					pFind = const_ref.find(99);
					assert(pFind == nullptr);
				} while (false);
			} while (false);

		}

		//ÖÇÄÜÖ¸Õë²âÊÔ
		static void test2()
		{
			bool bExisted = false;
			typedef priority_linked_hash_map<int, Student_sptr> linked_type;
			linked_type linked;
			linked.push_back(1, stdcxx::make_shared<Student>(STUDENT_PARAM(11)));
			linked.push_back(1, stdcxx::make_shared<Student>(STUDENT_PARAM(12)));
			linked.push_back(1, stdcxx::make_shared<Student>(STUDENT_PARAM(13)), bExisted);
			linked.push_back(1, stdcxx::make_shared<Student>(STUDENT_PARAM(12 * 1)), bExisted);

			linked.push_back(0, stdcxx::make_shared<Student>(STUDENT_PARAM(1)));
			linked.push_back(0, stdcxx::make_shared<Student>(STUDENT_PARAM(2)));
			linked.push_back(0, stdcxx::make_shared<Student>(STUDENT_PARAM(3)), bExisted);
			linked.push_back(0, stdcxx::make_shared<Student>(STUDENT_PARAM(3 * 1)), bExisted);

			linked.push_back(2, stdcxx::make_shared<Student>(STUDENT_PARAM(20)));
			linked.push_back(2, stdcxx::make_shared<Student>(STUDENT_PARAM(21)));
			linked.push_back(2, stdcxx::make_shared<Student>(STUDENT_PARAM(22)));
			linked.push_back(2, stdcxx::make_shared<Student>(STUDENT_PARAM(23)), bExisted);
			linked.push_back(2, stdcxx::make_shared<Student>(STUDENT_PARAM(23 * 1)), bExisted);

			cout << __FUNCTION__ << "*************************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			cout << "is empty:" << linked.empty() << endl;
			linked.traverse(showPtr);

			//remove
			cout << __FUNCTION__ << "**remove***********************************************************************" << endl;
			bool wasExisted = linked.remove(20);
			wasExisted = linked.remove(9999);
			cout << "size:" << linked.size() << endl;
			cout << "is empty:" << linked.empty() << endl;
			linked.traverse(showPtr);

			//pop
			if (linked.empty() == false)
			{
				auto &front = linked.front();
				auto &back = linked.back();

				linked.pop_back();
				linked.pop_front();
			}
			cout << __FUNCTION__ << "**pop***********************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			linked.traverse(showPtr);

			//clear(priority)
			linked.clear(1);
			cout << __FUNCTION__ << "**clear(priority)***********************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			linked.traverse(showPtr);

			//find
			do
			{
				cout << __FUNCTION__ << "**find***********************************************************************" << endl;
				auto pFind = linked.find(3);
				if (pFind)
				{
					cout << *pFind << endl;
				}
				pFind = linked.find(99);
				assert(pFind == nullptr);

				Student_sptr spStudent;
				if (linked.find_elem(3, spStudent))
					cout << spStudent << endl;

				//Student_sptr spStudent1;
				if (linked.find_elem(99, spStudent))
					cout << spStudent << endl;

			} while (false);



			linked_type otherLinked;
			linked.swap(otherLinked);
			cout << __FUNCTION__ << "**linked***********************************************************************" << endl;
			linked.traverse_reverse(showPtr);
			cout << __FUNCTION__ << "**otherLinked***********************************************************************" << endl;
			otherLinked.traverse_reverse(showPtr);

			//const
			do {
				const auto &const_ref = otherLinked;
				auto &const_back = const_ref.back();
				auto &const_front = const_ref.front();
				cout << "size:" << const_ref.size() << endl;
				cout << "is empty:" << const_ref.empty() << endl;
				cout << __FUNCTION__ << "**const_ref traverse***********************************************************************" << endl;
				const_ref.traverse(showPtr);
				cout << __FUNCTION__ << "**const_ref traverse_reverse***********************************************************************" << endl;
				const_ref.traverse_reverse(showPtr);

				//find
				cout << __FUNCTION__ << "**find const***********************************************************************" << endl;
				auto pFind = const_ref.find(3);
				if (pFind)
				{
					cout << *pFind << endl;
				}
				pFind = const_ref.find(99);
				assert(pFind == nullptr);


				Student_sptr spStudent;
				if (const_ref.find_elem(3, spStudent))
					cout << spStudent << endl;

				Student_sptr spStudent1;
				if (const_ref.find_elem(99, spStudent1))
					cout << spStudent << endl;

			} while (false);

			otherLinked.clear();
		}

	public:
		static void test_all()
		{
			test1();
			//test2();
		}
	};
}
