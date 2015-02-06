#pragma once
#include "base.h"


template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:

    typedef MarkedAtomicNode<ElementType> MarkedAtomicNodeValue;
    typedef boost::shared_ptr<MarkedAtomicNodeValue> MarkedAtomicNodePtr;

    LockFreeList() {
        tail = boost::shared_ptr<MarkedAtomicNode<ElementType> >(new MarkedAtomicNodeValue(ElementType(), std::numeric_limits<KeyType>::max()));
        head = boost::shared_ptr<MarkedAtomicNode<ElementType> >(new MarkedAtomicNodeValue(ElementType(), std::numeric_limits<KeyType>::min(), tail));
    }

    void insert(ElementType & element, KeyType key) {
        //std::cout << "in insert\n";
        while (true) {
            //std::cout << "before find\n";
            auto neighbours = find_pair(key);
            //std::cout << "after find\n";
            auto pred = neighbours.first;
            auto curr = neighbours.second;
            if (curr->get_key() == key) {
                return;
                //do nothing
            } else {
                MarkedAtomicNodePtr node = boost::shared_ptr<MarkedAtomicNode<ElementType> >(new MarkedAtomicNodeValue(element, key, curr));
                if (pred->compareAndSet(curr, node)) return;
            }
        }

    }


    void erase(KeyType key) {
        //std::cout << "in erase\n";

        while (true) {
            auto neighbours = find_pair(key);
            auto pred = neighbours.first;
            auto curr = neighbours.second;
            if (curr->get_key() >= key) {
              return;
            } else {
                MarkedAtomicNodePtr succ = curr->next();
                if (curr->tryToUpdateData(succ, true)){
                    //if (!curr->compareAndSet(succ, succ, false, true)) continue;
                    pred->compareAndSet(curr, succ);
                    return;
                }
            }
        }
    }

    std::pair<bool, ElementType> find(KeyType key) {
        //std::cout <<"find\n";
        auto w = find_pair(key);
        return std::make_pair(w.second->get_key()- key == 0, w.second->get_element());
    }


    ~LockFreeList() {    }

  private:

    std::pair<MarkedAtomicNodePtr, MarkedAtomicNodePtr > find_pair(KeyType key) {
        //std::cout <<"in find_pair: key= " << key << "\n";
        while(true) {
            MarkedAtomicNodePtr pred = head;
            MarkedAtomicNodePtr curr = head->next();
            MarkedAtomicNodePtr succ;
            bool marked = false;
            while (true) {
                succ = curr->next();
                marked = curr->marked();
                if (marked) {
                    if (!pred->compareAndSet(curr, succ)) break;
                    curr = succ;
                } else {
                    if (curr->get_key()-key >= 0) {
                        return std::make_pair(pred, curr);
                    }
                    pred = curr;
                    curr = succ;
                }
            }
        }
    }


    MarkedAtomicNodePtr head, tail;
};
