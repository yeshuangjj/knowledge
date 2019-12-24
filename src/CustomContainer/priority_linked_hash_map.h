#ifndef HORSE_PRIORITY_LINKED_HASH_MAP
#define HORSE_PRIORITY_LINKED_HASH_MAP

#ifdef _WIN32
#pragma once
#endif

#include <list>
#include <map>
#include <unordered_map>
#include <memory>
#include <assert.h>

//#define SHARED_PTR_ENABLE_BOOST
#ifdef SHARED_PTR_ENABLE_BOOST
#include <boost/shared_ptr.hpp>
#endif

/***
* @brief:要求 类型 _Ty 成员函数 key()
***/
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_priority_linked_hash_map
{
	inline const _Kty &operator()(const _Ty& l)
	{
		return l.key();
	}
};

//模板特例化:std智能指针的偏特化
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_priority_linked_hash_map< _Kty, std::shared_ptr<_Ty> >
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
struct default_obtain_key_func_of_priority_linked_hash_map< _Kty, boost::shared_ptr<_Ty> >
{
	inline const _Kty &operator()(const boost::shared_ptr<_Ty>& sp)
	{
		BOOST_ASSERT(sp);
		return sp->key();
	}
};
#endif

template<class _Kty,
	class _Ty,
	class _ObtainKeyFunc = default_obtain_key_func_of_priority_linked_hash_map<_Kty, _Ty>,  // 函数或仿函数
	bool _IsConvered = false,                   // when key is existed,do nothing if _IsConvered is false; convered if _IsConvered is true
	class _Priority = std::size_t,
	class _PriorityPr = std::less< _Priority >, // priority comparator predicate type
	class _Hasher = std::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>
>
class priority_linked_hash_map
{
public:
	typedef _Kty key_type;
	typedef _Ty  list_value_type;

	//std::map
	typedef _Priority   priority_type;       //map:key
	typedef _PriorityPr priority_compare;    //map:key_compare

	//std::unordered_map
	typedef _Hasher hasher;
	typedef _Keyeq  hash_key_equal;
	typedef _ObtainKeyFunc obtain_key_func;

	//std::list
	typedef typename std::list<list_value_type> list_type;
	typedef typename list_type::size_type size_type;
	typedef typename list_type::reference list_reference;
	typedef typename list_type::const_reference const_list_reference;
	typedef typename list_type::iterator list_iterator;
	typedef typename list_type::const_iterator const_list_iterator;
	typedef typename list_type::reverse_iterator list_reverse_iterator;
	typedef typename list_type::const_reverse_iterator const_list_reverse_iterator;

	////Consistency with the standard library 与标准库保持一致性
	//typedef typename list_reference reference;
	//typedef typename const_list_reference const_reference;
	//typedef typename list_iterator iterator;
	//typedef typename const_list_iterator const_iterator;
	//typedef typename list_reverse_iterator reverse_iterator;
	//typedef typename const_list_reverse_iterator const_reverse_iterator;

	//std::unordered_map
	//typedef typename std::pair<const key_type, list_iterator > hash_map_value_type;
	//typedef typename std::allocator<hash_map_value_type> hash_map_allocator_type;
	//typedef typename std::unordered_map<key_type, list_iterator, hasher, hash_key_equal, hash_map_allocator_type> hash_map_type;
	typedef typename std::unordered_map<key_type, list_iterator, hasher, hash_key_equal> hash_map_type;
	typedef typename hash_map_type::value_type hash_map_value_type;
	typedef typename hash_map_type::iterator hash_map_iterator;
	typedef typename hash_map_type::const_iterator const_hash_map_iterator;

	//std::map
	typedef typename std::map< priority_type, list_type, priority_compare> priority_map_type;
	typedef typename priority_map_type::value_type priority_map_value_type;
	typedef typename priority_map_type::iterator priority_map_iterator;
	typedef typename priority_map_type::const_iterator const_priority_map_iterator;
	typedef typename priority_map_type::reverse_iterator priority_map_reverse_iterator;
	typedef typename priority_map_type::const_reverse_iterator const_priority_map_reverse_iterator;
public:
	priority_linked_hash_map() = default;
	~priority_linked_hash_map() = default;

	priority_linked_hash_map(const priority_linked_hash_map&) = delete;
	priority_linked_hash_map& operator=(const priority_linked_hash_map&) = delete;
private:
	/**
	* @brief:core of algorithm.
	* @param:
	* @return:
	**/
	list_iterator _Push(const list_value_type &val, const priority_type &priority, bool bCover, bool bPushback, bool &bExisted)
	{
		//这里是关键所在
		static obtain_key_func obtain_key; //避免重复构造
		const key_type &k = obtain_key(val);

		//判断k是否存在
		hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
		{
#ifdef _DEBUG
			//检查
			bool bFind = false;
			for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); ++priorityMapItr)
			{
				const list_type &l = priorityMapItr->second;
				for (const_list_iterator listItr = l.cbegin(); listItr != l.cend(); ++listItr)
				{
					if (k == obtain_key(*listItr))
					{
						bFind = true;
						assert(hashMapItr->second == listItr);
					}

				}
			}
			assert(bFind);
#endif

