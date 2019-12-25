#ifndef HORSE_LINKED_HASH_MAP
#define HORSE_LINKED_HASH_MAP

#ifdef _WIN32
#pragma once
#endif

#include <unordered_map>
#include <list>
#include <assert.h>
#include <boost/unordered_map.hpp>

/*
* ֧�ֿ��ٲ��ҵĹ���
* (1)˼·:
*     typedef typename std::pair<const key_type, mapped_type>   list_value_type;
*     typedef typename std::pair<const key_type, list_iterator> map_value_type; //map_��Ԫ������ (list�ĵ�����������Ϊ �����ɾ������Ԫ�ض����µ�����ʧЧ�����ʹ��vector,����Ϊ����������µ�����ʧЧ)
*     list_iterator������ָ���Ԫ������Ϊ list_value_type
*     list_value_type.first  <==> key_type
*     map_value_type.first   <==> key_type
*     map_value_type.second  <==> list_iterator
*
* (2)ע������������� ���뱣��һ���ԣ�ɾ������һ����
* (3)�д���֤:��Ҫ���� �� ����߼�
*	 �ر�Ҫע�⣺_IsConveredΪtrueʱ���߼�, ������ͬkey��Ԫ��ʱ������� old elem�ĵ�����ʧЧ��
*
*
*/

template<class _Kty,
	class _Ty,
	bool _IsConvered = false,            //when key is existed,do nothing if _IsConvered is false; convered if _IsConvered is true
	class _Hasher = std::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>
>
class linked_hash_map
{
public:
	typedef _Kty key_type;
	typedef _Ty mapped_type;
	typedef _Hasher hasher;
	typedef _Keyeq key_equal;

	typedef typename std::pair<const key_type, mapped_type> list_value_type; //list_��Ԫ������ ��key_type�ǲ�����ı��,����ʹ��const��
	typedef typename std::list<list_value_type> list_type;
	typedef typename list_type::size_type size_type;
	typedef typename list_type::reference list_reference;
	typedef typename list_type::const_reference const_list_reference;
	typedef typename list_type::iterator list_iterator;
	typedef typename list_type::const_iterator const_list_iterator;
	typedef typename list_type::reverse_iterator list_reverse_iterator;
	typedef typename list_type::const_reverse_iterator const_list_reverse_iterator;

	//Consistency with the standard library ���׼�Ᵽ��һ����
	typedef list_reference reference;
	typedef const_list_reference const_reference;
	typedef list_iterator iterator;
	typedef const_list_iterator const_iterator;
	typedef list_reverse_iterator reverse_iterator;
	typedef const_list_reverse_iterator const_reverse_iterator;
private:
	typedef typename std::pair<const key_type, list_iterator> map_value_type; //map_��Ԫ������ (list�ĵ���������ʧЧ)
	typedef typename std::allocator<map_value_type> map_allocator_type;
	typedef typename std::unordered_map<key_type, list_iterator, hasher, key_equal, map_allocator_type> hash_map_type;  //ʹ�õ��������Ͳ��ÿ�������
	typedef typename hash_map_type::iterator map_iterator;
	typedef typename hash_map_type::const_iterator const_map_iterator;
private:
	/**
	* @brief:core of algorithm.
	*        note,if bExisted is true,and _Where== mapItr->second, will cause "_Where" to be invalid
	* @param:
	* @return:
	**/
	list_iterator _Insert(const_list_iterator _Where, const list_value_type &val, bool bCover, bool &bExisted)
	{
		assert(_Where._Getcont() == &list_); //���������Ƿ�ʧЧ��,��� list��insert,�жϵ������Ƿ�ʧЧ

		map_iterator mapItr = map_.find(val.first);
		list_iterator listItr = list_.end();
		if (mapItr != map_.end())
		{//this key is existed
			assert(mapItr->second != list_.end());
			bExisted = true;
			if (bCover)
			{
				//����һ��
				//�����ȵ���list_.insert(_Where, val) ,Ȼ�������� list_.erase(mapItr->second)
				//ԭ��_Where �� mapItr->second��ͬһ��Ԫ�صĵ����� ������ͬһ�������������ͬһԪ�صĵ���������erase(mapItr->second)�ᵼ�� _WhereʧЧ���Ӷ�����insert����
				listItr = list_.insert(_Where, val); //������Ԫ��
				list_.erase(mapItr->second);         //ɾ����Ԫ��
				mapItr->second = listItr;            //�����µĵ�����

				//��������
				//if (_Where == mapItr->second)
				//{ //��� _Where �� mapItr->second��ͬһ��Ԫ�صĵ����� [�ر���Ҫע��]
				//	const mapped_type *p = &(_Where->second);
				//	const_cast<mapped_type*>(p)->operator=(val.second);//����mapped_type�ĸ�ֵ���캯��
				//	//const_cast<mapped_type*>(&(_Where->second))->operator=(val.second); //����mapped_type�ĸ�ֵ���캯��
				//}
				//else
				//{
				//	listItr = list_.insert(_Where, val);
				//	list_.erase(mapItr->second);					
				//	mapItr->second = listItr;
				//}

				std::cout << __FUNCTION__ << "[key:" << val.first << "] is existed, be covered!!!" << std::endl;
			}
			else
			{
				listItr = mapItr->second;
				std::cout << __FUNCTION__ << "[key:" << val.first << "] is existed, do nothing!!!" << std::endl;
			}
		}
		else
		{
			std::cout << __FUNCTION__ << "[key:" << val.first << "] is not existed!!!" << std::endl;
			bExisted = false;
			listItr = list_.insert(_Where, val);
			map_.insert(map_value_type(val.first, listItr));
		}
		return listItr;
	}
public:
	inline list_iterator insert(const_list_iterator _Where, const list_value_type &val, bool &bExisted)
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
		if (mapItr != map_.end())
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

