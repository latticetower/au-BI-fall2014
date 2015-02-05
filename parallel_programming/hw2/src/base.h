#pragma once

//#include<boost/atomic.hpp>
#include<boost/shared_ptr.hpp>

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

  ElementType element;
  KeyType key;
  boost::shared_ptr<Node<ElementType> > next;
};

template<class ElementType, class NextElementType >
class MarkedNode {
public:
  MarkedNode(ElementType element_, KeyType key_,
    boost::shared_ptr<NextElementType >& next_,
    bool marked_ = false)
  : element(element_), key(key_), next(next_), marked(marked_) {
    //std::cout << "made new MarkedNode with key="<< key <<", next=" <<next_<<"\n";
  }

  MarkedNode(ElementType element_, KeyType key_)
  : element(element_), key(key_), next(nullptr), marked(false) {
    //std::cout << "made new MarkedNode{e} with key="<< key <<", next=" <<next<<"\n";
  }

  MarkedNode() : element(ElementType()), next(nullptr), marked(false) {
    key = std::numeric_limits<KeyType>::max();
    //std::cout << "made new {empty }MarkedNode\n";
  }
  MarkedNode(MarkedNode & that) : element(that.element), key(that.key), next(that.next), marked(that.marked) {
    //std::cout <<"Marked node copy c\n";
  }
  /*
  bool operator == (MarkedNode<ElementType, NextElementType> const & that){
    std::cout << "in  == : " << (this->key==that.key) << " \n";
    std::cout << "in  == : " << (this->marked == that.marked) << "\n ";
    std::cout << "in  == : " <<  (this->next == that.next)<<"\n";
    return this->key == that.key && this->marked == that.marked && this->next == that.next;
  }*/

  ElementType element;
  KeyType key;
  boost::shared_ptr<NextElementType > next;
  bool marked;
};


template<class ElementType>
struct MarkedAtomicNode {
  typedef MarkedNode<ElementType, MarkedAtomicNode<ElementType> > InnerNode;

  MarkedAtomicNode(ElementType element_,
          KeyType key_,
          boost::shared_ptr<MarkedAtomicNode<ElementType> > next_ = nullptr,
          bool marked_ = false) {
      data.store(new InnerNode(element_, key_, next_, marked_));

      //std::cout << "made new MarkedAtomicNode "<< get_key() << " "<< key_ <<"\n";
  }


  /*bool operator == (MarkedAtomicNode<ElementType> & that){
    return this->data == that.data;
  }*/

  MarkedAtomicNode() {
        data.store(new InnerNode());

        //std::cout << "made new {empty }MarkedAtomicNode\n";
  }

  std::atomic<InnerNode* > data;

  ElementType element() {
      //std::cout <<"in element ";
      auto inner_data = data.load(std::memory_order_relaxed);
      //std::cout << inner_data->key << "\n";
      return inner_data->element;
  }

  boost::shared_ptr<MarkedAtomicNode<ElementType> > next() {
      //std::cout << "retrieving next\n"<< data.load(std::memory_order_relaxed)->element <<" "<<
      //data.load(std::memory_order_relaxed)->key << " " << data.load(std::memory_order_relaxed)->next <<"\n";
      //std::cout <<"in next ";
      auto inner_data = data.load(std::memory_order_relaxed);
      //std::cout << inner_data->key <<"\n";
      //std::cout << (inner_data == nullptr) <<"\n";
      boost::shared_ptr<MarkedAtomicNode<ElementType> > result = inner_data->next;

      return result;
  }
  KeyType get_key() {
    //std::cout <<"in key ";
    auto inner_data = data.load(std::memory_order_relaxed);
    //std::cout << inner_data->key <<"\n";
    return inner_data->key;
  }
  
  InnerNode* get() const{
      return data.load(std::memory_order_relaxed);
  }


  bool marked() {
      //std::cout <<"in marked ";
      auto inner_data = data.load(std::memory_order_relaxed);
      //std::cout << inner_data->key << "\n";
      return inner_data->marked;
  }

  /* method returns copy of newly created object ptr with modified next and marked fields*/
  InnerNode * construct_new(
      boost::shared_ptr<MarkedAtomicNode<ElementType> >& new_next_ptr,
      bool new_mark) {
    //    std::cout <<"construct_new\n";
    auto inner_data = data.load(std::memory_order_relaxed);
    return new InnerNode(inner_data->element, inner_data->key, new_next_ptr, new_mark);
  }

  bool compareAndSet(
    boost::shared_ptr<MarkedAtomicNode<ElementType> >& next_ptr,
    boost::shared_ptr<MarkedAtomicNode<ElementType> >& new_next_ptr,
    bool next_mark = false,
    bool new_next_mark = false
  ) {
    //std::cout<<"nnnh\n";
    typedef MarkedAtomicNode<ElementType> NodeValue;
    //auto value_to_compare = this->construct_new(next_ptr, next_mark);

    auto new_value = this->construct_new(new_next_ptr, new_next_mark);
    //std::cout << value_to_compare << " "<<new_value<<"\n";
    auto current_value = this->data.load(std::memory_order_relaxed);
    //std::cout << next_ptr<< " "<< current_value->next<<"\n";
    //std::cout << this->data.load(std::memory_order_relaxed) << " " << current_value << "\n";
    //std::cout << new_value << " "<< (this->data.load(std::memory_order_relaxed) == *value_to_compare) <<"\n";
    //std::cout << (next_ptr == current_value->next) << " " << (next_mark == current_value->marked) <<" " <<
    //    (new_next_ptr == current_value->next)<<" " << (new_next_mark == current_value->marked)<<"\n";
    return next_ptr == current_value->next && next_mark == current_value->marked &&
          (
            (new_next_ptr == current_value->next && new_next_mark == current_value->marked)
            || CAS(current_value, new_value));
  }
  bool CAS(InnerNode* current_value, InnerNode* new_value) {
    if(this->data.compare_exchange_strong(current_value, new_value, std::memory_order_relaxed)){
      //std::cout << "CAS returned true\n";
      return true;
    };
    //std::cout << "CAS returned false\n";
    return false;
  }
};




template<class ElementType>
class SyncList{
  public:
    virtual void insert(ElementType & element, KeyType key) {    }
    virtual void erase(KeyType key) {    }
    virtual std::pair<bool, ElementType> find(KeyType key) {   }
    virtual void print(std::ostream&os) {}
    virtual ~SyncList() {}
};
