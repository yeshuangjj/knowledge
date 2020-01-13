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


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename StdContainerType, typename StdIteratorType>
StdContainerType * _GetStdContainerByIterator(const StdIteratorType itr)
{
	if (itr._Getcont() == nullptr)
		return nullptr;

	const StdContainerType* p = static_cast<const StdContainerType*>(itr._Getcont());
	return const_cast<StdContainerType*>(p);
}

template<typename StdContainerType, typename StdReverseIteratorType>
StdContainerType * _GetStdContainerByReverseIterator(const StdReverseIteratorType r_itr)
{
	if (r_itr.base()._Getcont() == nullptr)
		return nullptr;

	const StdContainerType* p = static_cast<const StdContainerType*>(r_itr.base()._Getcont());
	return const_cast<StdContainerType*>(p);
}

template<typename StdContainerType>
class IteratorTool
{
	typedef typename StdContainerType* StdContainerPointer;
	typedef typename StdContainerType::iterator iterator;
	typedef typename StdContainerType::const_iterator const_iterator;
public:
	inline static iterator _GetBeginIterator(StdContainerType &container) { return container.begin(); }
	inline static iterator _GetEndIterator(StdContainerType &container) { return container.end(); }
	inline static const_iterator _GetConstBeginIterator(const StdContainerType &container) { return container.cbegin(); }
	inline static const_iterator _GetConstEndIterator(const StdContainerType &container) { return container.cend(); }
	inline static StdContainerPointer _GetContainerPointer(const_iterator itr) { return _GetStdContainerByIterator<StdContainerType, const_iterator>(itr); }
};

template<typename StdContainerType>
class ReverseIteratorTool
{
	typedef typename StdContainerType* StdContainerPointer;
	typedef typename StdContainerType::reverse_iterator reverse_iterator;
	typedef typename StdContainerType::const_reverse_iterator const_reverse_iterator;
public:
	inline static reverse_iterator _GetBeginIterator(StdContainerType &container) { return container.rbegin(); }
	inline static reverse_iterator _GetEndIterator(StdContainerType &container) { return container.rend(); }
	inline static const_reverse_iterator _GetConstBeginIterator(const StdContainerType &container) { return container.crbegin(); }
	inline static const_reverse_iterator _GetConstEndIterator(const StdContainerType &container) { return container.crend(); }
	inline static StdContainerPointer _GetContainerPointer(const_reverse_iterator itr) { return _GetStdContainerByReverseIterator<StdContainerType, const_reverse_iterator>(itr); }
};

template<typename ToolType>
class ToolWrapper
{
public:
	typedef ToolType type;
};

#endif//!HORSE_LINKED_HASH_MAP_HELPER
