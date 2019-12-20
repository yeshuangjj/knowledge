#pragma once
#include <unordered_map>
#include <list>
#include <mutex>
#include <assert.h>

//支持快速查找的功能

class dummy_mutex
{
public:
	dummy_mutex() = default; 
	~dummy_mutex() = default;

	dummy_mutex(const dummy_mutex&) = delete;
	dummy_mutex& operator=(const dummy_mutex&) = delete;

	inline void lock() {}
	inline void unlock() {}
};

template<class _Kty,
	class _Ty,
	class _Hasher = std::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>,
	class _Mutex = std::mutex
	>
class linked_hash_map
{
public:	
	//thread-safe
	typedef _Mutex mutex_type;
	typedef std::lock_guard<mutex_type> lock_type;

	//list
	//typedef _Ty elem_type;

	//unordered_map
	typedef _Hasher hasher;
	typedef _Kty key_type;
	typedef _Ty mapped_type;
	typedef _Ty referent_type;	// extra
	typedef _Keyeq key_equal;

//private:
	typedef std::pair<const key_type, mapped_type> value_type;
	typedef std::allocator<value_type> allocator_type;

	typedef typename std::unordered_map<key_type, mapped_type, key_equal, allocator_type> hash_map_type;
	
	//typedef typename hash_map_type::value_type value_type;
	typedef typename std::list<value_type> list_type;
	typedef typename list_type::size_type size_type;
	typedef typename list_type::reference reference;
	typedef typename list_type::const_reference const_reference;
	typedef typename list_type::iterator iterator;
	typedef typename list_type::const_iterator const_iterator;

	//typedef typename hash_map_type::size_type size_type;
	//typedef typename hash_map_type::difference_type difference_type;
	//typedef typename hash_map_type::pointer pointer;
	//typedef typename hash_map_type::const_pointer const_pointer;
	//typedef typename hash_map_type::reference reference;
	//typedef typename hash_map_type::const_reference const_reference;
	//typedef typename hash_map_type::iterator iterator;
	//typedef typename hash_map_type::const_iterator const_iterator;

public:
	bool push_back(const value_type &val)
	{
		lock_type lock(mutex_);
		auto mapItr = map_.find(val.first);
		//this key is existed
		if (mapItr != map_.end())
			return false;

		list_.push_back(val);		
		map_.insert(val);
		return true;
	}

	bool push_front(const value_type &val)
	{
		lock_type lock(mutex_);
		auto mapItr = map_.find(val.first);
		//this key is existed
		if (mapItr != map_.end())
			return false;

		list_.push_front(val);
		map_.insert(val);
		return true;
	}

	//同时返回pop出的元素
	void pop_front()
	{
		lock_type lock(mutex_);
	}

	void pop_back()
	{	
	}



	void find(const key_type &k)
	{
		lock_type lock(mutex_);
		auto mapItr = map_.find(k);


	}



	//void push_back(const key_type &k,const mapped_type &v)
	//{
	//	lock_type lock(mutex_);
	//	list_.push_back(v);
	//	map_.insert()
	//}

	void empty()
	{
		lock_type lock(mutex_);
#ifdef _DEBUG
		if (list_.empty())
			assert(map_.empty());

		if (list_.empty() == false)
			assert(map_.empty() == false);
#endif
		return list_.empty();
	}

	void clear()
	{
		lock_type lock(mutex_);
		list_.clear();
		map_.clear();
	}

private:
	mutex_type mutex_;
	list_type list_;
	hash_map_type map_;
};


//template <class K, class T, class H, class P, class A> 
//class LinkedHashMap
//{
//public:
//    typedef K key_type;
//    typedef T mapped_type;
//    typedef std::pair<const K, T> value_type;
//    typedef H hasher;
//    typedef P key_equal;
//    typedef A allocator_type;
//}