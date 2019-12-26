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
*���ӣ�
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
*		//���Student û�г�Ա����key(),�����Զ��� һ��"������º���"������ȡ keyֵ
*		priority_linked_hash_map<int, Student, ObtainKeyOfStudent> linked1;
*		priority_linked_hash_map<int, Student_sptr, ObtainKeyOfStudentPtr> linked2;
*
*		//���Student �г�Ա����key(),ʹ��Ĭ��ֵdefault_obtain_key_func_of_linked_hash_map<K,V>
*		priority_linked_hash_map<int, Student> linked3;
*		priority_linked_hash_map<int, Student_sptr, default_obtain_key_func_of_linked_hash_map<int, Student_sptr>> linked4; //ʹ��ƫ�ػ���ģ��
*	}
****/

/*
*if user call function find_elem, _Ty need have default construct func [����û����ú��� find_elem,���� _Ty ������Ĭ�ϵĹ��캯��]
*/

/*
* @brief:StdIteratorType is standard iterator [StdIteratorType �Ǳ�׼�����ĵ�����]
*/

template<class _Kty,
	class _Ty,
	class _ObtainKeyFunc = default_obtain_key_func_of_linked_hash_map<_Kty, _Ty>,  // ������º���
	bool _IsAllowCover = false,                                                    // when key is existed,do nothing if _IsAllowCover is false; covered if _IsAllowCover is true
	class _Priority = std::size_t,
	class _PriorityPr = std::less< _Priority >,                                    // priority comparator predicate type
	class _Hasher = std::hash<_Kty>,
	class _Keyeq = std::equal_to<_Kty>
>
class priority_linked_hash_map
{
public:
	typedef _Kty key_type;
	typedef _Ty  list_value_type;
	typedef list_value_type* list_value_pointer;

	//std::map
	typedef _Priority   priority_type;       //map:key
	typedef _PriorityPr priority_compare;    //map:key_compare

	//std::unordered_map
	typedef _Hasher hasher;
	typedef _Keyeq  hash_key_equal;
	typedef _ObtainKeyFunc obtain_key_func;

	//std::list
	typedef typename std::list<list_value_type> list_type;
	typedef list_type* list_pointer;
	typedef typename list_type::size_type size_type;
	typedef typename list_type::reference list_value_reference;
	typedef typename list_type::const_reference const_list_value_reference;
	typedef typename list_type::iterator list_iterator;
	typedef typename list_type::const_iterator const_list_iterator;
	typedef typename list_type::reverse_iterator list_reverse_iterator;
	typedef typename list_type::const_reverse_iterator const_list_reverse_iterator;

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

	template<typename StdContainerType, typename StdIteratorType>
	static StdContainerType * _GetStdContainer(StdIteratorType itr)
	{
		if (itr._Getcont() == nullptr)
			return nullptr;

		const StdContainerType* p = static_cast<const StdContainerType*>(itr._Getcont());
		return const_cast<StdContainerType*>(p);
	}
public:
	//������
	class Iterator;
	friend class Iterator;
	/*
	*@brief:��α�ʾ end ,��� �� --end()��Ч
	*       ����õ�������ʧЧ
	*
	*
	*@think:[˼��] 1.����Ϊ��ʱ��begin end
	*              2.�����ǿ�ʱ��begin end
	*/
	class Iterator
	{
		//typedef priority_linked_hash_map<key_type, list_value_type, _IsAllowCover, priority_type, priority_compare, hasher, hash_key_equal> container_type;
		typedef priority_linked_hash_map container_type;
		friend class priority_linked_hash_map;
	private:
		const container_type  *pContainer_;
		priority_map_iterator  priorityMapItr_;
		list_iterator          listItr_;
		bool 				   endFlag_;
	public:
		Iterator() :pContainer_(nullptr), endFlag_(true) {}

		Iterator(const Iterator &right)
		{
			_Init(right);
		}

		Iterator&operator=(const Iterator &right)
		{
			_Init(right);
			return *this;
		}

	private:
		void _Init(const Iterator &right)
		{
			this->pContainer_ = right.pContainer_;
			this->priorityMapItr_ = right.priorityMapItr_;
			this->listItr_ = right.listItr_;
			this->endFlag_ = right.endFlag_;
		}
	public:
		list_value_reference operator*() const
		{	// return designated value
			assert(pContainer_->empty() == false);
			assert(pContainer_ != nullptr);
			assert(priorityMapItr_._Getcont() != nullptr);
			assert(listItr_._Getcont() != nullptr);
			assert(!endFlag_); //if assert false, iterator is out of range,please check your logic
			return (*listItr_);
		}

