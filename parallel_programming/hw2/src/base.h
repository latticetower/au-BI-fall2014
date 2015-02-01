#pragma once

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
class SyncList{
  public:
    virtual void insert(ElementType & element, int key) {    }
    virtual void erase(int key) {    }
    virtual std::pair<bool, ElementType> find(int key) {   }
    virtual void print(std::ostream&os) {}
    virtual ~SyncList() {}
};
