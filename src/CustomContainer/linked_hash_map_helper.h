#ifndef HORSE_LINKED_HASH_MAP_HELPER
#define HORSE_LINKED_HASH_MAP_HELPER

#ifdef _WIN32
#pragma once
#endif

#define SHARED_PTR_ENABLE_BOOST
#ifdef SHARED_PTR_ENABLE_BOOST
#include <boost/shared_ptr.hpp>
#endif

#include <memory>
#include <assert.h>

/***
* @brief:要求 类型 _Ty 成员函数 key()
***/
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_linked_hash_map
{
	inline const _Kty &operator()(const _Ty& l)
	{
		return l.key();
	}
};

//模板特例化:std智能指针的偏特化
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_linked_hash_map< _Kty, std::shared_ptr<_Ty> >
{
	inline const _Kty &operator()(const std::shared_ptr<_Ty>& sp)
	{
		assert(sp);
		return sp->key();
	}
};

#ifdef SHARED_PTR_ENABLE_BOOST
//模板特例化:boost智能指针的偏特化
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_linked_hash_map< _Kty, boost::shared_ptr<_Ty> >
{
	inline const _Kty &operator()(const boost::shared_ptr<_Ty>& sp)
	{
		BOOST_ASSERT(sp);
		return sp->key();
	}
};
#endif

#endif//!HORSE_LINKED_HASH_MAP_HELPER
