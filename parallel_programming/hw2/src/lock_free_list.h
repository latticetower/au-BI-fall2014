#pragma once
#include "base.h"


template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:
    LockFreeList() {
        head = NULL;
    }

    void insert(ElementType& element, int key) {
        NodePtr previous = head;
        std::shared_ptr<NodeValue> prev_ptr = previous.load(std::memory_order_relaxed);
        NodePtr current = prev_ptr->next;
        for (; previous)
        try_insert(NodePtr previous, NodePtr current, ElementType& element, int key);
        //std::cout << "insert called\n";
        if (head == NULL || head->key > key) {
            head = std::make_shared<NodeValue>(std::make_pair(element, false), key, head);
            return;
        }
        auto list_iterator = head;
        for (; list_iterator != NULL; list_iterator = list_iterator->next) {
            if (list_iterator->key >= key) break;
        }
        if (list_iterator == NULL) return;
        if (list_iterator->key == key) {
            list_iterator->element = std::make_pair(element, false);
            return;
        }
        list_iterator->next = std::make_shared<NodeValue>(std::make_pair(element, false), key, list_iterator->next);
    }

    bool try_insert(NodePtr previous, NodePtr current, ElementType& element, int key) {
      //std::cout << "insert called\n";
      if (head == NULL || head->key > key) {
        head = std::make_shared<NodeValue>(std::make_pair(element, false), key, head);
        return;
      }
      auto list_iterator = head;
      for (; list_iterator != NULL; list_iterator = list_iterator->next) {
        if (list_iterator->key >= key) break;
      }
      if (list_iterator == NULL) return;
      if (list_iterator->key == key) {
        list_iterator->element = std::make_pair(element, false);
        return;
      }
      list_iterator->next = std::make_shared<NodeValue>(std::make_pair(element, false), key, list_iterator->next);
    }


    void erase(int key) {
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
        //std::cout << "find called\n";

        for (auto list_iterator = head; list_iterator != NULL; list_iterator = list_iterator->next) {
            if (list_iterator->key == key) return std::make_pair(true, list_iterator->element.first);
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
    std::pair<NodePtr, NodePtr> find(int key) {
        while(true) {
            NodePtr pred = head, curr = pred->next, succ;
            while (true) {
                auto succ = curr->next;
                bool marked = curr->element.second;
                if (marked) { // Если curr логически удален
                    if (!CAS(&pred->next, curr, std::make_shared<NodeValue>(std::make_pair(succ, false), key, list_iterator->next)))
                    continue;
                    curr = succ;
                } else {
                    if (curr.key >= key) return std::make_pair(pred, curr);
                    pred = curr;
                    curr = succ;
                }
            }
        }
    }
  private:
    typedef Node<std::pair<ElementType, bool> > NodeValue;
    typedef std::atomic<std::shared_ptr<NodeValue> > NodePtr;
    NodePtr head;
    //fst element is the same as in blocking class, second in deletion mark
};
