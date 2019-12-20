#ifndef HORSE_PRIORITY_LINKED_HASH_MAP
#define HORSE_PRIORITY_LINKED_HASH_MAP

#ifdef _WIN32
#pragma once
#endif

#include <list>
#include <map>
#include <unordered_map>
#include <memory>

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

	//std::list
	typedef typename std::list<list_value_type> list_type;
	typedef typename list_type::size_type size_type;
	typedef typename list_type::reference list_reference;
	typedef typename list_type::const_reference const_list_reference;
	typedef typename list_type::iterator list_iterator;
	typedef typename list_type::const_iterator const_list_iterator;
	typedef typename list_type::reverse_iterator list_reverse_iterator;
	typedef typename list_type::const_reverse_iterator const_list_reverse_iterator;

	//std::unordered_map
	//typedef typename std::pair<const key_type, list_iterator > hash_map_value_type;
	//typedef typename std::allocator<hash_map_value_type> hash_map_allocator_type;
	//typedef std::unordered_map<const key_type, list_iterator, hasher, hash_key_equal, hash_map_allocator_type> hash_map_type;
	typedef std::unordered_map<const key_type, list_iterator, hasher, hash_key_equal> hash_map_type;

	//std::map
	typedef std::map< priority_type, list_type, priority_compare> priority_map_type;
	//typedef typename std::shared_ptr<list_type> list_sptr;
	//typedef std::map< priority_type, list_sptr, priority_compare> priority_map_;  //需要斟酌

private:
	priority_map_type priority_map_;
	hash_map_type     hash_map_;
};

#endif //!HORSE_PRIORITY_LINKED_HASH_MAP
