#ifndef HORSE_SINGLETON_BASE_HEADER
#define HORSE_SINGLETON_BASE_HEADER
#pragma once

#define HORSE_SINGLETON_BASE_USE_BOOST
#ifndef HORSE_SINGLETON_BASE_USE_BOOST
#include <memory>
#include <mutex>
#include <assert.h>
#else
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>
#endif

//借鉴自： \thrift-0.12.0\lib\cpp\src\thrift\windows\TWinsockSingleton.h
//陈硕的<<Linux多线程网络编程>>一书中讲到：单例模式使用 double checking lock(DCL) 不可靠
/***
eg:
	class Test :public singleton_base_thread_safe<Test>
	{
		friend class singleton_base_thread_safe<Test>;
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


/**
	1.这样就解决 全局对象 动态初始化顺序不确定的问题
	2.c++11 标准:局部静态对象的构造是线程安全的
	3.为什么不直接使用 static boost::shared_ptr<T> spInstance(new T());
		防止 T 的构造函数抛出异常。局部静态对象构造时抛出异常，再次调用此函数时，该局部静态对象继续构造

	4.退出main函数后，如何解决 静态对象释放的顺序？
	  描述:如果一个静态对象已经析构， 而另一个静态对象依旧在 调用它。
	  解决方案： (1) 修改自己程序的设计方案
				(2)保存 instance() 返回的智能指针的副本，以确保 实际的对象不会因为 静态智能指针的释放而释放
*/

#ifndef HORSE_SINGLETON_BASE_USE_BOOST
template<typename T>
class singleton_base_thread_safe
{
private:
	singleton_base_thread_safe(const singleton_base_thread_safe&) = delete;
	singleton_base_thread_safe& operator=(const singleton_base_thread_safe&) = delete;
public:
	singleton_base_thread_safe() = default;
	~singleton_base_thread_safe() = default;
public:
	static std::shared_ptr<T> instance()
	{
		static std::once_flag flags;
		static std::shared_ptr<T> spInstance(nullptr);
		//std::call_once(flags,std::bind(init, std::ref(spInstance)));
		std::call_once(flags, [&]() {init(spInstance); });

		assert(spInstance);
		return spInstance;
	}
private:
	static void init(std::shared_ptr<T> &spInstance)
	{
		assert(spInstance == nullptr);
		if (spInstance == nullptr)
			spInstance.reset(new T());
	}
};

template<typename T>
class singleton_base_thread_unsafe
{
public:
	singleton_base_thread_unsafe() = default;
	~singleton_base_thread_unsafe() = default;
public:
	static std::shared_ptr<T> instance()
	{
		static std::shared_ptr<T> spInstance(new T());
		return spInstance;
	}
};

#else

template<typename T>
class singleton_base_thread_safe : public boost::noncopyable
{
public:
	singleton_base_thread_safe() = default;
	~singleton_base_thread_safe() = default;
public:
	static boost::shared_ptr<T> instance()
	{
		static boost::once_flag flags = BOOST_ONCE_INIT;
		static boost::shared_ptr<T> spInstance(nullptr);
		//boost::call_once(boost::bind(init, boost::ref(spInstance)), flags);
		boost::call_once([&]() {init(spInstance); }, flags);

		BOOST_ASSERT(spInstance);
		return spInstance;
	}
private:
	static void init(boost::shared_ptr<T> &spInstance)
	{
		BOOST_ASSERT(spInstance == nullptr);
		if (spInstance == nullptr)
			spInstance.reset(new T());
	}
};

template<typename T>
class singleton_base_thread_unsafe : public boost::noncopyable
{
public:
	singleton_base_thread_unsafe() = default;
	~singleton_base_thread_unsafe() = default;
public:
	static boost::shared_ptr<T> instance()
	{
		static boost::shared_ptr<T> spInstance(new T());
		return spInstance;
	}
};
#endif


/**
note:when you use c++11 , lazy_singleton_base is thread-safe!
*/
template<typename T>
class lazy_singleton_base
{
public:
	static T *instance()
	{
		static T obj;
		return &obj;
	}

	//static void destory()
	//{
	//	instance()->~T();
	//}
};


//缺点：需要自己管理内存
//template<typename T>
//class singleton_base_thread_safe : public boost::noncopyable
//{
//public:
//	static T* instance()
//	{
//		static boost::once_flag flags_;
//		boost::call_once(init, flags_);
//		BOOST_ASSERT(pInstance_);
//		return pInstance_;
//	}
//private:
//	static void init()
//	{
//		pInstance_ = new T();
//	}
//
//private:
//	static T * pInstance_;	
//};
//
//template<typename T>
//T * singleton_base_threadsafe<T>::pInstance_ = nullptr;

#endif // !HORSE_SINGLETON_BASE_HEADER