#ifndef HORSE_LINKED_HASH_MAP
#define HORSE_LINKED_HASH_MAP
#pragma once

#include <unordered_map>
#include <list>
#include <mutex>
#include <assert.h>
//#include <vector>
//#include <boost/unordered_map.hpp>



//class dummy_mutex
//{
//public:
//	dummy_mutex() = default;
//	~dummy_mutex() = default;
//	dummy_mutex(const dummy_mutex&) = delete;
//	dummy_mutex& operator=(const dummy_mutex&) = delete;
//	inline void lock() {}
//	inline void unlock() {}
//};
//template<class _Kty,
//	class _Ty,
//	class _Hasher = std::hash<_Kty>,
//	class _Keyeq = std::equal_to<_Kty>,
//	class _Mutex = std::mutex
//>
//class linked_hash_map
//{
//public:
//	//thread-safe
//	typedef _Mutex mutex_type;
//	typedef std::lock_guard<mutex_type> lock_type;
//}

////不能将迭代器暴露出去。返回一个副本，才能保证线程安全
//bool find(const key_type &k,mapped_type & v)
//{
//	lock_type lock(mutex_);
//	map_iterator mapItr = map_.find(k);
//	if (mapItr != map_.end())
//	{
//		return true;
//	}
//	return false;
//}

/*
* 支持快速查找的功能
*思路:
*     typedef typename std::pair<const key_type, mapped_type>   list_value_type;
*     typedef typename std::pair<const key_type, list_iterator> map_value_type; //map_的元素类型 (list的迭代器不会失效)
*     list_iterator迭代器指向的元素类型为 list_value_type
*     list_value_type.first  <==> key_type
*     map_value_type.first   <==> key_type
*     map_value_type.second  <==> list_iterator
*
* 有待验证
*
*
*
*/


template<class _Kty,
	class _Ty,
	class _Hasher = std::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>,
	bool _IsConvered = false
>
class linked_hash_map
{
public:
	typedef _Kty key_type;
	typedef _Ty mapped_type;
	typedef _Hasher hasher;
	typedef _Keyeq key_equal;

	typedef typename std::pair<const key_type, mapped_type> list_value_type; //list_的元素类型
	typedef typename std::list<list_value_type> list_type;
	typedef typename list_type::reference list_reference;
	typedef typename list_type::const_reference const_list_reference;
	typedef typename list_type::iterator list_iterator;
	typedef typename list_type::const_iterator const_list_iterator;
	typedef typename list_type::reverse_iterator list_reverse_iterator;
	typedef typename list_type::const_reverse_iterator const_list_reverse_iterator;

	typedef typename list_type::size_type size_type;

private:
	typedef typename std::pair<const key_type, list_iterator> map_value_type; //map_的元素类型 (list的迭代器不会失效)
	typedef typename std::allocator<map_value_type> map_allocator_type;
	typedef typename std::unordered_map<key_type, list_iterator, hasher, key_equal, map_allocator_type> hash_map_type;  //使用迭代器，就不用拷贝副本
	typedef typename hash_map_type::iterator map_iterator;
	typedef typename hash_map_type::const_iterator const_map_iterator;

	//typedef typename hash_map_type::value_type value_type;	
	//typedef typename list_type::reference reference;
	//typedef typename list_type::const_reference const_reference;
	//typedef typename std::unordered_map<key_type, mapped_type, key_equal, allocator_type> hash_map_type;

