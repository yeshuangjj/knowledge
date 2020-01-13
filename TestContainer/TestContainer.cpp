// TestContainer.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <list>
#include <map>
#include <string>

//#include "../src/CustomContainer/linked_hash_map_helper.h"
#include "../src/CustomContainer/test/priority_linked_hash_map_test.h"


typedef std::list<int> list_type;
typedef std::map<int, std::string> map_type;

//
//typedef IteratorTool<map_type> PriorityMapIteratorTool;
//typedef ReverseIteratorTool<map_type> PriorityMapReverseIteratorTool;
//typedef IteratorTool<list_type> ListIteratorTool;
//typedef ReverseIteratorTool<list_type> ListReverseIteratorTool;
//
//template< class PriorityMapToolType, class ListToolType>
//class Test :public ToolWrapper<PriorityMapToolType>::type,public ToolWrapper<ListToolType>::type
//{
//	typedef typename ToolWrapper<PriorityMapToolType>::type  PriorityMapTool;
//	typedef typename ToolWrapper<ListToolType>::type         ListTool;
//    typedef typename Test<PriorityMapToolType, ListToolType> this_type;
//public:
//    static void foo()
//    {
//        list_type l;        
//        l.push_back(1);
//        l.push_back(2);
//        l.push_back(3);
//
//        map_type m;
//        m.insert(std::make_pair(10, std::string("a")));
//        m.insert(std::make_pair(11, std::string("b")));
//        m.insert(std::make_pair(12, std::string("c")));
//
//        //Test * p = new this_type();
//        Test obj;
//        {
//            auto itr_begin = obj.ListTool::_GetBeginIterator(l);
//            auto itr_end = obj.ListTool::_GetEndIterator(l);
//            auto list_pointer = obj.ListTool::_GetContainerPointer(itr_begin);
//        }
//
//        {
//            auto itr_begin = obj.PriorityMapTool::_GetBeginIterator(m);
//            auto itr_end = obj.PriorityMapTool::_GetEndIterator(m);
//            auto list_pointer = obj.PriorityMapTool::_GetContainerPointer(itr_begin);
//        }
//    }
//
//};



int main()
{
    {
        list_type l;
        l.push_back(1);
        //list_type::iterator itr;
        //itr != l.begin();

        l.begin()._Getcont();
    }

    {
        map_type m;
        m.insert(std::make_pair(3, std::string("c")));
        //map_type::iterator itr;
        //itr == m.begin();

        m.begin() == m.cbegin();
    }


 
    //Test<PriorityMapReverseIteratorTool, ListReverseIteratorTool>::foo();
    priority_linked_hash_map_test::test_case::test_all();
    return 0;
}

