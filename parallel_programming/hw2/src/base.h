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
  MarkedNode(ElementType element_, int key_, std::shared_ptr<NextElementType > next_ = NULL, bool marked_ = false)
  : element(element_), key(key_), marked(marked_), next(next_) {
  }
  MarkedNode() {
    element = ElementType();
    key = 0;
    marked = false;
    next = NULL;
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

  MarkedAtomicNode()
      : data(InnerNode(ElementType(), 0, NULL, false)) {
  }

  boost::atomic<InnerNode> data;

  ElementType element() {
      return data.load().element;
  }

  std::shared_ptr<MarkedAtomicNode<ElementType> > next() {
      return data.load().next;
  }
  bool key() {
    return data.load().key;
  }

  bool marked() {
    return data.load().marked;
  }

};

/* method returns copy of newly created object ptr with modified next and marked fields*/
template<class ElementType>
std::shared_ptr<MarkedAtomicNode<ElementType> > construct_new_from_atomic(
        boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& atomic_node,
        std::shared_ptr<MarkedAtomicNode<ElementType> >& new_ptr,
        bool new_mark) {
    auto node = atomic_node.load();
    return std::make_shared<MarkedAtomicNode<ElementType> >(node->element, node->key, new_mark, new_ptr.load());
}


/* method loads next and marked value to corresponding variables from given atomic obj ptr*/
template<class ElementType>
void load_value(boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& atomic_node,
                boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& next_node,
                bool & marked) {
  auto atomic_value = atomic_node.load();
  next_node = atomic_value->next;
  marked = atomic_value->marked;
}

template<class ElementType>
bool compareAndSet(
        std::shared_ptr<MarkedAtomicNode<ElementType> >& atomic_node,
        std::shared_ptr<MarkedAtomicNode<ElementType> >& next_ptr,
        std::shared_ptr<MarkedAtomicNode<ElementType> >& new_next_ptr,
        bool next_mark = false,
        bool new_next_mark = false
        ) {
    typedef MarkedAtomicNode<ElementType> NodeValue;
    auto value_to_compare = construct_new_from_atomic(atomic_node, next_ptr, next_mark);
    auto new_value = construct_new_from_atomic(atomic_node, new_next_ptr, new_next_mark);
    return (atomic_node.compare_exchange_weak(&value_to_compare, new_value));
}


template<class ElementType>
class SyncList{
  public:
    virtual void insert(ElementType & element, int key) {    }
    virtual void erase(int key) {    }
    virtual std::pair<bool, ElementType> find(int key) {   }
    virtual void print(std::ostream&os) {}
    virtual ~SyncList() {}
};
