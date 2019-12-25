#ifndef HORSE_PRIORITY_LINKED_HASH_MAP
#define HORSE_PRIORITY_LINKED_HASH_MAP

#ifdef _WIN32
#pragma once
#endif

#include <list>
#include <map>
#include <unordered_map>
#include <assert.h>
#include "linked_hash_map_helper.h"

/***
*例子：
*	class Student
*	{
*	public:
*       Student(int id) :id_(-1){}
*		Student(int id, const std::string &name) :id_(id), name_(name) {}
*		inline const int &id()const { return id_; }
*		const std::string &name()const { return  name_; }
*		inline const int &key()const { return id_; }
*	private:
*		int id_;
*		std::string name_;
*	};
*	typedef std::shared_ptr<Student> Student_sptr;
*
*	struct ObtainKeyOfStudent
*	{
*		inline const int& operator()(const Student&l)const
*		{
*			return l.id();
*		}
*	};
*
*	struct ObtainKeyOfStudentPtr
*	{
*		inline const int& operator()(const Student_sptr& sp)const
*		{
*			assert(sp);
*			return sp->id();
*		}
*	};
*
*	void example()
*	{
*		//如果Student 没有成员函数key(),必须自定义 一个"函数或仿函数"，来获取 key值
*		priority_linked_hash_map<int, Student, ObtainKeyOfStudent> linked1;
*		priority_linked_hash_map<int, Student_sptr, ObtainKeyOfStudentPtr> linked2;
*
*		//如果Student 有成员函数key(),使用默认值default_obtain_key_func_of_linked_hash_map<K,V>
*		priority_linked_hash_map<int, Student> linked3;
*		priority_linked_hash_map<int, Student_sptr, default_obtain_key_func_of_linked_hash_map<int, Student_sptr>> linked4; //使用偏特化的模板
*	}
****/

/*
if user call function find_elem, _Ty need have default construct func [如果用户调用函数 find_elem,类型 _Ty 必须有默认的构造函数]
*/

template<class _Kty,
	class _Ty,
	class _ObtainKeyFunc = default_obtain_key_func_of_linked_hash_map<_Kty, _Ty>,  // 函数或仿函数
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
private:
	//std::unordered_map
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
#ifdef _DEBUG
	void _CheckCount()const
	{
#define _USE_DEBUG_CHECK
#ifdef _USE_DEBUG_CHECK
		int count = 0;
		for (const_priority_map_iterator priorityMapItr = priority_map_.cbegin(); priorityMapItr != priority_map_.cend(); ++priorityMapItr)
		{
			const list_type &l = priorityMapItr->second;
			count += l.size();
		}
		assert(count == hash_map_.size());
#endif
	}
#else
	inline void _CheckCount()const {}
#endif

	/*
	* @brief:get listPtr by iterator
	*/
	list_type *_GetListPtr(list_iterator listItr)
	{
		if (listItr._Getcont() == nullptr)
			return nullptr;

		const list_type* pList = static_cast<const list_type*>(listItr._Getcont());
		return const_cast<list_type*>(pList);
	}

	/**
	* @brief:core of algorithm.
	* @param:
	* @return:
	**/
	list_iterator _Push(const list_value_type &val, const priority_type &priority, bool bCover, bool bPushback, bool &bExisted)
	{
		const key_type &k = obtain_key_(val);

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
					if (k == obtain_key_(*listItr))
					{
						bFind = true;
						assert(hashMapItr->second == listItr);
						break;
					}
				}

				if (bFind)
					break;
			}
			assert(bFind);
