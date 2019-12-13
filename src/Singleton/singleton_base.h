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

//����ԣ� \thrift-0.12.0\lib\cpp\src\thrift\windows\TWinsockSingleton.h
//��˶��<<Linux���߳�������>>һ���н���������ģʽʹ�� double checking lock(DCL) ���ɿ�
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
	1.�����ͽ�� ȫ�ֶ��� ��̬��ʼ��˳��ȷ��������
	2.c++11 ��׼:�ֲ���̬����Ĺ������̰߳�ȫ��
	3.Ϊʲô��ֱ��ʹ�� static boost::shared_ptr<T> spInstance(new T());
		��ֹ T �Ĺ��캯���׳��쳣���ֲ���̬������ʱ�׳��쳣���ٴε��ô˺���ʱ���þֲ���̬�����������

	4.�˳�main��������ν�� ��̬�����ͷŵ�˳��
	  ����:���һ����̬�����Ѿ������� ����һ����̬���������� ��������
	  ��������� (1) �޸��Լ��������Ʒ���
				(2)���� instance() ���ص�����ָ��ĸ�������ȷ�� ʵ�ʵĶ��󲻻���Ϊ ��̬����ָ����ͷŶ��ͷ�
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


//ȱ�㣺��Ҫ�Լ������ڴ�
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