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
*if user call function find_elem, _Ty need have default construct func [如果用户调用函数 find_elem,类型 _Ty 必须有默认的构造函数]
*/

/*
* @brief:StdIteratorType is standard iterator [StdIteratorType 是标准容器的迭代器]
*/

template<class _Kty,
	class _Ty,
	class _ObtainKeyFunc = default_obtain_key_func_of_linked_hash_map<_Kty, _Ty>,  // 函数或仿函数
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
	typedef const list_value_type* const_list_value_pointer;

	//std::map
	typedef _Priority   priority_type;       //map:key
	typedef _PriorityPr priority_compare;    //map:key_compare

	//std::unordered_map
	typedef _Hasher hasher;
	typedef _Keyeq  hash_key_equal;
	typedef _ObtainKeyFunc obtain_key_func;

	//std::list
	typedef typename std::list<list_value_type> list_type;
	typedef typename list_type* list_pointer;
	typedef typename const list_type* const_list_pointer;
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
	typedef typename priority_map_type*  priority_map_pointer;
	typedef typename const priority_map_type* const_priority_map_pointer;
	typedef typename priority_map_type::value_type priority_map_value_type;
	typedef typename priority_map_type::iterator priority_map_iterator;
	typedef typename priority_map_type::const_iterator const_priority_map_iterator;
	typedef typename priority_map_type::reverse_iterator priority_map_reverse_iterator;
	typedef typename priority_map_type::const_reverse_iterator const_priority_map_reverse_iterator;

private:
	//迭代器
	/*
	*@brief:如何表示 end ,如何 让 --end()有效
	*       如何让迭代器不失效
	*
	*
	*@think:[思考] 1.容器为空时的begin end
	*              2.容器非空时的begin end
	*/
	typedef typename priority_linked_hash_map container_type;

