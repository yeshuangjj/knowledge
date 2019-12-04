#include "stdafx.h"
#include "Singleton.h"

Singleton::instance_ptr Singleton::instance_ptr_(nullptr);

boost::once_flag Singleton::flags_ = BOOST_ONCE_INIT;
//std::once_flag Singleton::flags_;

Singleton::Singleton(void)
{
}

Singleton::~Singleton(void)
{
}

Singleton::instance_ptr Singleton::instance(void)
{
	boost::call_once(init, flags_);
	//std::call_once(flags_,init);

	return instance_ptr_;
}

void Singleton::init(void)
{
	instance_ptr_.reset(new this_type());
}