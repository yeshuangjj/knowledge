#pragma once

#ifndef HORSE_SIMPLE_LINKED_HASH_MAP
#define HORSE_SIMPLE_LINKED_HASH_MAP

#ifdef _WIN32
#pragma once
#endif

#include <unordered_map>
#include <list>
#include <assert.h>
#include <memory>

//#define SHARED_PTR_ENABLE_BOOST
#ifdef SHARED_PTR_ENABLE_BOOST
#include <boost/shared_ptr.hpp>
#endif

/***
*���ӣ�
*	class Student
*	{
*	public:
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
*		//���Student û�г�Ա����key(),�����Զ��� һ��"������º���"������ȡ keyֵ
*		simple_linked_hash_map<int, Student, ObtainKeyOfStudent> linked1;
*		simple_linked_hash_map<int, Student_sptr, ObtainKeyOfStudentPtr> linked2;
*
*		//���Student �г�Ա����key(),ʹ��Ĭ��ֵdefault_obtain_key_func_of_simple_linked_hash_map<K,V>
*		simple_linked_hash_map<int, Student> linked3;
*		simple_linked_hash_map<int, Student_sptr, default_obtain_key_func_of_simple_linked_hash_map<int, Student_sptr>> linked4; //ʹ��ƫ�ػ���ģ��
*	}
****/

/***
* @brief:Ҫ�� ���� _Ty ��Ա���� key()
***/
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_simple_linked_hash_map
{
	inline const _Kty &operator()(const _Ty& l)
	{
		return l.key();
	}
};

//ģ��������:std����ָ���ƫ�ػ�
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_simple_linked_hash_map< _Kty, std::shared_ptr<_Ty> >
{
	inline const _Kty &operator()(const std::shared_ptr<_Ty>& sp)
	{
		assert(sp);
		return sp->key();
	}
};

#ifdef SHARED_PTR_ENABLE_BOOST
//ģ��������:boost����ָ���ƫ�ػ�
template<class _Kty, class _Ty>
struct default_obtain_key_func_of_simple_linked_hash_map< _Kty, boost::shared_ptr<_Ty> >
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
	class _ObtainKeyFunc = default_obtain_key_func_of_simple_linked_hash_map<_Kty, _Ty>,  // ������º���
	bool _IsConvered = false,                                                             // when key is existed,do nothing if _IsConvered is false; convered if _IsConvered is true
	class _Hasher = std::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>
>
class simple_linked_hash_map
{
public:
	typedef _Kty key_type;
	typedef _Ty list_value_type;
	typedef _Hasher hasher;
	typedef _Keyeq key_equal;
	typedef _ObtainKeyFunc obtain_key_func;

	typedef typename std::list<list_value_type> list_type;

	typedef typename list_type::size_type size_type;
	typedef typename list_type::reference list_reference;
	typedef typename list_type::const_reference const_list_reference;
	typedef typename list_type::iterator list_iterator;
	typedef typename list_type::const_iterator const_list_iterator;
	typedef typename list_type::reverse_iterator list_reverse_iterator;
	typedef typename list_type::const_reverse_iterator const_list_reverse_iterator;

	//Consistency with the standard library ���׼�Ᵽ��һ����
	typedef typename list_reference reference;
	typedef typename const_list_reference const_reference;
	typedef typename list_iterator iterator;
	typedef typename const_list_iterator const_iterator;
	typedef typename list_reverse_iterator reverse_iterator;
	typedef typename const_list_reverse_iterator const_reverse_iterator;
private:
	//map_��Ԫ������ (list�ĵ���������ʧЧ)  ���ﲻ��ʹ�� std::pair<const key_type, const_list_iterator >,��Ϊfind()�����᷵��list_iterator
	//typedef typename std::pair<const key_type, list_iterator > map_value_type;
	//typedef typename std::allocator<map_value_type> map_allocator_type;
	//typedef typename std::unordered_map<const key_type, list_iterator, hasher, key_equal, map_allocator_type> hash_map_type;  //ʹ�õ��������Ͳ��ÿ�������
	typedef typename std::unordered_map<key_type, list_iterator, hasher, key_equal> hash_map_type;  //ʹ�õ��������Ͳ��ÿ�������
	typedef typename hash_map_type::value_type map_value_type; //ʵ���Ͼ��� std::pair<const key_type, list_iterator >
	typedef typename hash_map_type::iterator map_iterator;
	typedef typename hash_map_type::const_iterator const_map_iterator;
public:
	simple_linked_hash_map() = default;
	~simple_linked_hash_map() = default;

	simple_linked_hash_map(const simple_linked_hash_map&) = delete;
	simple_linked_hash_map& operator=(const simple_linked_hash_map&) = delete;
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

		//�����ǹؼ�����
		static obtain_key_func obtain_key; //�����ظ�����
		const key_type &k = obtain_key(val);

		map_iterator mapItr = map_.find(k);
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

				std::cout << __FUNCTION__ << "[key:" << k << "] is existed, be covered!!!" << std::endl;
			}
			else
			{
				listItr = mapItr->second;
				std::cout << __FUNCTION__ << "[key:" << k << "] is existed, do nothing!!!" << std::endl;
			}
		}
		else
		{
			std::cout << __FUNCTION__ << "[key:" << k << "] is not existed!!!" << std::endl;
			bExisted = false;
			listItr = list_.insert(_Where, val);
			map_.insert(map_value_type(k, listItr));
		}
		return listItr;
	}

public:
	inline list_iterator insert(const_list_iterator _Where, const list_value_type &val, bool &bExisted)
	{
		return _Insert(_Where, val, _IsConvered, bExisted);
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

	//**************************************************************************************************
	/*ignore param��bExisted*/
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

	//**************************************************************************************************
	inline list_iterator push_back(const list_value_type &val, bool &bExisted)
	{
		return insert(list_.end(), val, bExisted);
	}

	inline list_iterator push_back(const list_value_type &val)
	{
		bool bExisted = false;
		return insert(list_.end(), val, bExisted);
	}

	//**************************************************************************************************
	inline list_iterator push_front(const list_value_type &val, bool &bExisted)
	{
		return insert(list_.begin(), val, bExisted);
	}

	inline list_iterator push_front(const list_value_type &val)
	{
		bool bExisted = false;
		return insert(list_.begin(), val, bExisted);
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
		assert(empty() == false);
		return list_.front();
	}

	inline const_list_reference front()const
	{
		assert(empty() == false);
		return list_.front();
	}

	inline list_reference back()
	{
		assert(empty() == false);
		return list_.back();
	}

	inline const_list_reference back()const
	{
		assert(empty() == false);
		return list_.back();
	}

	inline bool empty()const
	{
		assert(list_.size() == map_.size());
		return list_.empty();
	}

	void clear()
	{
		map_.clear();
		list_.clear();
	}

	inline size_type size()const
	{
		assert(list_.size() == map_.size());
		return list_.size();
	}

	void swap(simple_linked_hash_map &other)
	{
		if (this == &other)
			return;

		this->list_.swap(other.list_);
		this->map_.swap(other.map_);
	}
private:
	list_type list_;
	hash_map_type map_;
};

#endif //!HORSE_SIMPLE_LINKED_HASH_MAP