#define __USE_PLAN_ONE
#ifdef __USE_PLAN_ONE
	//方案一：
	template<typename PriorityMapIteratorType,
		typename ListIteratorType,
		typename PriorityMapToolType,
		typename ListToolType>
		class IteratorImpl
	{
		typedef typename ToolWrapper<PriorityMapToolType>::type PriorityMapTool;
		typedef typename ToolWrapper<ListToolType>::type        ListTool;

		typedef typename PriorityMapIteratorType                PriorityMapIterator;
		typedef typename ListIteratorType                       ListIterator;
	public:
		const container_type         *pContainer_;
		mutable PriorityMapIterator   priorityMapItr_;
		mutable ListIterator          listItr_;
		bool 				          endFlag_;
	public:
		IteratorImpl() :pContainer_(nullptr), endFlag_(true) {}

		IteratorImpl(const IteratorImpl &right)
		{
			_Init(right);
		}

		IteratorImpl & operator=(const IteratorImpl &right)
		{
			_Init(right);
			return *this;
		}

	private:
		void _Init(const IteratorImpl &right)
		{
			this->pContainer_ = right.pContainer_;
			this->priorityMapItr_ = right.priorityMapItr_;
			this->listItr_ = right.listItr_;
			this->endFlag_ = right.endFlag_;
		}
	public:
		list_value_reference operator*() const
		{	// return designated value			
			assert(pContainer_ != nullptr);
			assert(pContainer_->empty() == false);
			assert(_GetListPtr() != nullptr && _GetListPtr()->empty() == false);
			assert(_GetMapPtr() != nullptr && _GetMapPtr()->empty() == false);
			assert(!endFlag_); //if assert false, iterator is out of range,please check your logic
			return (*listItr_);
		}

		list_value_pointer operator->() const
		{	// return pointer to class object
			assert(pContainer_ != nullptr);
			assert(pContainer_->empty() == false);
			assert(_GetListPtr() != nullptr && _GetListPtr()->empty() == false);
			assert(_GetMapPtr() != nullptr && _GetMapPtr()->empty() == false);
			assert(!endFlag_); //if assert false, iterator is out of range,please check your logic
			return &(*listItr_);
		}

		bool operator==(const IteratorImpl &right)const
		{
			assert(this->pContainer_ == right.pContainer_);

			//如果 priority_map_为空  end iterator 比较特殊[需要仔细斟酌]
			if (this->pContainer_->priority_map_.empty())
			{	//此时listItr_是无效的
				assert(this->_GetListPtr() == nullptr && right._GetListPtr() == nullptr);
				assert(this->priorityMapItr_ == _GetPriorityMapEndIterator(pContainer_->priority_map_) && this->priorityMapItr_ == right.priorityMapItr_);
				assert(this->endFlag_ && right.endFlag_);

				return true;
			}
			else
			{
				if (this->endFlag_)
				{
					assert(this->priorityMapItr_ == _GetPriorityMapEndIterator(pContainer_->priority_map_));
					return this->endFlag_ == right.endFlag_
						&& this->priorityMapItr_ == right.priorityMapItr_;
				}
				else
				{
					return this->endFlag_ == right.endFlag_
						&& this->priorityMapItr_ == right.priorityMapItr_
						&& this->listItr_ == right.listItr_;
				}
			}
		}

		inline bool operator!=(const IteratorImpl &right)const
		{
			return !(this->operator==(right));
		}

		void operator++() //++itr
		{	// preincrement
			_Increment();
		}

		void operator--()
		{	// predecrement
			_Decrement();
		}

	private:
		/*
		*@brief: 如何表示end : priorityMapItr_ == priority_map_.end() listItr_为最后ListIterator(),
		*@note： 如果容器是空的呢？begin()本身就是end()
		*
		*/
		void _Increment()
		{
			assert(pContainer_ && pContainer_->empty() == false && endFlag_ == false);

			list_pointer pList = _GetListPtr();
			assert(pList && pList->empty() == false);

			if (++listItr_ != _GetListEndIterator(*pList))
			{	// 如果该链表后面还有元素
				(void)0; //do nothing
			}
			else
			{	// 如果该链表后面没有元素
				bool bFind = false;
				++priorityMapItr_;

				for (; priorityMapItr_ != _GetPriorityMapEndIterator(pContainer_->priority_map_); ++priorityMapItr_)
				{	//一直找到非空的链表
					pList = &(priorityMapItr_->second);
					if (pList->empty())
					{
						listItr_ = _GetListEndIterator(*pList);
						continue;
					}
					else
					{
						bFind = true;
						listItr_ = _GetListBeginIterator(*pList);
						break;
					}
				}
				assert(pList);
				endFlag_ = !bFind;
				if (endFlag_)
				{	//说明到了end
					assert(listItr_ == _GetListEndIterator(*pList));
					assert(priorityMapItr_ == _GetPriorityMapEndIterator(pContainer_->priority_map_));
					listItr_ = ListIterator();
				}
			}
		}

		/*
		*@brief:考虑 --end()
		*		判别出 --begin(),将此种情况进行 assert(false)
		*/
		void _Decrement()
		{
			assert(pContainer_ && pContainer_->empty() == false);//必须确保是有元素的情况下

			list_pointer pList = _GetListPtr();
			if (endFlag_)
			{	// 说明当前再执行 --end()
				assert(_GetListPtr() == nullptr);
				assert(priorityMapItr_ == _GetPriorityMapEndIterator(pContainer_->priority_map_));
				bool bFind = false;
				while (true)
				{
					--priorityMapItr_;
					pList = &(priorityMapItr_->second);
					assert(pList);
					if (pList->empty() == false)
					{
						bFind = true;
						listItr_ = (--_GetListEndIterator(*pList)); //list最后一个元素的迭代器
						endFlag_ = false;
						break;
					}

					if (priorityMapItr_ == _GetPriorityMapBeginIterator(pContainer_->priority_map_))  // 说明此迭代器已经是 begin，不能再执行 --
						break;
				}
				assert(bFind);  // if bFind is false,indicate the container is empty,please check my code			
			}
			else
			{
				assert(pList && pList->empty() == false && listItr_ != _GetListEndIterator(*pList));
				if (listItr_ == _GetListBeginIterator(*pList))
				{	// 如果该迭代器是该list的begin元素
					// 寻找上一个非空的list

					bool bFind = false;
					while (true)
					{
						if (priorityMapItr_ == _GetPriorityMapBeginIterator(pContainer_->priority_map_))  // 说明此迭代器已经是 begin，不能再执行 --
							break;

						--priorityMapItr_; //跳到上一个迭代器
						pList = &(priorityMapItr_->second);
						if (pList->empty() == false)
						{
							bFind = true;
							listItr_ = (--_GetListEndIterator(*pList)); //list最后一个元素的迭代器
							break;
						}
					}

					assert(bFind);  // if bFind is false,indicate origin iterator is begin  [ 如果为assert(false),表面当前再执行 --begin() ]
				}
				else
				{	// 如果该迭代器不是该list的begin元素
					//直接 --
					--listItr_;
				}
			}
		}
	public:
		static IteratorImpl _Begin(container_type * const pContainer)
		{
			assert(pContainer);
			IteratorImpl itr;
			itr.pContainer_ = pContainer;
			if (pContainer->empty())
			{
				itr.priorityMapItr_ = _GetPriorityMapEndIterator(pContainer->priority_map_);
				itr.endFlag_ = true;
				assert(itr._GetListPtr() == nullptr);
			}
			else
			{
				bool bFind = false;
				
				for (PriorityMapIterator priorityMapItr = _GetPriorityMapBeginIterator(pContainer->priority_map_); priorityMapItr != _GetPriorityMapEndIterator(pContainer->priority_map_); ++priorityMapItr)
				{
					list_type &l = priorityMapItr->second;
					if (l.empty() == false)
					{
						bFind = true;
						itr.priorityMapItr_ = priorityMapItr;
						itr.listItr_ = _GetListBeginIterator(l);
						itr.endFlag_ = false;
						break;
					}
				}
				assert(bFind);
			}

			return itr;
		}

		static IteratorImpl _End(container_type * const pContainer)
		{
			//assert(pContainer);
			IteratorImpl itr;
			itr.pContainer_ = pContainer;			
			itr.priorityMapItr_ = _GetPriorityMapEndIterator(pContainer->priority_map_);
			itr.endFlag_ = true;
			//itr.listItr_ 为 list_iterator()
			return itr;
		}

	private:
		inline const priority_map_pointer _GetMapPtr()const
		{
			return PriorityMapTool::_GetContainerPointer(priorityMapItr_);
		}

		inline list_pointer _GetListPtr()const
		{
			return ListTool::_GetContainerPointer(listItr_);
		}

		inline static PriorityMapIterator _GetPriorityMapBeginIterator(const priority_map_type &priority_map)
		{
			//return PriorityMapTool::_GetBeginIterator(const_cast<priority_map_type&>(priority_map));
			return PriorityMapTool::_GetBeginIterator(*(const_cast<priority_map_type *>(&priority_map)));
		}

		inline static PriorityMapIterator _GetPriorityMapEndIterator(const priority_map_type &priority_map)
		{
			//return PriorityMapTool::_GetEndIterator(const_cast<priority_map_type&>(priority_map));
			return PriorityMapTool::_GetEndIterator(*(const_cast<priority_map_type *>(&priority_map)));
		}

		inline static ListIterator _GetListBeginIterator(const list_type &l)
		{
			//return ListTool::_GetBeginIterator(const_cast<list_type&>(l));
			return ListTool::_GetBeginIterator(*(const_cast<list_type *>(&l)));
		}

		inline static ListIterator _GetListEndIterator(const list_type &l)
		{
			//return ListTool::_GetEndIterator(const_cast<list_type&>(l));
			return ListTool::_GetEndIterator(*(const_cast<list_type *>(&l)));
		}

	};

	typedef IteratorImpl< 
		typename container_type::priority_map_iterator,
		typename container_type::list_iterator,
		typename IteratorTool<priority_map_type>,
		typename IteratorTool<list_type>
	> IteratorImplEx;

	typedef IteratorImpl< 
		typename container_type::priority_map_reverse_iterator,
		typename container_type::list_reverse_iterator,
		typename ReverseIteratorTool<priority_map_type>,
		typename ReverseIteratorTool<list_type>
	> ReverseIteratorImplEx;

