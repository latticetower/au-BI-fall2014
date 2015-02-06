#pragma once

#include<memory>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
//#include <boost/shared_ptr.hpp>


typedef int KeyType;

template<class ElementType>
struct Node {
    Node(ElementType element_, KeyType key_, boost::shared_ptr<Node<ElementType> > next_ = NULL)
        : element(element_), key(key_), next(next_) {
    }
    Node() {
        element = ElementType();
        key = 0;
        next = NULL;
    }

    void lock() {
        mutex_.lock();
    }
    void unlock() {
        mutex_.unlock();
    }

    ElementType element;
    KeyType key;
    boost::shared_ptr<Node<ElementType> > next;
    boost::mutex mutex_;
};

template<class ElementType, class NextElementType >
class MarkedNode {
  public:
    MarkedNode(boost::shared_ptr<NextElementType > next_, bool marked_ = false)
          : next(next_), marked(marked_) { }
    MarkedNode(NextElementType * next_, bool marked_ = false)
          : next(next_), marked(marked_) { }

    MarkedNode() : next(nullptr), marked(false) {  }
    MarkedNode(MarkedNode const& that) : next(that.next), marked(that.marked) {  }


    boost::shared_ptr<NextElementType> next;
    bool marked;
};


template<class ElementType>
struct MarkedAtomicNode {
    typedef MarkedNode<ElementType, MarkedAtomicNode<ElementType> > InnerNode;

    MarkedAtomicNode(ElementType element_,
            KeyType key_,
            boost::shared_ptr<MarkedAtomicNode<ElementType> > next_ = nullptr,
            bool marked_ = false) {
        data = boost::shared_ptr<boost::atomic<InnerNode* > >(new boost::atomic<InnerNode* >(new InnerNode(next_, marked_)));
        element = element;
        key = key_;
    }

    MarkedAtomicNode() {
        data = boost::shared_ptr<boost::atomic<InnerNode* > >(new InnerNode());
        element = ElementType();
        key = KeyType();
    }

    boost::shared_ptr<boost::atomic<InnerNode* > > data;
    ElementType element;
    KeyType key;

    ~MarkedAtomicNode() {
        auto contents = data->load(boost::memory_order_relaxed);
        delete contents;
    }

    ElementType get_element() {
        return element;
    }

    boost::shared_ptr<MarkedAtomicNode<ElementType> > next() {
        InnerNode* inner_data = data->load(boost::memory_order_relaxed);
        return inner_data->next;
    }

    KeyType get_key() {
        return key;
    }

    bool marked() {
        InnerNode* inner_data = data->load(boost::memory_order_relaxed);
        return inner_data->marked;
    }

    /* method returns copy of newly created object ptr with modified next and marked fields*/
    InnerNode* construct_new(
            boost::shared_ptr<MarkedAtomicNode<ElementType> > new_next_ptr,
            bool new_mark) {
        return new InnerNode(new_next_ptr, new_mark);
    }

    bool compareAndSet(
        boost::shared_ptr<MarkedAtomicNode<ElementType> > next_ptr,
        boost::shared_ptr<MarkedAtomicNode<ElementType> > new_next_ptr,
        bool next_mark = false,
        bool new_next_mark = false
    ) {
        typedef MarkedAtomicNode<ElementType> NodeValue;

        auto current_value = this->data->load(boost::memory_order_relaxed);
        bool next_ptr_equals_to_this = next_ptr == current_value->next && next_mark == current_value->marked;
        if (!next_ptr_equals_to_this) {
            return false;
        }

        bool new_ptr_equals_to_this = new_next_ptr == current_value->next && new_next_mark == current_value->marked;
        if (new_ptr_equals_to_this) {
          //delete new_value;
          return true; //there is no need for memory manipulation, just clear temp new_value and exit
        }
        InnerNode* new_value = this->construct_new(new_next_ptr, new_next_mark);

        auto cas_result = CAS(current_value, new_value);
        if (cas_result) {
            delete current_value;
        } else {
            delete new_value;
        }
        return cas_result;
    }

    bool tryToUpdateData(boost::shared_ptr<MarkedAtomicNode<ElementType> > nnext, bool mark) {

        InnerNode * current = data->load();
        if (nnext != current->next)
            return false;

        if (mark == current->marked) return true;
        InnerNode* new_value = this->construct_new(nnext, mark);

        if (this->data->compare_exchange_strong(current, new_value)) {
            delete current;
            return true;
        }else {
            delete new_value;
            return false;
        }
    }

    bool CAS(InnerNode* current_value, InnerNode* new_value) {
        //the following function call is hidden inside this method just for debug purposes
        return (this->data->compare_exchange_strong(current_value, new_value));
    }
};




template<class ElementType>
class SyncList{
  public:
    virtual void insert(ElementType & element, KeyType key) {    }
    virtual void erase(KeyType key) {    }
    virtual std::pair<bool, ElementType> find(KeyType key) {   }
    virtual ~SyncList() {}
};