		list_value_pointer operator->() const
		{	// return pointer to class object
			assert(pContainer_->empty() == false);
			assert(pContainer_ != nullptr);
			assert(priorityMapItr_._Getcont() != nullptr);
			assert(listItr_._Getcont() != nullptr);
			assert(!endFlag_); //if assert false, iterator is out of range,please check your logic
			return &(*listItr_);
		}

		Iterator& operator++() //++itr
		{	// preincrement
			_Increment();
			return (*this);
		}

		Iterator operator++(int) //itr++
		{	// postincrement
			Iterator tempItr = *this;
			_Increment();
			return tempItr;
		}

		Iterator& operator--()
		{	// predecrement
			_Decrement();
			return (*this);
		}

		Iterator operator--(int)
		{	// postdecrement
			Iterator tempItr = *this;
			_Decrement();
			return tempItr;
		}

		bool operator==(const Iterator &right)const
		{
			assert(this->pContainer_ == right.pContainer_);

			//��� priority_map_Ϊ��  end iterator �Ƚ�����[��Ҫ��ϸ����]
			if (this->pContainer_->priority_map_.empty())
			{	//��ʱlistItr_����Ч��
				assert(this->listItr_._Getcont() == nullptr && right.listItr_._Getcont() == nullptr);
				assert(this->priorityMapItr_ == pContainer_->priority_map_.end() && this->priorityMapItr_ == right.priorityMapItr_);
				assert(this->endFlag_ && right.endFlag_);

				return true;
			}
			else
			{
				return this->priorityMapItr_ == right.priorityMapItr_
					&& this->listItr_ == right.listItr_
					&& this->endFlag_ == right.endFlag_;
			}
		}

		bool operator!=(const Iterator&other)const
		{
			return !(this->operator==(other));
		}
	private:
		/*
		*@brief: ��α�ʾend : priorityMapItr_ == priority_map_.end() listItr_Ϊ���һ�������end,
		*@note�� ��������ǿյ��أ�begin()�������end()
		*
		*/
		void _Increment()
		{
			assert(pContainer_ && pContainer_->empty() == false && endFlag_ == false);

			list_pointer pList = _GetListPtr();
			assert(pList && pList->empty() == false);

			if (++listItr_ != pList->end())
			{	// �����������滹��Ԫ��
				(void)0; //do nothing
			}
			else
			{	// ������������û��Ԫ��
				bool bFind = false;
				++priorityMapItr_;
				for (; priorityMapItr_ != pContainer_->priority_map_.end(); ++priorityMapItr_)
				{	//һֱ�ҵ��ǿյ�����
					pList = &(priorityMapItr_->second);
					if (pList->empty())
					{
						listItr_ = pList->end();
						continue;
					}
					else
					{
						bFind = true;
						listItr_ = pList->begin();
						break;
					}
				}
				assert(pList);
				endFlag_ = !bFind;
				if (endFlag_)
				{	//˵������end					
					assert(listItr_ == pList->end());
					assert(priorityMapItr_ == pContainer_->priority_map_.end());
				}
			}
		}

		/*
		*@brief:���� --end()
				�б�� --begin(),������������� assert(false)
		*/
		void _Decrement()
		{
			assert(pContainer_ && pContainer_->empty() == false);//����ȷ������Ԫ�ص������

			list_pointer pList = _GetListPtr();
			assert(pList);
			if (endFlag_)
			{
				if (pList->empty())
				{
					--listItr_;
					endFlag_ = false;
				}
				else
				{
					bool bFind = false;
					while (true)
					{
						if (priorityMapItr_ == pContainer_->priority_map_.begin())  // ˵���˵������Ѿ��� begin��������ִ�� --
							break;

						--priorityMapItr_; //������һ��������
						pList = &(priorityMapItr_->second);
						if (pList->empty() == false)
						{
							bFind = true;
							listItr_ = (--pList->end()); //list���һ��Ԫ�صĵ�����
							endFlag_ = false;
							break;
						}
					}

					assert(bFind);  // if bFind is false,indicate origin iterator is begin 
				}
			}
			else
			{
				assert(pList->empty() == false && listItr_ != pList->end());
				if (listItr_ == pList->begin())
				{	// ����õ������Ǹ�list��beginԪ��
					// Ѱ����һ���ǿյ�list

					bool bFind = false;
					while (true)
					{
						if (priorityMapItr_ == pContainer_->priority_map_.begin())  // ˵���˵������Ѿ��� begin��������ִ�� --
							break;

						--priorityMapItr_; //������һ��������
						pList = &(priorityMapItr_->second);
						if (pList->empty() == false)
						{
							bFind = true;
							listItr_ = (--pList->end()); //list���һ��Ԫ�صĵ�����
							break;
						}
					}

					assert(bFind);  // if bFind is false,indicate origin iterator is begin 
				}
				else
				{	// ����õ��������Ǹ�list��beginԪ��
					//ֱ�� --
					--listItr_;
				}
			}
		}

