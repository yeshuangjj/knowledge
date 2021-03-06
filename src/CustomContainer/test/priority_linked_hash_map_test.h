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

#include "..\priority_linked_hash_map.h"

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
		void setName(const std::string name) { name_ = name; }
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



		template<typename linked_type>
		static void check_for_each(linked_type &linked)
		{
			cout << __FUNCTION__ << "**linked for : ***********************************************************************" << endl;
			for (auto &elem : linked)
			{
				cout << elem << endl;
			}

			cout << __FUNCTION__ << "**linked begin end***********************************************************************" << endl;
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**linked cbegin cend***********************************************************************" << endl;
			for (auto itr = linked.cbegin(); itr != linked.cend(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**linked rbegin rend***********************************************************************" << endl;
			for (auto itr = linked.rbegin(); itr != linked.rend(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**linked crbegin crend***********************************************************************" << endl;
			for (auto itr = linked.crbegin(); itr != linked.crend(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = linked.end() : ***********************************************************************" << endl;
			for (auto itr = linked.end(); itr != linked.begin(); )
			{
				cout << *(--itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = linked.cend() : ***********************************************************************" << endl;
			for (auto itr = linked.cend(); itr != linked.cbegin(); )
			{
				cout << *(--itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = linked.rend() : ***********************************************************************" << endl;
			for (auto itr = linked.rend(); itr != linked.rbegin(); )
			{
				cout << *(--itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = linked.crend() : ***********************************************************************" << endl;
			for (auto itr = linked.crend(); itr != linked.crbegin(); )
			{
				cout << *(--itr) << endl;
			}

			//��������const_linked	
			const linked_type & const_linked = linked;
			cout << __FUNCTION__ << "**const_linked for : ***********************************************************************" << endl;
			for (auto &elem : const_linked)
			{
				cout << elem << endl;
			}

			cout << __FUNCTION__ << "**const_linked begin end***********************************************************************" << endl;
			for (auto itr = const_linked.begin(); itr != const_linked.end(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**const_linked cbegin cend***********************************************************************" << endl;
			for (auto itr = const_linked.cbegin(); itr != const_linked.cend(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**const_linked rbegin rend***********************************************************************" << endl;
			for (auto itr = const_linked.rbegin(); itr != const_linked.rend(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**const_linked crbegin crend***********************************************************************" << endl;
			for (auto itr = const_linked.crbegin(); itr != const_linked.crend(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = const_linked.end() : ***********************************************************************" << endl;
			for (auto itr = const_linked.end(); itr != const_linked.begin(); )
			{
				cout << *(--itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = const_linked.cend() : ***********************************************************************" << endl;
			for (auto itr = const_linked.cend(); itr != const_linked.cbegin(); )
			{
				cout << *(--itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = const_linked.rend() : ***********************************************************************" << endl;
			for (auto itr = const_linked.rend(); itr != const_linked.rbegin(); )
			{
				cout << *(--itr) << endl;
			}

			cout << __FUNCTION__ << "**auto itr = const_linked.crend() : ***********************************************************************" << endl;
			for (auto itr = const_linked.crend(); itr != const_linked.crbegin(); )
			{
				cout << *(--itr) << endl;
			}
		}

		template<typename linked_type>
		static void check_some_operator(linked_type &linked)
		{
			auto itr_begin = linked.begin();
			auto itr_end = linked.end();
			
			{
				const linked_type &const_linked = linked;
				linked_type::const_iterator c_itr1 = const_linked.begin();
				linked_type::const_iterator c_itr2 = const_linked.end();
				linked_type::iterator itr1 = linked.begin();
				linked_type::iterator itr2 = linked.end();

				linked_type::const_iterator c_left1 = itr1;
				linked_type::const_iterator c_left2;
				c_left2 = itr1;

				--itr2;
				assert(itr1 == itr1);
				assert(c_itr1 == c_itr1);
				assert(c_itr1 == itr1);
				assert(itr1 == c_itr1);

				assert(itr1 != itr2);
				assert(c_itr1 != c_itr2);
				assert(itr1 != c_itr2);
				assert(c_itr1 != itr2);
			}
		}

		//���Ա���
		static void test0()
		{
			bool bExisted = false;
			typedef priority_linked_hash_map<int, Student, ObtainKeyOfStudent, false> linked_type;
			linked_type linked;
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
				cout << (*itr) << endl;

			linked.push_back(3, Student(STUDENT_PARAM(31)));
			linked.push_back(3, Student(STUDENT_PARAM(32)));
			linked.push_back(3, Student(STUDENT_PARAM(33)), bExisted);

			linked.push_back(1, Student(STUDENT_PARAM(11)));
			linked.push_back(1, Student(STUDENT_PARAM(12)));
			linked.push_back(1, Student(STUDENT_PARAM(13)), bExisted);

			linked.push_back(5, Student(STUDENT_PARAM(51)));
			linked.push_back(5, Student(STUDENT_PARAM(52)));
			linked.push_back(5, Student(STUDENT_PARAM(53)), bExisted);

			linked.push_back(4, Student(STUDENT_PARAM(41)));
			linked.push_back(4, Student(STUDENT_PARAM(42)));
			linked.push_back(4, Student(STUDENT_PARAM(43)), bExisted);

			linked.push_back(6, Student(STUDENT_PARAM(61)));
			linked.push_back(6, Student(STUDENT_PARAM(62)));
			linked.push_back(6, Student(STUDENT_PARAM(63)), bExisted);

			auto itr_begin1 = linked.begin();
			auto itr_end1 = linked.end();

			linked.push_back(2, Student(STUDENT_PARAM(21)));
			linked.push_back(2, Student(STUDENT_PARAM(22)));
			linked.push_back(2, Student(STUDENT_PARAM(23)), bExisted);

			linked.push_back(7, Student(STUDENT_PARAM(71)));
			linked.push_back(7, Student(STUDENT_PARAM(72)));
			linked.push_back(7, Student(STUDENT_PARAM(73)), bExisted);

			auto itr_begin2 = linked.begin();
			linked.push_back(0, Student(STUDENT_PARAM(1)));
			linked.push_back(0, Student(STUDENT_PARAM(2)));
			linked.push_back(0, Student(STUDENT_PARAM(3)), bExisted);
			auto itr_begin3 = linked.begin();			
			auto itr_end2 = linked.end();

			check_for_each<linked_type>(linked);
			check_some_operator<linked_type>(linked);

			assert(itr_end1== itr_end2);
			assert(itr_begin1 == itr_begin2);
			assert(itr_begin1 != itr_begin3);

			cout << __FUNCTION__ << "**erase 62 52 43***********************************************************************" << endl;
			for (auto itr = linked.begin(); itr != linked.end();)
			{
				cout << (*itr) << endl;
				if (itr->id() == 62)
					itr = linked.erase(itr);
				else if (itr->id() == 52)
					itr = linked.erase(itr);
				else if (itr->id() == 43)
					itr = linked.erase(itr);
				else
					++itr;
			}
			cout << "**************" << endl;
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << (*itr) << endl;
			}

			cout << __FUNCTION__ << "**remove 6***********************************************************************" << endl;
			linked.remove(61);
			linked.remove(62);
			linked.remove(63);
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << (*itr) << endl;
			}


			cout << __FUNCTION__ << "**remove 5***********************************************************************" << endl;
			linked.remove(51);
			linked.remove(52);
			linked.remove(53);
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << (*itr) << endl;
			}
			cout << __FUNCTION__ << "**remove 2***********************************************************************" << endl;
			linked.remove(21);
			linked.remove(22);
			linked.remove(23);
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << (*itr) << endl;
			}

		}

		static void test1()
		{
			bool bExisted = false;
			//typedef priority_linked_hash_map<int, Student> linked_type;
			typedef priority_linked_hash_map<int, Student, ObtainKeyOfStudent, false> linked_type;
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

			cout << __FUNCTION__ << "**begin end***********************************************************************" << endl;
			for (auto itr = linked.begin(); itr != linked.end(); ++itr)
			{
				cout << (*itr) << endl;
			}


			cout << __FUNCTION__ << "*************************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			cout << "is empty:" << linked.empty() << endl;
			linked.for_each(show);

			//remove
			cout << __FUNCTION__ << "**remove***********************************************************************" << endl;
			std::size_t wasExisted = linked.remove(20);
			wasExisted = linked.remove(9999);
			cout << "size:" << linked.size() << endl;
			cout << "is empty:" << linked.empty() << endl;
			linked.for_each(show);

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
			linked.for_each(show);

			//clear(priority)
			linked.clear(1);
			cout << __FUNCTION__ << "**clear(priority)***********************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			linked.for_each(show);

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
			linked.for_each_reverse(show);
			cout << __FUNCTION__ << "**otherLinked***********************************************************************" << endl;
			otherLinked.for_each_reverse(show);

			//const
			do {
				const auto &const_ref = otherLinked;
				auto &const_back = const_ref.back();
				auto &const_front = const_ref.front();
				cout << "size:" << const_ref.size() << endl;
				cout << "is empty:" << const_ref.empty() << endl;
				cout << __FUNCTION__ << "**const_ref for_each***********************************************************************" << endl;
				const_ref.for_each(show);
				cout << __FUNCTION__ << "**const_ref for_each_reverse***********************************************************************" << endl;
				const_ref.for_each_reverse(show);

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

		//����ָ�����
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
			linked.for_each(showPtr);

			//remove
			cout << __FUNCTION__ << "**remove***********************************************************************" << endl;
			std::size_t wasExisted = linked.remove(20);
			wasExisted = linked.remove(9999);
			cout << "size:" << linked.size() << endl;
			cout << "is empty:" << linked.empty() << endl;
			linked.for_each(showPtr);

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
			linked.for_each(showPtr);

			//clear(priority)
			linked.clear(1);
			cout << __FUNCTION__ << "**clear(priority)***********************************************************************" << endl;
			cout << "size:" << linked.size() << endl;
			linked.for_each(showPtr);

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
			linked.for_each_reverse(showPtr);
			cout << __FUNCTION__ << "**otherLinked***********************************************************************" << endl;
			otherLinked.for_each_reverse(showPtr);

			//const
			do {
				const auto &const_ref = otherLinked;
				auto &const_back = const_ref.back();
				auto &const_front = const_ref.front();
				cout << "size:" << const_ref.size() << endl;
				cout << "is empty:" << const_ref.empty() << endl;
				cout << __FUNCTION__ << "**const_ref for_each***********************************************************************" << endl;
				const_ref.for_each(showPtr);
				cout << __FUNCTION__ << "**const_ref for_each_reverse***********************************************************************" << endl;
				const_ref.for_each_reverse(showPtr);

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
			test0();
			test1();
			test2();
		}
	};
}