#else
	//方案二：
	template<class PriorityMapIteratorType, class ListIteratorType>
	class IteratorImplBase
	{
	protected:
		typedef typename PriorityMapIteratorType PriorityMapIterator;
		typedef typename ListIteratorType ListIterator;
	public:
		const container_type          *pContainer_;
		PriorityMapIterator            priorityMapItr_;
		ListIterator                   listItr_;
		bool 				           endFlag_;
	public:
		IteratorImplBase() :pContainer_(nullptr), endFlag_(true) {}

		IteratorImplBase(const IteratorImplBase &right)
		{
			_Init(right);
		}

		IteratorImplBase & operator=(const IteratorImplBase &right)
		{
			_Init(right);
			return *this;
		}

	private:
		void _Init(const IteratorImplBase &right)
		{
			this->pContainer_ = right.pContainer_;
			this->priorityMapItr_ = right.priorityMapItr_;
			this->listItr_ = right.listItr_;
			this->endFlag_ = right.endFlag_;
		}
	public:
		list_value_reference operator*() const
		{	// return designated value			
			assert(pContainer_ != nullptr);
			assert(pContainer_->empty() == false);
			assert(_GetListPtr() != nullptr);
			assert(_GetMapPtr() != nullptr);
			assert(!endFlag_); //if assert false, iterator is out of range,please check your logic
			return (*listItr_);
		}

		list_value_pointer operator->() const
		{	// return pointer to class object
			assert(pContainer_ != nullptr);
			assert(pContainer_->empty() == false);
			assert(_GetListPtr() != nullptr);
			assert(_GetMapPtr() != nullptr);
			assert(!endFlag_); //if assert false, iterator is out of range,please check your logic
			return &(*listItr_);
		}

		void operator++() //++itr
		{	// preincrement
			_Increment();
		}

		void operator--()
		{	// predecrement
			_Decrement();
		}

	private:
		virtual void _Increment() = 0;
		virtual void _Decrement() = 0;
		virtual list_pointer _GetListPtr()const = 0;
		virtual priority_map_pointer _GetMapPtr()const = 0;
	};

	class ReverseIteratorImpl
		:public IteratorImplBase<priority_map_reverse_iterator, list_reverse_iterator>
	{
		typedef typename IteratorImplBase<priority_map_reverse_iterator, list_reverse_iterator> base_type;
		typedef typename base_type::PriorityMapIterator PriorityMapIterator;   //具体为：priority_map_reverse_iterator
		typedef typename base_type::ListIterator ListIterator;                 //具体为：list_reverse_iterator
	public:
		bool operator==(const ReverseIteratorImpl &right)const
		{
			assert(this->pContainer_ == right.pContainer_);

			//如果 priority_map_为空  end iterator 比较特殊[需要仔细斟酌]
			if (this->pContainer_->priority_map_.empty())
			{	//此时listItr_是无效的
				assert(this->_GetListPtr() == nullptr && right._GetListPtr() == nullptr);
				assert(this->priorityMapItr_ == pContainer_->priority_map_.rend() && this->priorityMapItr_ == right.priorityMapItr_);
				assert(this->endFlag_ && right.endFlag_);

				return true;
			}
			else
			{
				if (this->endFlag_)
				{
					assert(this->priorityMapItr_ == pContainer_->priority_map_.rend());
					return this->endFlag_ == right.endFlag_
						&& this->priorityMapItr_ == right.priorityMapItr_;
				}
				else
				{
					return this->endFlag_ == right.endFlag_
						&& this->priorityMapItr_ == right.priorityMapItr_
						&& this->listItr_ == right.listItr_;
				}
			}
		}

		inline bool operator!=(const ReverseIteratorImpl&right)const
		{
			return !(this->operator==(right));
		}
	private:
		/*
		*@brief: 如何表示end : priorityMapItr_ == priority_map_.end()  且 listItr_为 list_reverse_iterator()
		*        问题:为什么不用 priorityMapItr_ == priority_map_.end() 且 listItr_为最后一个链表的end()?
		*        回答:如果向priority_map_最后面新添加一个非空list，导致之前的获取的end，执行--end()有问题
		*             即push_back(new_max_priotity,value) 会导致 不同地方获取的end() 不相等
		*@note： 如果容器是空的呢？begin()本身就是end()
		*
		*/
		void _Increment()override
		{
			assert(pContainer_ && pContainer_->empty() == false && endFlag_ == false);

			list_pointer pList = _GetListPtr();
			assert(pList && pList->empty() == false);

			if (++listItr_ != pList->rend())
			{	// 如果该链表后面还有元素
				(void)0; //do nothing
			}
			else
			{	// 如果该链表后面没有元素
				bool bFind = false;
				++priorityMapItr_;
				for (; priorityMapItr_ != pContainer_->priority_map_.rend(); ++priorityMapItr_)
				{	//一直找到非空的链表
					pList = &(priorityMapItr_->second);
					if (pList->empty())
					{
						listItr_ = pList->rend();
						continue;
					}
					else
					{
						bFind = true;
						listItr_ = pList->rbegin();
						break;
					}
				}
				assert(pList);
				endFlag_ = !bFind;
				if (endFlag_)
				{	//说明到了end					
					assert(listItr_ == pList->rend());
					assert(priorityMapItr_ == pContainer_->priority_map_.rend());
					listItr_ = ListIterator(); //实际执行 listItr_ = list_reverse_iterator();
				}
			}
		}

		/*
		*@brief:考虑 --end()
		*		判别出 --begin(),将此种情况进行 assert(false)
		*/
		void _Decrement()override
		{
			assert(pContainer_ && pContainer_->empty() == false);//必须确保是有元素的情况下

			list_pointer pList = _GetListPtr();
			if (endFlag_)
			{	// 说明当前再执行 --end()
				assert(_GetListPtr() == nullptr);
				assert(priorityMapItr_ == pContainer_->priority_map_.rend());
				bool bFind = false;
				while (true)
				{
					--priorityMapItr_;
					pList = &(priorityMapItr_->second);
					assert(pList);
					if (pList->empty() == false)
					{
						bFind = true;
						listItr_ = (--pList->rend()); //list最后一个元素的迭代器
						endFlag_ = false;
						break;
					}

					if (priorityMapItr_ == pContainer_->priority_map_.rbegin())  // 说明此迭代器已经是 begin，不能再执行 --
						break;
				}
				assert(bFind);  // if bFind is false,indicate the container is empty,please check my code			
			}
			else
			{
				assert(pList && pList->empty() == false && listItr_ != pList->rend());
				if (listItr_ == pList->rbegin())
				{	// 如果该迭代器是该list的begin元素
					// 寻找上一个非空的list

					bool bFind = false;
					while (true)
					{
						if (priorityMapItr_ == pContainer_->priority_map_.rbegin())  // 说明此迭代器已经是 begin，不能再执行 --
							break;

						--priorityMapItr_; //跳到上一个迭代器
						pList = &(priorityMapItr_->second);
						if (pList->empty() == false)
						{
							bFind = true;
							listItr_ = (--pList->rend()); //list最后一个元素的迭代器
							break;
						}
					}

					assert(bFind);  // if bFind is false,indicate origin iterator is begin  [ 如果为assert(false),表面当前再执行 --begin() ]
				}
				else
				{	// 如果该迭代器不是该list的begin元素
					//直接 --
					--listItr_;
				}
			}
		}

		virtual list_pointer _GetListPtr()const
		{
			return _GetStdContainerByReverseIterator<list_type, list_reverse_iterator>(listItr_);

		}
		virtual priority_map_pointer _GetMapPtr()const
		{
			return _GetStdContainerByReverseIterator<priority_map_type, priority_map_reverse_iterator>(priorityMapItr_);
		}
	public:
		static ReverseIteratorImpl _Begin(container_type * const pContainer)
		{
			assert(pContainer);
			ReverseIteratorImpl itr;
			itr.pContainer_ = pContainer;
			if (pContainer->empty())
			{
				itr.priorityMapItr_ = pContainer->priority_map_.rend();
				itr.endFlag_ = true;
				assert(itr._GetListPtr() == nullptr);
			}
			else
			{
				bool bFind = false;
				for (priority_map_reverse_iterator priorityMapItr = pContainer->priority_map_.rbegin(); priorityMapItr != pContainer->priority_map_.rend(); ++priorityMapItr)
				{
					list_type &l = priorityMapItr->second;
					if (l.empty() == false)
					{
						bFind = true;
						itr.priorityMapItr_ = priorityMapItr;
						itr.listItr_ = l.rbegin();
						itr.endFlag_ = false;
						break;
					}
				}
				assert(bFind);
			}

			return itr;
		}

		static ReverseIteratorImpl _End(container_type * const pContainer)
		{
			assert(pContainer);
			ReverseIteratorImpl itr;
			itr.pContainer_ = pContainer;
			itr.priorityMapItr_ = pContainer->priority_map_.rend();
			itr.endFlag_ = true;
			//if (pContainer->empty() == false)
			//{
			//	for (priority_map_iterator priorityMapItr = pContainer->priority_map_.begin(); priorityMapItr != pContainer->priority_map_.end(); ++priorityMapItr)
			//	{
			//		itr.listItr_ = priorityMapItr->second.rend();
			//		break;
			//	}
			//}

			return itr;
		}

	};

	class IteratorImpl
		:public IteratorImplBase<priority_map_iterator, list_iterator>
	{
		typedef typename IteratorImplBase<priority_map_iterator, list_iterator> base_type;
		typedef typename base_type::PriorityMapIterator PriorityMapIterator;   //具体为：priority_map_iterator
		typedef typename base_type::ListIterator ListIterator;                 //具体为：list_iterator
	public:
		bool operator==(const IteratorImpl &right)const
		{
			assert(this->pContainer_ == right.pContainer_);

			//如果 priority_map_为空  end iterator 比较特殊[需要仔细斟酌]
			if (this->pContainer_->priority_map_.empty())
			{	//此时listItr_是无效的
				assert(this->_GetListPtr() == nullptr && right._GetListPtr() == nullptr);
				assert(this->priorityMapItr_ == pContainer_->priority_map_.end() && this->priorityMapItr_ == right.priorityMapItr_);
				assert(this->endFlag_ && right.endFlag_);

				return true;
			}
			else
			{
				if (this->endFlag_)
				{
					assert(this->priorityMapItr_ == pContainer_->priority_map_.end());
					return this->endFlag_ == right.endFlag_
						&& this->priorityMapItr_ == right.priorityMapItr_;
				}
				else
				{
					return this->endFlag_ == right.endFlag_
						&& this->priorityMapItr_ == right.priorityMapItr_
						&& this->listItr_ == right.listItr_;
				}
			}
		}

		inline bool operator!=(const IteratorImpl&right)const
		{
			return !(this->operator==(right));
		}
	private:
		/*
		*@brief: 如何表示end : priorityMapItr_ == priority_map_.end() listItr_为最后一个链表的end,
		*@note： 如果容器是空的呢？begin()本身就是end()
		*
		*/
		void _Increment()override
		{
			assert(pContainer_ && pContainer_->empty() == false && endFlag_ == false);

			list_pointer pList = _GetListPtr();
			assert(pList && pList->empty() == false);

			if (++listItr_ != pList->end())
			{	// 如果该链表后面还有元素
				(void)0; //do nothing
			}
			else
			{	// 如果该链表后面没有元素
				bool bFind = false;
				++priorityMapItr_;
				for (; priorityMapItr_ != pContainer_->priority_map_.end(); ++priorityMapItr_)
				{	//一直找到非空的链表
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
				{	//说明到了end					
					assert(listItr_ == pList->end());
					assert(priorityMapItr_ == pContainer_->priority_map_.end());
					listItr_ = ListIterator();  //实际执行 listItr_ = list_iterator();
				}
			}
		}

		/*
		*@brief:考虑 --end()
		*		判别出 --begin(),将此种情况进行 assert(false)
		*/
		void _Decrement()override
		{
			assert(pContainer_ && pContainer_->empty() == false);//必须确保是有元素的情况下

			list_pointer pList = _GetListPtr();
			if (endFlag_)
			{	// 说明当前再执行 --end()
				assert(_GetListPtr() == nullptr);
				assert(priorityMapItr_ == pContainer_->priority_map_.end());
				bool bFind = false;
				while (true)
				{
					--priorityMapItr_;
					pList = &(priorityMapItr_->second);
					assert(pList);
					if (pList->empty() == false)
					{
						bFind = true;
						listItr_ = (--pList->end()); //list最后一个元素的迭代器
						endFlag_ = false;
						break;
					}

					if (priorityMapItr_ == pContainer_->priority_map_.begin())  // 说明此迭代器已经是 begin，不能再执行 --
						break;
				}
				assert(bFind);  // if bFind is false,indicate the container is empty,please check my code			
			}
			else
			{
				assert(pList && pList->empty() == false && listItr_ != pList->end());
				if (listItr_ == pList->begin())
				{	// 如果该迭代器是该list的begin元素
					// 寻找上一个非空的list

					bool bFind = false;
					while (true)
					{
						if (priorityMapItr_ == pContainer_->priority_map_.begin())  // 说明此迭代器已经是 begin，不能再执行 --
							break;

						--priorityMapItr_; //跳到上一个迭代器
						pList = &(priorityMapItr_->second);
						if (pList->empty() == false)
						{
							bFind = true;
							listItr_ = (--pList->end()); //list最后一个元素的迭代器
							break;
						}
					}

					assert(bFind);  // if bFind is false,indicate origin iterator is begin  [ 如果为assert(false),表面当前再执行 --begin() ]
				}
				else
				{	// 如果该迭代器不是该list的begin元素
					//直接 --
					--listItr_;
				}
			}
		}

		inline virtual list_pointer _GetListPtr()const
		{
			return _GetStdContainerByIterator<list_type, list_iterator>(listItr_);

		}
		inline virtual priority_map_pointer _GetMapPtr()const
		{
			return _GetStdContainerByIterator<priority_map_type, priority_map_iterator>(priorityMapItr_);
		}
	public:
		static IteratorImpl _Begin(container_type * const pContainer)
		{
			assert(pContainer);
			IteratorImpl itr;
			itr.pContainer_ = pContainer;
			if (pContainer->empty())
			{
				itr.priorityMapItr_ = pContainer->priority_map_.end();
				itr.endFlag_ = true;
				assert(itr._GetListPtr() == nullptr);
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

		static IteratorImpl _End(container_type * const pContainer)
		{
			assert(pContainer);
			IteratorImpl itr;
			itr.pContainer_ = pContainer;
			itr.priorityMapItr_ = pContainer->priority_map_.end();
			itr.endFlag_ = true;
			//itr.listItr_ 为 list_iterator()
			return itr;
		}

	};

#endif

public:
	template<class IteratorImplType>
	class Iterator
	{
		friend class priority_linked_hash_map;
	public:
		IteratorImplType impl_;
	public:
		const list_iterator& listItr()const { return impl_.listItr_; }
	public:
		Iterator() {}
		Iterator(const Iterator &right)
		{
			this->impl_ = right.impl_;
		}

		Iterator & operator=(const Iterator &right)
		{
			this->impl_ = right.impl_;
			return *this;
		}

		inline list_value_reference operator*() const
		{	// return designated value
			return (*impl_);
		}

		inline list_value_pointer operator->() const
		{	// return pointer to class object
			return &(*impl_);
		}

		Iterator& operator++() //++itr
		{	// preincrement
			++impl_;
			return (*this);
		}

		Iterator operator++(int) //itr++
		{	// postincrement
			Iterator tempItr = *this;
			++impl_;
			return tempItr;
		}

		Iterator& operator--()
		{	// predecrement
			--impl_;
			return (*this);
		}

		Iterator operator--(int)
		{	// postdecrement
			Iterator tempItr = *this;
			--impl_;
			return tempItr;
		}

		friend bool operator==(const Iterator &left, const Iterator &right)
		{
			return left.impl_ == right.impl_;
		}

		friend bool operator!=(const Iterator &left, const Iterator &right)
		{
			return left.impl_ != right.impl_;
		}

	private:
		static Iterator _Begin(container_type * const pContainer)
		{
			Iterator itr;
			itr.impl_ = IteratorImplType::_Begin(pContainer);
			return itr;
		}

		static Iterator _End(container_type * const pContainer)
		{
			Iterator itr;
			itr.impl_ = IteratorImplType::_End(pContainer);
			return itr;
		}
	};

	template<class IteratorImplType>
	class ConstIterator
	{
		friend class priority_linked_hash_map;
	public:
		IteratorImplType impl_;
	public:
		const list_iterator& listItr()const { return impl_.listItr_; }
	public:
		ConstIterator() {}
		ConstIterator(const ConstIterator &right)
		{
			this->impl_ = right.impl_;
		}

		ConstIterator(const Iterator<IteratorImplType> &right)
		{
			this->impl_ = right.impl_;
		}

		ConstIterator & operator=(const ConstIterator &right)
		{
			this->impl_ = right.impl_;
			return *this;
		}

		ConstIterator & operator=(const Iterator<IteratorImplType> &right)
		{
			this->impl_ = right.impl_;
			return *this;
		}

		inline const_list_value_reference operator*() const
		{	// return designated value
			return (*impl_);
		}

		inline const_list_value_pointer operator->() const
		{	// return pointer to class object
			return &(*impl_);
		}

		ConstIterator& operator++() //++itr
		{	// preincrement
			++impl_;
			return (*this);
		}

		ConstIterator operator++(int) //itr++
		{	// postincrement
			Iterator tempItr = *this;
			++impl_;
			return tempItr;
		}

		ConstIterator& operator--()
		{	// predecrement
			--impl_;
			return (*this);
		}

		ConstIterator operator--(int)
		{	// postdecrement
			Iterator tempItr = *this;
			--impl_;
			return tempItr;
		}

		friend bool operator==(const ConstIterator &left, const ConstIterator &right)
		{
			return left.impl_ == right.impl_;
		}

		friend bool operator==(const Iterator<IteratorImplType> &left, const ConstIterator &right)
		{
			return left.impl_ == right.impl_;
		}

		friend bool operator==(const ConstIterator &left, const Iterator<IteratorImplType> &right)
		{
			return left.impl_ == right.impl_;
		}

		friend bool operator!=(const ConstIterator &left, const ConstIterator &right)
		{
			return left.impl_ != right.impl_;
		}

		friend bool operator!=(const Iterator<IteratorImplType> &left, const ConstIterator &right)
		{
			return left.impl_ != right.impl_;
		}

		friend bool operator!=(const ConstIterator &left, const Iterator<IteratorImplType> &right)
		{
			return left.impl_ != right.impl_;
		}

	private:
		static ConstIterator _Begin(const container_type * const pContainer)
		{
			ConstIterator c_itr;
			c_itr.impl_ = IteratorImplType::_Begin(const_cast<container_type *>(&(*pContainer)));
			return c_itr;
		}

		static ConstIterator _End(const container_type * const pContainer)
		{
			ConstIterator c_itr;
			c_itr.impl_ = IteratorImplType::_End(const_cast<container_type *>(&(*pContainer)));
			return c_itr;
		}

		Iterator<IteratorImplType> _Make_itr()
		{
			Iterator<IteratorImplType> itr;
			itr.impl_ = this->impl_;
			return itr;
		}
	};

	//Consistency with the standard library 与标准库保持一致性
#ifdef __USE_PLAN_ONE
	typedef typename Iterator<IteratorImplEx> iterator;
	typedef typename ConstIterator<IteratorImplEx> const_iterator;

	typedef typename Iterator<ReverseIteratorImplEx> reverse_iterator;
	typedef typename ConstIterator<ReverseIteratorImplEx> const_reverse_iterator;
#else
	typedef typename Iterator<IteratorImpl> iterator;
	typedef typename ConstIterator<IteratorImpl> const_iterator;

	typedef typename Iterator<ReverseIteratorImpl> reverse_iterator;
	typedef typename ConstIterator<ReverseIteratorImpl> const_reverse_iterator;
#endif

	typedef typename list_value_reference reference;
	typedef typename const_list_value_reference const_reference;
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
#define _DEBUG_STRICK_CHECK
#ifdef _DEBUG_STRICK_CHECK
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
		return _GetStdContainerByIterator<list_type, list_iterator>(listItr);
	}

	list_pointer _GetListPtr(const_list_iterator listItr)
	{
		return _GetStdContainerByIterator<list_type, const_list_iterator>(listItr);
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
			bExisted = true;
			if (bCover)
			{
				//方案一： 原位置覆盖
				//list_iterator listItr = hashMapItr->second;
				//*listItr = val; //调用list_value_type的赋值构造函数

				//方案二：删除原来的，然后再push [这种更符合 "按照添加的顺序"的语义]
				list_iterator oldListItr = hashMapItr->second;
				list_pointer pList = _GetListPtr(oldListItr);
				assert(pList && oldListItr != pList->end());

				//1.删除旧元素
				pList->erase(oldListItr);
				//2.插入新元素
				list_iterator newListItr;
				if (bPushback)
					newListItr = pList->insert(pList->end(), val);
				else
					newListItr = pList->insert(pList->begin(), val);

				hashMapItr->second = newListItr;            //保存新的迭代器

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

			//查找priority
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
		return (*begin());
	}

	inline const_list_value_reference front()const
	{
		assert(empty() == false);
		return (*begin());
	}

	inline list_value_reference back()
	{
		assert(empty() == false);
		//return (*(--end()));
		return (*rbegin()); // Better performance
	}

	inline const_list_value_reference back()const
	{
		assert(empty() == false);
		//return (*(--end()));
		return (*rbegin()); // Better performance
	}

	inline void pop_front()
	{
		assert(empty() == false);
		erase(begin());
	}

	inline void pop_back()
	{
		assert(empty() == false);
		erase(--end());
	}

	//**************************************************************************************************
	/*
	void pop_front()
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

	void pop_back()
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


	list_value_reference front()
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

	const_list_value_reference front()const
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

	list_value_reference back()
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

	const_list_value_reference back()const
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
	*/

	//**************************************************************************************************
	size_type remove(const key_type &k)
	{//清理保持一致性
		hash_map_iterator hashMapItr = hash_map_.find(k);
		if (hashMapItr != hash_map_.end())
		{
			list_iterator listItr = hashMapItr->second;
			//通过listItr 获取 list
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

	iterator erase(const_iterator itr)
	{//清理保持一致性
		assert(itr != end());

		const_iterator rtnItr = itr; //copy
		++rtnItr;

		list_pointer pList = _GetListPtr(itr.listItr());
		assert(pList && itr.listItr() != pList->end());
		//1.
		const key_type &k = obtain_key_(*(itr.listItr()));
		hash_map_iterator hashMapItr = hash_map_.find(k);
		assert(hashMapItr != hash_map_.end());
		hash_map_.erase(hashMapItr);

		//2.
		pList->erase(itr.listItr());//会使得itr.listItr_失效。如果 执行 ++itr，会出问题,所以拷贝一份，执行++

		return rtnItr._Make_itr(); //借鉴 std::list
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

	iterator begin()
	{
		return iterator::_Begin(this);
	}

	const_iterator begin()const
	{
		return const_iterator::_Begin(this);
	}

	const_iterator cbegin()const
	{
		return const_iterator::_Begin(this);
	}

	iterator end()
	{
		return iterator::_End(this);
	}

	const_iterator end()const
	{
		return const_iterator::_End(this);
	}

	const_iterator cend()const
	{
		return const_iterator::_End(this);
	}

	reverse_iterator rbegin()
	{
		return reverse_iterator::_Begin(this);
	}

	const_reverse_iterator rbegin()const
	{
		return const_reverse_iterator::_Begin(this);
	}

	const_reverse_iterator crbegin()const
	{
		return const_reverse_iterator::_Begin(this);
	}

	reverse_iterator rend()
	{
		return reverse_iterator::_End(this);
	}

	const_reverse_iterator rend()const
	{
		return const_reverse_iterator::_End(this);
	}

	const_reverse_iterator crend()const
	{
		return const_reverse_iterator::_End(this);
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

	//正序遍历
	template<typename Func>
	void for_each(Func f)const
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
	void for_each_reverse(Func f)const
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