		static Iterator _Begin(container_type * const pContainer)
		{
			assert(pContainer);
			Iterator itr;
			itr.pContainer_ = pContainer;
			if (pContainer->empty())
			{
				itr.priorityMapItr_ = pContainer->priority_map_.end();
				itr.endFlag_ = true;
				assert(itr.listItr_._Getcont() == nullptr);
			}
			else
			{
				bool bFind = false;
				for (priority_map_iterator priorityMapItr = pContainer->priority_map_.begin(); priorityMapItr != pContainer->priority_map_.end(); ++priorityMapItr)
				{
					list_type &l = priorityMapItr->second;
					if (l.empty() == false)
					{
						bFind = true;
						itr.priorityMapItr_ = priorityMapItr;
						itr.listItr_ = l.begin();
						itr.endFlag_ = false;
						break;
					}
				}
				assert(bFind);
			}

			return itr;
		}

		static Iterator _End(container_type * const pContainer)
		{
			assert(pContainer);
			Iterator itr;
			itr.pContainer_ = pContainer;
			itr.priorityMapItr_ = pContainer->priority_map_.end();
			itr.endFlag_ = true;
			if (pContainer->empty() == false)
			{
				for (priority_map_reverse_iterator priorityMapItr = pContainer->priority_map_.rbegin(); priorityMapItr != pContainer->priority_map_.rend(); ++priorityMapItr)
				{
					itr.listItr_ = priorityMapItr->second.end();
					break;
				}
			}

			return itr;
		}

	private:
		void _Check()
		{
			assert(pContainer_ != nullptr);
			assert(priorityMapItr_._Getcont() != nullptr);
			assert(listItr_._Getcont() != nullptr);
		}

		inline list_pointer _GetListPtr()
		{
			return container_type::_GetStdContainer<list_type, list_iterator>(listItr_);
		}
	};

	////Consistency with the standard library ���׼�Ᵽ��һ����
	typedef typename Iterator iterator;
	typedef typename list_value_reference reference;
	typedef typename const_list_value_reference const_reference;

	//û��ʵ��
	//typedef typename const_list_iterator const_iterator;
	//typedef typename list_reverse_iterator reverse_iterator;
	//typedef typename const_list_reverse_iterator const_reverse_iterator;
public:
	priority_linked_hash_map() = default;
	~priority_linked_hash_map() = default;

	priority_linked_hash_map(const priority_linked_hash_map&) = delete;
	priority_linked_hash_map& operator=(const priority_linked_hash_map&) = delete;

	static bool is_allow_cover() { return _IsAllowConver; }
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
	list_pointer _GetListPtr(list_iterator listItr)
	{
		return _GetStdContainer<list_type, list_iterator>(listItr);
	}

	list_pointer _GetListPtr(const_list_iterator listItr)
	{
		return _GetStdContainer<list_type, const_list_iterator>(listItr);
	}

