#pragma once
#include "base.h"


template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:

    typedef MarkedAtomicNode<ElementType> MarkedAtomicNodeValue;
    typedef boost::atomic<std::shared_ptr<MarkedAtomicNodeValue> > MarkedAtomicNodePtr;

    LockFreeList() {
        tail = std::make_shared<MarkedAtomicNodeValue>(
              ElementType(),
              std::numeric_limits<int>::max()
            );
        head = std::make_shared<MarkedAtomicNodeValue>(
              ElementType(),
              std::numeric_limits<int>::min(), tail.load()
              );
    }

    void insert(ElementType& element, int key) {
        while (true) {
            auto neighbours = find_pair(key);
            MarkedAtomicNodePtr& pred = *neighbours.first;
            MarkedAtomicNodePtr& curr = *neighbours.second;
            if (get_key(curr) == key) {
                return;//do nothing - maybe should update value instead
            } else {
                MarkedAtomicNodePtr node(std::make_shared<MarkedAtomicNodeValue>(element, key, curr.load()));
                if (compareAndSet(get_ptr(pred)->next, curr, node)) return;
            }
        }
    }

    void erase(int key) {
        while (true) {
            auto neighbours = find_pair(key);
            MarkedAtomicNodePtr& pred = *neighbours.first;
            MarkedAtomicNodePtr& curr = *neighbours.second;
            if (get_key(curr) != key) {
              return; //false;
            } else {
                MarkedAtomicNodePtr& succ = get_ptr(curr)->next;
                if (!compareAndSet(curr, succ, succ, false, true)) continue;// retry;
                compareAndSet(pred, curr, succ);
                return ;//true;
            }
        }
    }


    std::pair<bool, ElementType> find(int key) {
        auto w = find_pair(key);
        return std::make_pair(get_key(*w.second) == key, get_ptr(*w.second)->element);
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
    std::pair<MarkedAtomicNodePtr*,
              MarkedAtomicNodePtr* > find_pair(int key) {
      //bool retry;
      while(true) {
        //retry = false;
        MarkedAtomicNodePtr* pred = &head;
        MarkedAtomicNodePtr* curr = &get_ptr(*pred)->next;
        MarkedAtomicNodePtr* succ;
        bool marked = false;
        while (true) {
          load_value(*curr, *succ, marked);
          if (marked) { // Если curr логически удален
              if (!compareAndSet(*pred, *curr, *succ))
                  break;
              curr = succ;
          } else {
            if (get_key(*curr) >= key)
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
