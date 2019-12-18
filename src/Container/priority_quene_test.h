#pragma once
#include <windows.h>
#include <string>
#include <iostream>
using std::cout;
using std::cin;
using std::endl;

#ifndef DISABLE_STD
#include <queue>
#include <concurrent_priority_queue.h>
#include <memory>
#else
#include <boost/heap/priority_queue.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#endif

namespace stdcxx
{
#ifndef DISABLE_STD
	using ::std::priority_queue;
	using ::std::shared_ptr;
	using ::std::make_shared;
#else
	using ::boost::heap::priority_queue;
	using ::boost::shared_ptr;
	using ::boost::make_shared;
#endif
}

namespace priority_quene_test
{
	class Student
	{
	public:
		Student() :id_(-1), age_(-1), timestamp_(-1) {}
		Student(int id, int age, time_t timestamp, const std::string &name) :id_(id), age_(age), timestamp_(timestamp), name_(name) {}

		bool operator<(const Student &other)const
		{
			if (age_ == other.age_)
				return timestamp_ < other.timestamp_;
			else
				return age_ < other.age_;
		}

		int id()const { return id_; }
		int age()const { return age_; }
		time_t timestamp()const { return timestamp_; }
		const std::string& name()const { return name_; }
	private:
		int id_;
		int age_;
		time_t timestamp_;
		std::string name_;
	};
	typedef stdcxx::shared_ptr<Student> Student_sptr;

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student &student)
	{
		strm << "id:" << student.id() << "\t" << "age:" << student.age() << "\t" << student.timestamp() << "\t" << student.name();
		return strm;
	}

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student_sptr spStudent)
	{
		strm << "id:" << spStudent->id() << "\t" << "age:" << spStudent->age() << "\t" << spStudent->timestamp() << "\t" << spStudent->name();
		return strm;
	}

	struct CompareOfStrudent_ASC
	{
		bool operator()(const Student&left, const Student &right)const
		{
			if (left.age() == right.age())
				return left.timestamp() > right.timestamp();
			else
				return left.age() > right.age();
		}
	};

	struct CompareOfStrudent_DESC
	{
		bool operator()(const Student&left, const Student &right)const
		{
			if (left.age() == right.age())
				return left.timestamp() < right.timestamp();
			else
				return left.age() < right.age();
		}
	};

	struct CompareOfStrudentPtr_ASC
	{
		bool operator()(const Student_sptr&left, const Student_sptr &right)const
		{
			if (left->age() == right->age())
				return left->timestamp() > right->timestamp();
			else
				return left->age() > right->age();
		}
	};

	struct CompareOfStrudentPtr_DESC
	{
		bool operator()(const Student_sptr&left, const Student_sptr &right)const
		{
			if (left->age() == right->age())
				return left->timestamp() < right->timestamp();
			else
				return left->age() < right->age();
		}
	};

	class test_case
	{
	private:

#ifndef DISABLE_STD
		template<typename Elem, typename Container, typename Pr>
		static void traverse(stdcxx::priority_queue<Elem, Container, Pr> & p_q)
		{
			std::vector<Elem> vec;
			vec.reserve(p_q.size());

			while (p_q.empty() == false)
			{
				vec.push_back(p_q.top());
				p_q.pop();
			}

			for (auto &elem : vec)
				cout << elem << endl;

			cout << "**********************************************" << endl;
		}

		static void test1()
		{
			//缺点：优先级相同的元素，顺序是不确定的
			//解决方案:Student增加一个成员变量：时间戳 timestamp,修改 CompareOfStrudent_ASC [timestamp相同，顺序依旧不确定，依旧无法解决该问题]
			stdcxx::priority_queue<Student, std::vector<Student>, CompareOfStrudent_ASC> p_q;
			//stdcxx::priority_queue<Student> p_q;

			p_q.push(Student(0, 15, ::GetTickCount64(), "a1"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a2"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a3"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a4"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a5"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a6"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a7"));

			//id : 0    age : 15  a1
			//id : 0    age : 15  a3
			//id : 0    age : 15  a6
			//id : 0    age : 15  a7
			//id : 0    age : 15  a5
			//id : 0    age : 15  a2
			//id : 0    age : 15  a4

			traverse(p_q);
		}

		static void test2()
		{
			stdcxx::priority_queue<Student_sptr, std::vector<Student_sptr>, CompareOfStrudentPtr_ASC> p_q;
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a1"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a2"));
			p_q.push(stdcxx::make_shared<Student>(0, 16, ::GetTickCount64(), "yeshuang1"));
			p_q.push(stdcxx::make_shared<Student>(0, 14, ::GetTickCount64(), "wang"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a3"));
			p_q.push(stdcxx::make_shared<Student>(0, 16, ::GetTickCount64(), "yeshuang2"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a4"));

			traverse(p_q);
		}
#else

		template<typename Elem, typename A0, typename A1, typename A2, typename A3>
		static void traverse(stdcxx::priority_queue<Elem, A0, A1, A2, A3> & p_q)
		{
			std::vector<Elem> vec;
			vec.reserve(p_q.size());

			while (p_q.empty() == false)
			{
				vec.push_back(p_q.top());
				p_q.pop();
			}

			for (auto &elem : vec)
				cout << elem << endl;

			cout << "**********************************************" << endl;
		}

		//优点:Student 优先级相同时，顺序以插入顺序为准，比 std::priority_queue功能强大
		//缺点 boost::priority_queue 不能指定容器, 使用的是std::vector
		static void test1()
		{
			stdcxx::priority_queue<Student, boost::heap::compare<CompareOfStrudent_ASC>, boost::heap::stable<true>> p_q;
			p_q.push(Student(0, 15, ::GetTickCount64(), "a1"));
			p_q.push(Student(0, 14, ::GetTickCount64(), "c1"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a2"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a3"));
			p_q.push(Student(0, 16, ::GetTickCount64(), "b1"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a4"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a5"));
			p_q.push(Student(0, 16, ::GetTickCount64(), "b2"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a6"));
			p_q.push(Student(0, 15, ::GetTickCount64(), "a7"));
			traverse<Student>(p_q);
		}

		static void test2()
		{
			stdcxx::priority_queue < Student_sptr, boost::heap::compare<CompareOfStrudentPtr_ASC>, boost::heap::stable<true> > p_q;
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a1"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a2"));
			p_q.push(stdcxx::make_shared<Student>(0, 16, ::GetTickCount64(), "yeshuang1"));
			p_q.push(stdcxx::make_shared<Student>(0, 14, ::GetTickCount64(), "wang"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a3"));
			p_q.push(stdcxx::make_shared<Student>(0, 16, ::GetTickCount64(), "yeshuang2"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a4"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a5"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a6"));
			p_q.push(stdcxx::make_shared<Student>(0, 15, ::GetTickCount64(), "a7"));

			traverse(p_q);
		}

#endif
	public:
		static void test_all()
		{
			test1();
			test2();
		}
	private:
	};
}