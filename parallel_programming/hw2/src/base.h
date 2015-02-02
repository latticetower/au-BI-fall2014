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

template<class ElementType>
struct MarkedAtomicNode {
  MarkedAtomicNode(ElementType element_, int key_,
          std::shared_ptr<MarkedAtomicNode<ElementType> > next_ = NULL,
          bool marked_ = false) : element(element_), key(key_), marked(marked_), next(next_) {
  }
  MarkedAtomicNode() {
    element = ElementType();
    key = 0;
    marked = false;
    next = NULL;
  }
  std::shared_ptr<MarkedAtomicNode<ElementType> > get_next_ptr() {
      std::shared_ptr<MarkedAtomicNode<ElementType> > next_node = next.load();
      return next;
  }

  ElementType element;
  int key;
  bool marked;
  boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > > next;
};

/* method returns object ptr for given atomic*/
template<class ElementType>
std::shared_ptr<MarkedAtomicNode<ElementType> > get_ptr(boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& atomic_node) {
  return atomic_node.load();
}

/* method returns copy of newly created object ptr with modified next and marked fields*/
template<class ElementType>
std::shared_ptr<MarkedAtomicNode<ElementType> > construct_new_from_atomic(
        boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& atomic_node,
        std::shared_ptr<MarkedAtomicNode<ElementType> >& new_ptr,
        bool new_mark) {
    auto node = atomic_node.load();
    return std::make_shared<MarkedAtomicNode<ElementType> >(node->element, node->key, new_mark, new_ptr.load());
}

/* method returns key for given atomic*/
template<class ElementType>
int get_key(boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& atomic_node) {
  return atomic_node.load()->key;
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
        boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& atomic_node,
        boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& next_ptr,
        boost::atomic<std::shared_ptr<MarkedAtomicNode<ElementType> > >& new_next_ptr,
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