	//typedef typename hash_map_type::size_type size_type;
	//typedef typename hash_map_type::difference_type difference_type;
	//typedef typename hash_map_type::pointer pointer;
	//typedef typename hash_map_type::const_pointer const_pointer;
	//typedef typename hash_map_type::reference reference;
	//typedef typename hash_map_type::const_reference const_reference;
	//typedef typename hash_map_type::iterator iterator;
	//typedef typename hash_map_type::const_iterator const_iterator;
private:
	/*core of algorithm*/
	list_iterator _Insert(const_list_iterator _Where, const list_value_type &val, bool bCover, bool &bExisted)
	{
		map_iterator mapItr = map_.find(val.first);
		list_iterator listItr = list_.end();
		if (mapItr != map_.end())
		{//this key is existed
			assert(mapItr->second != list_.end());
			bExisted = true;
			if (bCover)
			{
				list_.erase(mapItr->second);
				listItr = list_.insert(_Where, val);
				mapItr->second = listItr;
			}
			else
			{
				listItr = mapItr->second;
			}
		}
		else
		{
			bExisted = false;
			listItr = list_.insert(_Where, val);
			map_.insert(map_value_type(val.first, listItr));
		}
		return listItr;
	}
public:
	inline list_iterator insert(const_list_iterator _Where, const list_value_type &val,bool &bExisted)
	{
		return _Insert(_Where, val, _IsConvered, bExisted);
	}

	inline list_iterator insert(const_list_iterator _Where, const key_type &new_k, const mapped_type &v, bool &bExisted)
	{
		return _Insert(_Where, list_value_type(new_k, v), _IsConvered, bExisted);
	}

	/*if not found key,equal to push_back*/
	list_iterator insert(const key_type &k, const list_value_type &val, bool &bExisted)
	{
		map_iterator mapItr = map_.find(k);
		list_iterator listItr = list_.end();
		if (mapItr！ = map_.end())
			listItr = mapItr->second;

		return insert(listItr, val, bExisted);
	}

	list_iterator insert(const key_type &k, const key_type &new_k, const mapped_type &v, bool &bExisted)
	{
		map_iterator mapItr = map_.find(k);
		list_iterator listItr = list_.end();
		if (mapItr != map_.end())
			listItr = mapItr->second;

		return insert(listItr, list_value_type(new_k, v), bExisted);
	}

	inline list_iterator push_back(const list_value_type &val, bool &bExisted)
	{
		insert(list_.end(), val, bExisted);
	}

	inline list_iterator push_back(const key_type &new_k, const mapped_type &v, bool &bExisted)
	{
		insert(list_.end(), list_value_type(new_k, v), bExisted);
	}

	//**************************************************************************************************
	/*ignore param：bExisted*/
	list_iterator insert(const_list_iterator _Where, const list_value_type &val)
	{
		bool bExisted = false;
		return _Insert(_Where, val, _IsConvered, bExisted);
	}

	list_iterator insert(const key_type &k, const list_value_type &val)
	{
		bool bExisted = false;
		return insert(k, val, bExisted);
	}

	list_iterator insert(const key_type &k, const key_type &new_k, const mapped_type &v)
	{
		bool bExisted = false;
		return insert(k, new_k, v, bExisted);
	}

	inline list_iterator push_back(const list_value_type &val)
	{
		bool bExisted = false;
		insert(list_.end(), val, bExisted);
	}

	inline list_iterator push_back(const key_type &new_k, const mapped_type &v)
	{
		bool bExisted = false;
		insert(list_.end(), list_value_type(new_k, v), bExisted);
	}