	//**************************************************************************************************
	/*ignore param��bExisted*/
	list_iterator insert(const_list_iterator _Where, const list_value_type &val)
	{
		bool bExisted = false;
		return _Insert(_Where, val, _IsConvered, bExisted);
	}

	inline list_iterator insert(const_list_iterator _Where, const key_type &new_k, const mapped_type &v)
	{
		bool bExisted = false;
		return _Insert(_Where, list_value_type(new_k, v), _IsConvered, bExisted);
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

	//**************************************************************************************************
	inline list_iterator push_back(const list_value_type &val, bool &bExisted)
	{
		return insert(list_.end(), val, bExisted);
	}

	inline list_iterator push_back(const key_type &new_k, const mapped_type &v, bool &bExisted)
	{
		return insert(list_.end(), list_value_type(new_k, v), bExisted);
	}

	inline list_iterator push_back(const list_value_type &val)
	{
		bool bExisted = false;
		return insert(list_.end(), val, bExisted);
	}

	inline list_iterator push_back(const key_type &new_k, const mapped_type &v)
	{
		bool bExisted = false;
		return insert(list_.end(), list_value_type(new_k, v), bExisted);
	}

	//**************************************************************************************************
	inline list_iterator push_front(const list_value_type &val, bool &bExisted)
	{
		return insert(list_.begin(), val, bExisted);
	}

	inline list_iterator push_front(const key_type &new_k, const mapped_type &v, bool &bExisted)
	{
		return insert(list_.begin(), list_value_type(new_k, v), bExisted);
	}

	inline list_iterator push_front(const list_value_type &val)
	{
		bool bExisted = false;
		return insert(list_.begin(), val, bExisted);
	}

	inline list_iterator push_front(const key_type &new_k, const mapped_type &v)
	{
		bool bExisted = false;
		return insert(list_.begin(), list_value_type(new_k, v), bExisted);
	}

	//**************************************************************************************************

	void pop_front()
	{
		if (list_.empty() == false)
		{
			list_iterator listItr = list_.begin();
			if (listItr != list_.end())
			{
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
				map_.erase(listItr->first);
				list_.erase(listItr);
			}
		}
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
			//ע��erase��˳��
			list_iterator nextItr = list_.erase(mapItr->second);
			map_.erase(mapItr);
			return nextItr;
		}
		return list_.end();
	}

	list_iterator erase(const_list_iterator listItr)
	{
		assert(listItr._Getcont() == &list_); //���������Ƿ�ʧЧ��,��� list��insert,�жϵ������Ƿ�ʧЧ

		if (listItr == list_.end())
			return list_.end();
		assert(map_.find(listItr->first) != map_.end());
		map_.erase(listItr->first);
		return list_.erase(listItr);
	}

	/**
	* @brief ��ȡ��Ԫ�صĵ�����
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
