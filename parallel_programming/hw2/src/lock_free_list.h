#pragma once
#include "base.h"


template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:
    LockFreeList() {
        head = NULL;
    }

    void insert(ElementType& element, int key) {
        boost::mutex::scoped_lock lock(mutex);
        //std::cout << "insert called\n";
        if (head == NULL) {
            head = std::make_shared<Node<ElementType> >(element, key);
            return;
        }
        if (head->key > key) {
            head = std::make_shared<Node<ElementType> >(element, key, head);
            return;
        }
        auto list_iterator = head;
        for (; list_iterator != NULL; list_iterator = list_iterator->next) {
            if (list_iterator->key >= key) break;
        }
        if (list_iterator == NULL) return;
        if (list_iterator->key == key) {
            list_iterator->element = element;
            return;
        }
        list_iterator->next = std::make_shared<Node<ElementType> >(element, key, list_iterator->next);
    }


    void erase(int key) {
        boost::mutex::scoped_lock lock(mutex);
        //std::cout << "erase called\n";
        if (head == NULL) return;
        if (head->key > key) return;
        if (head->key == key) {
            head = head->next;
        }
        for (auto list_iterator = head; list_iterator->next != NULL; list_iterator = list_iterator->next) {
            if (list_iterator->next->key == key) {
                list_iterator->next = list_iterator->next->next;
            }
            if (list_iterator->next->key > key) break;
        }
    }

    std::pair<bool, ElementType> find(int key) {
        boost::mutex::scoped_lock lock(mutex);
        //std::cout << "find called\n";

        for (auto list_iterator = head; list_iterator != NULL; list_iterator = list_iterator->next) {
            if (list_iterator->key == key) return std::make_pair(true, list_iterator->element);
            if (list_iterator->key > key) break;
        }
        return std::make_pair(false, ElementType());
    }

    ~LockFreeList() {    }

    /*debug method*/
    void print(std::ostream& os) {
      os << "print list contents\n";
      for (auto list_iterator = head; list_iterator!= NULL; list_iterator = list_iterator->next) {
        os << list_iterator->key << " ";
      }
      os <<"end of contents\n";
    }
  private:
    std::shared_ptr<Node<ElementType > > head;
};