	/*
	bool push_back(const key_type &k, const mapped_type& v, bool bCover = false)
	{
		map_iterator mapItr = map_.find(k);
		//this key is existed
		bool bExisted = false;
		if (mapItr != map_.end())
			bExisted = true;

		if (bExisted)
		{
			assert(mapItr->second != list_.end());
			if (bCover)
			{
				list_.erase(mapItr->second);
				list_.push_back(list_value_type(k, v));
				mapItr->second = (--list_.end());
			}
		}
		else
		{
			list_.push_back(list_value_type(k, v));
			map_.insert(map_value_type(k, --list_.end()));
		}

		return !bExisted;
	}

	bool push_back(const list_value_type &val, bool bCover = false)
	{
		map_iterator mapItr = map_.find(val.first);
		//this key is existed
		bool bExisted = false;
		if (mapItr != map_.end())
			bExisted = true;

		if (bExisted)
		{
			assert(mapItr->second != list_.end());
			if (bCover)
			{
				list_.erase(mapItr->second);
				list_.push_back(val);
				mapItr->second = (--list_.end());
			}
		}
		else
		{
			list_.push_back(val);
			map_.insert(map_value_type(val.first, --list_.end()));
		}

		return !bExisted;
	}

	bool push_front(const key_type &k, const mapped_type& v, bool bCover = false)
	{
		map_iterator mapItr = map_.find(k);
		//this key is existed
		bool bExisted = false;
		if (mapItr != map_.end())
			bExisted = true;

		if (bExisted)
		{
			assert(mapItr->second != list_.end());
			if (bCover)
			{
				list_.erase(mapItr->second);
				list_.push_back(list_value_type(k, v));
				mapItr->second = (list_.begin());
			}
		}
		else
		{
			list_.push_back(list_value_type(k, v));
			map_.insert(map_value_type(k, list_.begin()));
		}

		return !bExisted;
	}

	bool push_front(const list_value_type &val, bool bCover = false)
	{
		map_iterator mapItr = map_.find(val.first);
		//this key is existed
		bool bExisted = false;
		if (mapItr != map_.end())
			bExisted = true;

		if (bExisted)
		{
			assert(mapItr->second != list_.end());
			if (bCover)
			{
				list_.erase(mapItr->second);
				list_.push_back(val);
				mapItr->second = list_.begin();
			}
		}
		else
		{
			list_.push_front(val);
			map_.insert(map_value_type(val.first, list_.begin()));
		}

		return !bExisted;
	}
	*/

	/**
	* @brief
	*
	* @return true:this val is not existed	false:this val is existed
	* @param [in] bCover: be covered if the key is existed,
	**/
	//inline std::pair<list_iterator, bool> push_back(const list_value_type &val, bool bCover = false)
	//{
	//	return insert(list_.end(), val, bCover);
	//}

	//inline std::pair<list_iterator, bool> push_back(const key_type &k, const mapped_type& v, bool bCover = false)
	//{
	//	return push_back(list_value_type(k, v), bCover);
	//}

	//inline std::pair<list_iterator, bool> push_front(const list_value_type &val, bool bCover = false)
	//{
	//	return insert(list_.begin(), val, bCover);
	//}

	//inline std::pair<list_iterator, bool> push_front(const key_type &k, const mapped_type& v, bool bCover = false)
	//{
	//	return push_front(list_value_type(k, v), bCover);
	//}

	/**
	* @brief
	*
	* @return std::pair<list_iterator, bool> if second is false,indicate the key is existed
	* @param [in] bCover: be covered if the key is existed,
	**/
	//std::pair<list_iterator, bool> insert(const_list_iterator _Where, const list_value_type &val, bool bCover = false)
	//{
	//	map_iterator mapItr = map_.find(val.first);
	//	//this key is existed
	//	if (mapItr != map_.end())
	//	{
	//		assert(mapItr->second != list_.end());
	//		if (bCover)
	//		{
	//			list_.erase(mapItr->second);
	//			list_iterator listItr = list_.insert(_Where, val);
	//			mapItr->second = listItr;
	//			return std::pair<list_iterator, bool>(listItr, false);
	//		}
	//		else
	//		{
	//			return std::pair<list_iterator, bool>(mapItr->second, false);
	//		}
	//	}
	//	else
	//	{
	//		list_iterator listItr = list_.insert(_Where, val);
	//		map_.insert(map_value_type(val.first, listItr));

	//		return std::pair<list_iterator, bool>(listItr, true);
	//	}
	//}

	//std::pair<list_iterator, bool> insert(const key_type &k, const list_value_type &val, bool bCover = false)
	//{
	//	map_iterator mapItr = map_.find(k);
	//	list_iterator listItr = list_.end();
	//	if (mapItr！ = map_.end())
	//		listItr = mapItr->second;

	//	return insert(listItr, val, bCover);
	//}

