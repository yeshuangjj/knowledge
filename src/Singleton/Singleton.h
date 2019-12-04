#pragma once
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>

//std
//#include <memory>
//#include <mutex>

//����ԣ� \thrift-0.12.0\lib\cpp\src\thrift\windows\TWinsockSingleton.h
//��˶��<<Linux���߳�������>>һ���н���������ģʽʹ�� double checking lock(DCL) ���ɿ�

class Singleton :public boost::noncopyable
{
public:
	typedef Singleton this_type;
	typedef boost::shared_ptr<Singleton> instance_ptr;
private:
	Singleton(void);
public:
	~Singleton(void);
public:
	static instance_ptr instance(void);
private:
	static void init(void);
private:
	static instance_ptr     instance_ptr_;
	static boost::once_flag flags_;
};