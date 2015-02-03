#pragma once
#include "base.h"


template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:

    typedef MarkedAtomicNode<ElementType> MarkedAtomicNodeValue;
    typedef std::shared_ptr<MarkedAtomicNodeValue> MarkedAtomicNodePtr;

    LockFreeList() {
      //std::cout<<"in construct "<<std::numeric_limits<int>::max()<<"\n" ;
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
      std::cout <<"in insert\n";
        while (true) {
          std::cout << "1 " << key << "\n";
            auto neighbours = find_pair(key);
            std::cout << "2\n";
            auto pred = neighbours.first;
            auto curr = neighbours.second;
            if (curr->key() == key) {
                return;//do nothing - maybe should update value instead
            } else {
                //std::cout <<"insertion\n";
                MarkedAtomicNodePtr node = std::make_shared<MarkedAtomicNodeValue>(element, key, curr);
                /*std::cout <<"insertion2\n";
                if (pred == NULL)
                  std::cout<<"pred = null\n";
                if (pred->next() == NULL)
                  std::cout<<"pred->NEXT = null\n";
                  if (curr == NULL)
                    std::cout<<"curr = null\n";
                    if (node == NULL)
                      std::cout<<"node = null\n";*/
                if (pred->compareAndSet(curr, node)) return;
                std::cout << "4\n";
            }
        }

    }

    void erase(int key) {
        std::cout <<"in erase\n";
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
      std::cout << "in find\n";
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
      std::cout << "find_pair " << key << "\n";
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