	//std::pair<list_iterator, bool> insert(const key_type &k, const key_type &new_k, const mapped_type &v, bool bCover = false)
	//{
	//	map_iterator mapItr = map_.find(k);
	//	list_iterator listItr = list_.end();
	//	if (mapItr != map_.end())
	//		listItr = mapItr->second;

	//	return insert(listItr, list_value_type(new_k, v), bCover);
	//}






	/**
	* @brief 获取首元素的迭代器
	*
	* @param [in] null
	**/
	inline list_iterator begin()
	{
		return list_.begin();
	}

	inline const_list_iterator begin()const
	{
		return list_.begin();
	}

	inline list_iterator end()
	{
		return list_.end();
	}

	inline const_list_iterator end()const
	{
		return list_.end();
	}

	inline const_list_iterator cbegin()const
	{
		return list_.cbegin();
	}

	inline const_list_iterator cend()const
	{
		return list_.cend();
	}

	inline list_reverse_iterator rbegin()
	{
		return list_.rbegin();
	}

	inline const_list_reverse_iterator rbegin() const
	{
		return list_.rbegin();
	}

	inline list_reverse_iterator rend()
	{
		return list_.rend();
	}

	inline const_list_reverse_iterator rend() const
	{
		return list_.rend();
	}

	inline const_list_reverse_iterator crbegin()const
	{
		return list_.crbegin();
	}

	inline const_list_reverse_iterator crend()const
	{
		return list_.crend();
	}

	/**
	* @brief find
	*
	* @param [in] key
	**/
	list_iterator find(const key_type &k)
	{
		if (empty() == false)
		{
			map_iterator mapItr = map_.find(k);
			if (mapItr != map_.end())
			{
				assert(mapItr->second != list_.end());
				return mapItr->second;
			}
		}
		return list_.end();
	}

	const_list_iterator find(const key_type &k)const
	{
		if (empty() == false)
		{
			map_iterator mapItr = map_.find(k);
			if (mapItr != map_.end())
			{
				assert(mapItr->second != list_.end());
				return mapItr->second;
			}
		}
		return list_.end();
	}

	void pop_front()
	{
		if (list_.empty() == false)
		{
			list_iterator listItr = list_.begin();
			if (listItr != list_.end())
			{
				//注意erase的顺序
				//1. 先移除map中的元素
				map_.erase(listItr->first);
				list_.erase(listItr);
			}
		}
	}

	void pop_back()
	{
		if (list_.empty() == false)
		{
			list_iterator listItr = --list_.end();
			if (listItr != list_.end())
			{
				//注意erase的顺序
				//1. 先移除map中的元素
				map_.erase(listItr->first);
				list_.erase(listItr);
			}
		}
	}

	/**
	* @brief eg:
	*     if(list.empty()==false)
	*	  {
	*        list.front();
	*        list.back();
	*     }
	*
	* @param [in] key
	**/
	inline list_reference front()
	{
		return list_.front();
	}

	inline const_list_reference front()const
	{
		return list_.front();
	}

	inline list_reference back()
	{
		return list_.back();
	}

	inline const_list_reference back()const
	{
		return list_.back();
	}

	/**
	* @brief erase
	* @return: next iterator of eraseed elem
	* @param [in] key
	**/
	list_iterator erase(const key_type &k)
	{
		map_iterator mapItr = map_.find(k);
		if (mapItr != map_.end())
		{
			assert(mapItr->second != list_.end());
			//注意erase的顺序
			list_iterator nextItr = list_.erase(mapItr->second);
			map_.erase(mapItr);
			return nextItr;
		}
		return list_.end();
	}

	list_iterator erase(const_list_iterator listItr)
	{
		if (listItr == list_.end())
			return list_.end();
		assert(map_.find(listItr->first) != map_.end());
		map_.erase(listItr->first);
		return list_.erase(listItr);
	}

	inline bool empty()
	{
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
		map_.clear();
		list_.clear();
	}

	inline size_type size()
	{
		assert(list_.size() == map_.size());
		return list_.size();
	}

private:
	list_type list_;
	hash_map_type map_;
};

#endif //!HORSE_LINKED_HASH_MAP
