#pragma once
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/once.hpp>

//std
//#include <memory>
//#include <mutex>

//借鉴自： \thrift-0.12.0\lib\cpp\src\thrift\windows\TWinsockSingleton.h
//陈硕的<<Linux多线程网络编程>>一书中讲到：单例模式使用 double checking lock(DCL) 不可靠

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