			bExisted = true;
			if (bCover)
			{
				list_iterator listItr = hashMapItr->second;
				*listItr = val; //调用list_value_type的赋值构造函数

				std::cout << __FUNCTION__ << "[key:" << k << "] is existed, be covered!!!" << std::endl;

				return listItr;
			}
			else
			{
				std::cout << __FUNCTION__ << "[key:" << k << "] is existed, do nothing!!!" << std::endl;
				return hashMapItr->second;
			}
		}
		else
		{
			bExisted = false;

			//查找priority
			priority_map_iterator priorityMapItr = priority_map_.find(priority);
			list_type *pList = nullptr;
			if (priorityMapItr != priority_map_.end())
			{
				pList = &(priorityMapItr->second);
			}
			else
			{
				auto rtn = priority_map_.insert(priority_map_value_type(priority, list_type()));
				assert(rtn.second);
				pList = &(rtn.first->second);
			}
			assert(pList);
			list_iterator listItr = pList->end();
			if (bPushback)
			{
				pList->push_back(val);
				listItr = (--pList->end());
			}
			else
			{
				pList->push_front(val);
				listItr = pList->begin();

			}
			hash_map_.insert(hash_map_value_type(k, listItr));
			assert(listItr != pList->end());

			std::cout << __FUNCTION__ << "[key:" << k << "] is not existed!!!" << std::endl;
			return listItr;
		}
	}

public:
	//**************************************************************************************************
	inline void push_back(const priority_type &priority, const list_value_type &val, bool &bExisted)
	{
		_Push(val, priority, _IsConvered, true, bExisted);
	}

	inline void push_back(const priority_type &priority, const list_value_type &val)
	{
		bool bExisted = false;
		_Push(val, priority, _IsConvered, true, bExisted);
	}

	//**************************************************************************************************
	inline void push_front(const priority_type &priority, const list_value_type &val, bool &bExisted)
	{
		_Push(val, priority, _IsConvered, false, bExisted);
	}

	inline void push_front(const priority_type &priority, const list_value_type &val)
	{
		bool bExisted = false;
		_Push(val, priority, _IsConvered, false, bExisted);
	}

	//**************************************************************************************************
	list_value_type *find(const key_type &k)
	{
		hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
			return &(*(hashMapItr->second));

		return nullptr;
	}

	const list_value_type *find(const key_type &k)const
	{
		const_hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
			return &(*(hashMapItr->second));

		return nullptr;
	}

private:
	void _Pop(bool bPopFront)
	{
		if (empty() == false)
		{
			for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); ++priorityMapItr)
			{
				list_type &l = priorityMapItr->second;
				if (bPopFront)
					l.pop_front();
				else
					l.pop_back();
			}
		}
	}
public:
	inline void pop_front()
	{
		_Pop(true);
	}

	inline void pop_back()
	{
		_Pop(false);
	}

	//**************************************************************************************************
	inline list_reference front()
	{
		assert(empty() == false);
		for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); ++priorityMapItr)
		{
			list_type &l = priorityMapItr->second;
			if (l.empty() == false)
				return l.front();
		}
	}

	inline const_list_reference front()const
	{
		assert(empty() == false);
		for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); ++priorityMapItr)
		{
			list_type &l = priorityMapItr->second;
			if (l.empty() == false)
				return l.front();
		}
	}

	inline list_reference back()
	{
		assert(empty() == false);
		for (priority_map_reverse_iterator priorityMapItr = priority_map_.rbegin(); priorityMapItr != priority_map_.rend(); ++priorityMapItr)
		{
			list_type &l = priorityMapItr->second;
			if (l.empty() == false)
				return l.back();
		}
	}

	inline const_list_reference back()const
	{
		assert(empty() == false);
		for (priority_map_reverse_iterator priorityMapItr = priority_map_.rbegin(); priorityMapItr != priority_map_.rend(); ++priorityMapItr)
		{
			list_type &l = priorityMapItr->second;
			if (l.empty() == false)
				return l.back();
		}
	}
	//**************************************************************************************************
	void clear()
	{
		hash_map_.clear();
		priority_map_.clear();
	}

	inline size_type size()const
	{
#ifdef _DEBUG
		int count = 0;
		for (const_priority_map_iterator priorityMapItr = priority_map_.cbegin(); priorityMapItr != priority_map_.cend(); ++priorityMapItr)
		{
			const list_type &l = priorityMapItr->second;
			for (const_list_iterator listItr = l.cbegin(); listItr != l.cend(); ++listItr)
				++count;
		}
		assert(count == hash_map_.size());
#endif
		return hash_map_.size();
	}

	bool empty()const
	{
#ifdef _DEBUG
		int count = 0;
		for (const_priority_map_iterator priorityMapItr = priority_map_.cbegin(); priorityMapItr != priority_map_.cend(); ++priorityMapItr)
		{
			const list_type &l = priorityMapItr->second;
			for (const_list_iterator listItr = l.cbegin(); listItr != l.cend(); ++listItr)
				++count;
		}
		assert(count == hash_map_.size());
#endif
		return hash_map_.empty();
	}

	void swap(priority_linked_hash_map &other)
	{
		if (this == &other)
			return;

		this->priority_map_.swap(other.priority_map_);
		this->hash_map_.swap(other.hash_map_);
	}

	//正序遍历
	template<typename Func>
	void traverse(Func f)const
	{
		for (const_priority_map_iterator priorityMapItr = priority_map_.cbegin(); priorityMapItr != priority_map_.cend(); ++priorityMapItr)
		{
			const list_type &l = priorityMapItr->second;
			for (const_list_iterator listItr = l.cbegin(); listItr != l.cend(); ++listItr)
				f(*listItr);
		}
	}

	//倒序遍历
	template<typename Func>
	void traverse_reverse(Func f)const
	{
		for (const_priority_map_reverse_iterator priorityMapItr = priority_map_.crbegin(); priorityMapItr != priority_map_.crend(); ++priorityMapItr)
		{
			const list_type &l = priorityMapItr->second;
			for (const_list_reverse_iterator listItr = l.crbegin(); listItr != l.crend(); ++listItr)
				f(*listItr);
		}
	}

private:
	priority_map_type priority_map_;  //std::map< priority_type, list_type, priority_compare>
	hash_map_type     hash_map_;
};

#endif //!HORSE_PRIORITY_LINKED_HASH_MAP