#endif

			bExisted = true;
			if (bCover)
			{
				list_iterator listItr = hashMapItr->second;
				*listItr = val; //调用list_value_type的赋值构造函数

				std::cout << "_Push [key:" << k << "] is existed, be covered!!!" << std::endl;
				return listItr;
			}
			else
			{
				std::cout << "_Push [key:" << k << "] is existed, do nothing!!!" << std::endl;
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
				std::pair<priority_map_iterator, bool> rtn = priority_map_.insert(priority_map_value_type(priority, list_type()));
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

			std::cout << "_Push [key:" << k << "] is not existed!!!" << std::endl;

#ifdef _DEBUG
			_CheckCount();
#endif
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

	/*
	*@brief: if list_value_type is shared_ptr,find_elem is good choice!
	*@return:if true,find ; if false,not find
	*/
	bool find_elem(const key_type &k, list_value_type &list_value)
	{
		hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
		{
			list_value = (*(hashMapItr->second)); //copy
			return true;
		}
		else
		{
			list_value = list_value_type(); //if call find_elem,must have default constuct func!
			return false;
		}
	}

	bool find_elem(const key_type &k, list_value_type &list_value)const
	{
		const_hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
		{
			list_value = (*(hashMapItr->second)); //copy
			return true;
		}
		else
		{
			list_value = list_value_type(); //if call find_elem,must have default constuct func!
			return false;
		}
	}

	//**************************************************************************************************
	inline void pop_front()
	{//删除保持一致性
		if (empty() == false)
		{
			for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); ++priorityMapItr)
			{
				list_type &l = priorityMapItr->second;
				if (l.empty() == false)
				{
					//1.删除 hash_map_中相应的元素 
					list_iterator listItr = l.begin();

					const key_type &k = obtain_key_(*listItr);
					hash_map_iterator hashMapItr = hash_map_.find(k);
					assert(hashMapItr != hash_map_.end() && hashMapItr->second == listItr);
					hash_map_.erase(hashMapItr);
					//2.删除list中的元素
					l.pop_front();

#ifdef _DEBUG
					_CheckCount();
#endif
					return;
				}
			}
		}
	}

	inline void pop_back()
	{//删除保持一致性
		assert(empty() == false);
		if (empty() == false)
		{
			//逆序遍历
			for (priority_map_reverse_iterator priorityMapItr = priority_map_.rbegin(); priorityMapItr != priority_map_.rend(); ++priorityMapItr)
			{
				list_type &l = priorityMapItr->second;
				if (l.empty() == false)
				{
					//1.删除 hash_map_中相应的元素 
					list_iterator listItr = (--l.end());
					const key_type &k = obtain_key_(*listItr);
					hash_map_iterator hashMapItr = hash_map_.find(k);
					assert(hashMapItr != hash_map_.end() && hashMapItr->second == listItr);
					hash_map_.erase(hashMapItr);

					//2.删除list中的元素
					l.pop_back();

#ifdef _DEBUG
					_CheckCount();
#endif
					return;
				}
			}
		}
	}

	//**************************************************************************************************
	/**
		if(empty()==false)
		{
			front();
			back();
			pop_front();
			pop_back();
		}
	*/
	inline list_reference front()
	{
		assert(empty() == false);
		list_value_type *pListValue = nullptr;
		for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); ++priorityMapItr)
		{
			list_type &l = priorityMapItr->second;
			if (l.empty() == false)
			{
				pListValue = &(l.front());
				break;
			}				
		}

		assert(pListValue);
		return *pListValue;
	}

	inline const_list_reference front()const
	{
		assert(empty() == false);
		const list_value_type *pListValue = nullptr;
		for (const_priority_map_iterator priorityMapItr = priority_map_.cbegin(); priorityMapItr != priority_map_.cend(); ++priorityMapItr)
		{
			const list_type &l = priorityMapItr->second;
			if (l.empty() == false)
			{
				pListValue = &(l.front());
				break;
			}
		}
		assert(pListValue);
		return *pListValue;
	}

	inline list_reference back()
	{
		assert(empty() == false);
		list_value_type *pListValue = nullptr;
		for (priority_map_reverse_iterator priorityMapItr = priority_map_.rbegin(); priorityMapItr != priority_map_.rend(); ++priorityMapItr)
		{
			list_type &l = priorityMapItr->second;
			if (l.empty() == false)
			{
				pListValue = &(l.back());
				break;
			}
		}

		assert(pListValue);
		return *pListValue;
	}

	inline const_list_reference back()const
	{
		assert(empty() == false);
		const list_value_type *pListValue = nullptr;
		for (const_priority_map_reverse_iterator priorityMapItr = priority_map_.crbegin(); priorityMapItr != priority_map_.crend(); ++priorityMapItr)
		{
			const list_type &l = priorityMapItr->second;
			if (l.empty() == false)
			{
				pListValue = &(l.back());
				break;
			}
		}

		assert(pListValue);
		return *pListValue;
	}

	//**************************************************************************************************
	bool remove(const key_type &k)
	{//清理保持一致性
		hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
		{
			list_iterator listItr = hashMapItr->second;
			//通过listItr 获取 list
			list_type * pList = _GetListPtr(listItr);
			assert(pList && listItr != pList->end());

			//1.
			hash_map_.erase(hashMapItr);
			//2.
			pList->erase(listItr);

#ifdef _DEBUG
			_CheckCount();
#endif
			return true;
		}

		return false;
	}

	inline bool erase(const key_type &k)
	{
		return remove(k);
	}

	//**************************************************************************************************
	void clear()
	{//清理保持一致性
		hash_map_.clear();
		priority_map_.clear();
	}

	//this function cost performance [耗费性能]
	void clear(const priority_type &priority)
	{//清理保持一致性		
		priority_map_iterator priorityMapItr = priority_map_.find(priority);
		if (priorityMapItr != priority_map_.end())
		{
			//1.清理hash_map_
			list_type &l = priorityMapItr->second;
			for (list_iterator listItr = l.begin(); listItr != l.end(); ++listItr)
			{
				const key_type &k = obtain_key_(*listItr);
				hash_map_iterator hashMapItr = hash_map_.find(k);
				assert(hashMapItr != hash_map_.end() && hashMapItr->second == listItr); //if assert false,need check logic
				hash_map_.erase(hashMapItr);
			}
			//2.清理priority_map_
			priority_map_.erase(priorityMapItr);
		}

#ifdef _DEBUG
		_CheckCount();
#endif
	}

public:
	inline size_type size()const
	{
#ifdef _DEBUG
		_CheckCount();
#endif
		return hash_map_.size();
	}

	bool empty()const
	{
#ifdef _DEBUG
		_CheckCount();
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
	obtain_key_func   obtain_key_;
	priority_map_type priority_map_;  //std::map< priority_type, list_type, priority_compare>
	hash_map_type     hash_map_;
};

#endif //!HORSE_PRIORITY_LINKED_HASH_MAP
