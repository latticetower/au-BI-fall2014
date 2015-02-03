#pragma once
#include "base.h"


template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:

    typedef MarkedAtomicNode<ElementType> MarkedAtomicNodeValue;
    typedef std::shared_ptr<MarkedAtomicNodeValue> MarkedAtomicNodePtr;

    LockFreeList() {
        tail = std::make_shared<MarkedAtomicNodeValue>(
              ElementType(),
              std::numeric_limits<int>::max()
            );
        head = std::make_shared<MarkedAtomicNodeValue>(
              ElementType(),
              std::numeric_limits<int>::min(), tail
              );
    }

    void insert(ElementType& element, int key) {

        while (true) {
            auto neighbours = find_pair(key);
            auto pred = neighbours.first;
            auto curr = neighbours.second;
            if (curr->key() == key) {
                return;//do nothing - maybe should update value instead
            } else {
                MarkedAtomicNodePtr node(std::make_shared<MarkedAtomicNodeValue>(element, key, curr.load()));
                if (pred->next()->compareAndSet(curr, node)) return;
            }
        }

    }

    void erase(int key) {
        while (true) {
            auto neighbours = find_pair(key);
            auto pred = neighbours.first;
            auto curr = neighbours.second;
            if (curr->key() != key) {
              return; //false;
            } else {
                MarkedAtomicNodePtr succ = curr->next();
                if (!curr->compareAndSet(succ, succ, false, true)) continue;// retry;
                pred->compareAndSet(curr, succ);
                return ;//true;
            }
        }
    }


    std::pair<bool, ElementType> find(int key) {
        auto w = find_pair(key);
        return std::make_pair(w.second->key() == key, w.second->element());
    }

    ~LockFreeList() {    }

    /*debug method*/
    void print(std::ostream& os) {/*
      os << "print list contents\n";
      for (auto list_iterator = head; list_iterator!= NULL; list_iterator = list_iterator->next) {
      os << list_iterator->key << " ";
    }
    os <<"end of contents\n";*/
    }

  private:
    std::pair<MarkedAtomicNodePtr,
              MarkedAtomicNodePtr > find_pair(int key) {
      //bool retry;
      while(true) {
        //retry = false;
        MarkedAtomicNodePtr pred = head;
        MarkedAtomicNodePtr curr = pred->next();
        MarkedAtomicNodePtr succ;
        bool marked = false;
        while (true) {
          succ = curr->next();
          marked = curr->marked();
          if (marked) { // Если curr логически удален
              if (!pred->compareAndSet(curr, succ))
                  break;
              curr = succ;
          } else {
            if (curr->key() >= key)
                return std::make_pair(pred, curr);
            pred = curr;
            curr = succ;
          }
        }
      }
    }

  private:
    MarkedAtomicNodePtr head, tail;
};
