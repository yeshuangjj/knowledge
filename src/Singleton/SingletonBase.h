#pragma once
#define USE_BOOST
#ifdef USE_BOOST
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>
#else
#include <memory>
#include <mutex>
#endif

//借鉴自： \thrift-0.12.0\lib\cpp\src\thrift\windows\TWinsockSingleton.h
//陈硕的<<Linux多线程网络编程>>一书中讲到：单例模式使用 double checking lock(DCL) 不可靠
/***
eg:
	class Test :public SingletonBase<Test>
	{
		friend class SingletonBase<Test>;
	private:
		Test() :age_(88) {}
	public:
		int age()const { return age_; }
	private:
		int age_;
	};

	static void testSingleton()
	{
		printf("[Test1] age:%d \n", Test::instance()->age());
	}
****/

#ifdef USE_BOOST
template<typename T>
class SingletonBase :public boost::noncopyable
{
//	friend T;
//private:
//	SingletonBase(void) {};
//public:
//	~SingletonBase(void) {}

public:
	SingletonBase(void) {};
	~SingletonBase(void) {}
public:
	static boost::shared_ptr<T> instance(void)
	{
		boost::call_once(init, flags_);
		BOOST_ASSERT(instance_ptr_);
		return instance_ptr_;
	}
private:
	static void init(void)
	{
		instance_ptr_.reset(new T());
	}
private:
	static boost::shared_ptr<T> instance_ptr_;
	static boost::once_flag flags_;
};

template<class T>
boost::shared_ptr<T> SingletonBase<T>::instance_ptr_(nullptr);
template<class T>
boost::once_flag SingletonBase<T>::flags_ = BOOST_ONCE_INIT;

#else

template<typename T>
class SingletonBase
{
	//	friend T;
	//private:
	//	SingletonBase(void) {};
	//public:
	//	~SingletonBase(void) {}
private:
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
public:
	SingletonBase(void) {};
	~SingletonBase(void) {}
public:
	static std::shared_ptr<T> instance(void)
	{
		std::call_once(flags_,init);
		assert(instance_ptr_);
		return instance_ptr_;
	}
private:
	static void init(void)
	{
		instance_ptr_.reset(new T());
	}
private:
	static std::shared_ptr<T> instance_ptr_;
	static std::once_flag flags_;
};

template<class T>
std::shared_ptr<T> SingletonBase<T>::instance_ptr_(nullptr);
template<class T>
std::once_flag SingletonBase<T>::flags_;

#endif



/*
namespace stdcxx
{
#ifdef USE_BOOST
	using ::boost::shared_ptr;
	using ::boost::once_flag;
#else
	using ::std::shared_ptr;
	using ::std::once_flag;
#endif
}

template<typename T>
#ifdef USE_BOOST
class SingletonBase :public boost::noncopyable
{
#else
class SingletonBase
{
private:
	SingletonBase(const SingletonBase&) = delete;
	SingletonBase& operator=(const SingletonBase&) = delete;
#endif
	//	friend T;
	//private:
	//	SingletonBase(void) {};
	//public:
	//	~SingletonBase(void) {}
public:
	SingletonBase(void) {};
	~SingletonBase(void) {}
public:
	static stdcxx::shared_ptr<T> instance(void)
	{
#ifdef USE_BOOST
		boost::call_once(init, flags_);
		BOOST_ASSERT(instance_ptr_);
#else
		std::call_once(flags_, init);
#endif
		return instance_ptr_;
	}
private:
	static void init(void)
	{
		instance_ptr_.reset(new T());
	}
private:
	static stdcxx::shared_ptr<T> instance_ptr_;
	static stdcxx::once_flag flags_;
};

template<class T>
stdcxx::shared_ptr<T> SingletonBase<T>::instance_ptr_(nullptr);

#ifdef USE_BOOST
template<class T>
stdcxx::once_flag SingletonBase<T>::flags_ = BOOST_ONCE_INIT;
#else
template<class T>
stdcxx::once_flag SingletonBase<T>::flags_;
#endif

*/






