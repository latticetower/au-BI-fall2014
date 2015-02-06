#pragma once
#include "base.h"

template<class ElementType>
class LockableList: public SyncList<ElementType>{
  public:
    LockableList() {
        auto tail = boost::shared_ptr<Node<ElementType> >(new Node<ElementType>(ElementType(),
            std::numeric_limits<KeyType>::max()));
        head = boost::shared_ptr<Node<ElementType> >(new Node<ElementType>(ElementType(),
            std::numeric_limits<KeyType>::min(), tail));
    }

    void insert(ElementType & element, KeyType key) {
        boost::shared_ptr<Node<ElementType> > pred = head;
        pred->lock();
        boost::shared_ptr<Node<ElementType> > curr = head->next;
        curr->lock();
        while (curr->key < key) {
          pred->unlock();
          pred = curr;
          curr = curr->next;
          curr->lock();
        }
        if (curr->key == key) {
          curr->unlock();
          pred->unlock();
          return;
        } else {
          pred->next = boost::shared_ptr<Node<ElementType> >(new Node<ElementType>(element, key, curr));
          curr->unlock();
          pred->unlock();
          return;
        }
    }


    void erase(KeyType key) {
        boost::shared_ptr<Node<ElementType> > pred = head;
        pred->lock();
        boost::shared_ptr<Node<ElementType> > curr = head->next;
        curr->lock();
        while (curr->key < key) {
            pred->unlock();
            pred = curr;
            curr = curr->next;
            curr->lock();
        }
        if (curr->key != key) {
            curr->unlock();
            pred->unlock();
            return;
        } else {
            curr->next->lock();
            pred->next = curr->next;
            pred->next->unlock();
            curr->unlock();
            pred->unlock();
          return;
        }
    }

    std::pair<bool, ElementType> find(KeyType key) {
        boost::shared_ptr<Node<ElementType> > pred = head;
        pred->lock();
        boost::shared_ptr<Node<ElementType> > curr = head->next;
        curr->lock();
        while (curr->key < key) {
            pred->unlock();
            pred = curr;
            curr = curr->next;
            curr->lock();
        }
        auto result = std::make_pair(curr->key == key, curr->element);
        curr->unlock();
        pred->unlock();
        return result;
    }

    ~LockableList() {   }
  private:
    boost::shared_ptr<Node<ElementType> > head;
};