	/**
	* @brief:core of algorithm.
	* @param:
	* @return:
	**/
	list_iterator _Push(const list_value_type &val, const priority_type &priority, bool bCover, bool bPushback, bool &bExisted)
	{
		const key_type &k = obtain_key_(val);

		//�ж�k�Ƿ����
		hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
		{
			bExisted = true;
			if (bCover)
			{
				//����һ�� ԭλ�ø���
				//list_iterator listItr = hashMapItr->second;
				//*listItr = val; //����list_value_type�ĸ�ֵ���캯��

				//��������ɾ��ԭ���ģ�Ȼ����push [���ָ����� "������ӵ�˳��"������]
				list_iterator oldListItr = hashMapItr->second;
				list_pointer pList = _GetListPtr(oldListItr);
				assert(pList && oldListItr != pList->end());

				//1.ɾ����Ԫ��
				pList->erase(oldListItr);
				//2.������Ԫ��
				list_iterator newListItr;
				if (bPushback)
					newListItr = pList->insert(pList->end(), val);
				else
					newListItr = pList->insert(pList->begin(), val);

				hashMapItr->second = newListItr;            //�����µĵ�����

				std::cout << "_Push [key:" << k << "] is existed, be covered!!!" << std::endl;
				return newListItr;
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

			//����priority
			priority_map_iterator priorityMapItr = priority_map_.find(priority);
			list_pointer pList = nullptr;
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
		_Push(val, priority, _IsAllowCover, true, bExisted);
	}

	inline void push_back(const priority_type &priority, const list_value_type &val)
	{
		bool bExisted = false;
		_Push(val, priority, _IsAllowCover, true, bExisted);
	}

	//**************************************************************************************************
	inline void push_front(const priority_type &priority, const list_value_type &val, bool &bExisted)
	{
		_Push(val, priority, _IsAllowCover, false, bExisted);
	}

	inline void push_front(const priority_type &priority, const list_value_type &val)
	{
		bool bExisted = false;
		_Push(val, priority, _IsAllowCover, false, bExisted);
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
	{//ɾ������һ����
		if (empty() == false)
		{
			for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); ++priorityMapItr)
			{
				list_type &l = priorityMapItr->second;
				if (l.empty() == false)
				{
					//1.ɾ�� hash_map_����Ӧ��Ԫ�� 
					list_iterator listItr = l.begin();

					const key_type &k = obtain_key_(*listItr);
					hash_map_iterator hashMapItr = hash_map_.find(k);
					assert(hashMapItr != hash_map_.end() && hashMapItr->second == listItr);
					hash_map_.erase(hashMapItr);
					//2.ɾ��list�е�Ԫ��
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
	{//ɾ������һ����
		assert(empty() == false);
		if (empty() == false)
		{
			//�������
			for (priority_map_reverse_iterator priorityMapItr = priority_map_.rbegin(); priorityMapItr != priority_map_.rend(); ++priorityMapItr)
			{
				list_type &l = priorityMapItr->second;
				if (l.empty() == false)
				{
					//1.ɾ�� hash_map_����Ӧ��Ԫ�� 
					list_iterator listItr = (--l.end());
					const key_type &k = obtain_key_(*listItr);
					hash_map_iterator hashMapItr = hash_map_.find(k);
					assert(hashMapItr != hash_map_.end() && hashMapItr->second == listItr);
					hash_map_.erase(hashMapItr);

					//2.ɾ��list�е�Ԫ��
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
	inline list_value_reference front()
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

	inline const_list_value_reference front()const
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

	inline list_value_reference back()
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

	inline const_list_value_reference back()const
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
	size_type remove(const key_type &k)
	{//������һ����
		hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
		{
			list_iterator listItr = hashMapItr->second;
			//ͨ��listItr ��ȡ list
			list_pointer pList = _GetListPtr(listItr);
			assert(pList && listItr != pList->end());

			//1.
			hash_map_.erase(hashMapItr);
			//2.
			pList->erase(listItr);

#ifdef _DEBUG
			_CheckCount();
#endif
			return 1;
		}

		return 0;
	}

	inline size_type erase(const key_type &k)
	{
		return remove(k);
	}

	iterator erase(iterator itr)
	{//������һ����
		assert(itr != end());

		iterator rtnItr = itr; //copy
		++rtnItr;

		list_pointer pList = _GetListPtr(itr.listItr_);
		assert(pList && itr.listItr_ != pList->end());
		//1.
		const key_type &k = obtain_key_(*(itr.listItr_));
		hash_map_iterator hashMapItr = hash_map_.find(k);
		assert(hashMapItr != hash_map_.end());
		hash_map_.erase(hashMapItr);

		//2.
		pList->erase(itr.listItr_);//��ʹ��itr.listItr_ʧЧ����� ִ�� ++itr���������,���Կ���һ�ݣ�ִ��++

		return rtnItr;
	}

	//**************************************************************************************************
	void clear()
	{//������һ����
		hash_map_.clear();
		priority_map_.clear();
	}

	//this function cost performance [�ķ�����]
	void clear(const priority_type &priority)
	{//������һ����		
		priority_map_iterator priorityMapItr = priority_map_.find(priority);
		if (priorityMapItr != priority_map_.end())
		{
			//1.����hash_map_
			list_type &l = priorityMapItr->second;
			for (list_iterator listItr = l.begin(); listItr != l.end(); ++listItr)
			{
				const key_type &k = obtain_key_(*listItr);
				hash_map_iterator hashMapItr = hash_map_.find(k);
				assert(hashMapItr != hash_map_.end() && hashMapItr->second == listItr); //if assert false,need check logic
				hash_map_.erase(hashMapItr);
			}
			//2.����priority_map_
			priority_map_.erase(priorityMapItr);
		}

#ifdef _DEBUG
		_CheckCount();
#endif
	}

	iterator begin()
	{
		return iterator::_Begin(this);
	}

	iterator end()
	{
		return iterator::_End(this);
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

	/*
	*@brief:clear up the empty list
	*/
	void shrink_to_fit()
	{
		for (priority_map_iterator priorityMapItr = priority_map_.begin(); priorityMapItr != priority_map_.end(); )
		{
			list_type &l = priorityMapItr->second;
			if (l.empty())//remove
				priorityMapItr = priority_map_.erase(priorityMapItr);
			else
				++priorityMapItr
		}

#ifdef _DEBUG
		_CheckCount();
#endif
	}

	//�������
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

	//�������
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
