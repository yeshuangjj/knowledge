#ifndef HORSE_PRIORITY_LINKED_HASH_MAP
#define HORSE_PRIORITY_LINKED_HASH_MAP

#ifdef _WIN32
#pragma once
#endif

#include <list>
#include <map>
#include <unordered_map>

template<class _Kty,
	class _Ty,
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
	typedef _Ty  mapped_type;

	//std::map
	typedef _Priority   priority_type;       //map:key
	typedef _PriorityPr priority_compare;  //map:key_compare

	//std::unordered_map
	typedef _Hasher hasher;
	typedef _Keyeq  hash_key_equal;

	//std::list
	typedef typename std::pair<const key_type, mapped_type> list_value_type; //list_的元素类型 （key_type是不允许改变的,必须使用const）
	typedef typename std::list<list_value_type> list_type;
	typedef typename list_type::size_type size_type;
	typedef typename list_type::reference list_reference;
	typedef typename list_type::const_reference const_list_reference;
	typedef typename list_type::iterator list_iterator;
	typedef typename list_type::const_iterator const_list_iterator;
	typedef typename list_type::reverse_iterator list_reverse_iterator;
	typedef typename list_type::const_reverse_iterator const_list_reverse_iterator;


private:
	
};

#endif //!HORSE_PRIORITY_LINKED_HASH_MAP
