#pragma once

#include<boost/atomic.hpp>

template<class ElementType>
struct Node {
  Node(ElementType element_, int key_, std::shared_ptr<Node<ElementType> > next_ = NULL)
  : element(element_), key(key_), next(next_) {
  }
  Node() {
    element = ElementType();
    key = 0;
    next = NULL;
  }

  ElementType element;
  int key;
  std::shared_ptr<Node<ElementType> > next;
};

template<class ElementType, class NextElementType >
struct MarkedNode {
  MarkedNode(ElementType element_, int key_,
    std::shared_ptr<NextElementType > next_ = NULL,
    bool marked_ = false)
  : element(element_), key(key_), marked(marked_), next(next_) {
  }
  MarkedNode() {
    element = ElementType();
    key = 0;
    marked = false;
    next = NULL;
  }
  bool operator == (MarkedNode<ElementType, NextElementType> & that){
    return this->key == that.key && this->marked == that.marked && this->next == that.next;
  }
  ElementType element;
  int key;
  bool marked;
  std::shared_ptr<NextElementType > next;
};


template<class ElementType>
struct MarkedAtomicNode {
  typedef MarkedNode<ElementType, MarkedAtomicNode<ElementType> > InnerNode;

  MarkedAtomicNode(ElementType element_, int key_,
          std::shared_ptr<MarkedAtomicNode<ElementType> > next_ = NULL,
          bool marked_ = false)
              : data(InnerNode(element_, key_, next_, marked_)) {
  }
  bool operator == (MarkedAtomicNode<ElementType> & that){
    return this->data == that.data;
  }

  MarkedAtomicNode()
      : data(InnerNode(ElementType(), 0, NULL, false)) {
  }

  boost::atomic<InnerNode > data;

  ElementType element() {
      return data.load().element;
  }

  std::shared_ptr<MarkedAtomicNode<ElementType> > next() {
      return data.load().next;
  }
  int key() {
    return data.load().key;
  }

  bool marked() {
    return data.load().marked;
  }

  /* method returns copy of newly created object ptr with modified next and marked fields*/
  std::shared_ptr<InnerNode > construct_new(
      std::shared_ptr<MarkedAtomicNode<ElementType> >& new_next_ptr,
      bool new_mark) {
    auto inner_data = data.load();
    return std::make_shared<InnerNode >(inner_data.element, inner_data.key, new_next_ptr, new_mark);
  }


  bool compareAndSet(
    std::shared_ptr<MarkedAtomicNode<ElementType> > next_ptr,
    std::shared_ptr<MarkedAtomicNode<ElementType> > new_next_ptr,
    bool next_mark = false,
    bool new_next_mark = false
  ) {
    typedef MarkedAtomicNode<ElementType> NodeValue;
    auto value_to_compare = this->construct_new(next_ptr, next_mark);
    std::cout << value_to_compare <<"\n";
    auto new_value = this->construct_new(new_next_ptr, new_next_mark);
    std::cout << new_value << " "<< (this->data.load() == *value_to_compare) <<"\n";
    return (this->data.compare_exchange_strong(*value_to_compare, *new_value));
  }
};




template<class ElementType>
class SyncList{
  public:
    virtual void insert(ElementType & element, int key) {    }
    virtual void erase(int key) {    }
    virtual std::pair<bool, ElementType> find(int key) {   }
    virtual void print(std::ostream&os) {}
    virtual ~SyncList() {}
};
