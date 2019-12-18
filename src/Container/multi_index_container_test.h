#pragma once

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


#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>



namespace multi_index_container_test
{
	using namespace stdcxx;
	class Student
	{
	public:
		Student() :id_(-1), age_(-1) {}
		Student(int id, int age, const std::string &name) :id_(id), age_(age), name_(name) {}

		int id()const { return id_; }
		int age()const { return age_; }
		const std::string& name()const { return name_; }
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

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student &student)
	{
		strm << "id:" << student.id() << "\t" << "age:" << student.age() << "\t" << student.name();
		return strm;
	}

	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, const Student_sptr spStudent)
	{
		strm << "id:" << spStudent->id() << "\t" << "age:" << spStudent->age() << "\t" << spStudent->name();
		return strm;
	}


	using namespace boost::multi_index;
	typedef sequenced<> seq_t;
	typedef hashed_unique<identity<Student>, StudentHash, StudentEqualCompare> hash_t;
	typedef hashed_unique<member<Student, int, &Student::id_>> hash_ex_t;

	typedef boost::multi_index_container<
		Student,
		indexed_by<hash_ex_t,seq_t>
		> StudentContainer;	

	class test_case
	{
	private:
		static void test1()
		{
			StudentContainer container;
			auto &hashedMapIndex = container.get<0>();
			hashedMapIndex.insert(Student(5, 25, "a5"));
			hashedMapIndex.insert(Student(3, 23, "a3"));
			hashedMapIndex.insert(Student(4, 24, "a4"));

			//container.insert(Student(5, 25, "a5"));
			//container.insert(Student(3, 23, "a3"));
			//container.insert(Student(4, 24, "a4"));

			auto &listIndex = container.get<1>();
			listIndex.push_back(Student(2, 22, "a2"));
			listIndex.push_back(Student(0, 20, "a0"));
			listIndex.push_back(Student(1, 21, "a1"));
			listIndex.push_back(Student(2, 22, "a2_re"));
			
			cout << "container.size():" << container.size() << endl;
			cout << "listIndex.size():" << listIndex.size() << endl;
			cout << "hashedMapIndex.size():" << hashedMapIndex.size() << endl;

			for (auto &elem : listIndex)
			{
				cout << elem << endl;
			}

			//auto & itr=container.find(2);
			//if (itr != container.end())
			//{
			//	cout <<"find(2):" <<*itr << endl;
			//}

		
		}

	public:
		static void test_all()
		{
			test1();
		}
	};



}
