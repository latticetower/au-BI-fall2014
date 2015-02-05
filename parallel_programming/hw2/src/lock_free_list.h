#pragma once
#include "base.h"


template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:

    typedef MarkedAtomicNode<ElementType> MarkedAtomicNodeValue;
    typedef boost::shared_ptr<MarkedAtomicNodeValue> MarkedAtomicNodePtr;

    LockFreeList() {
      //std::cout<<"in construct "<<std::numeric_limits<int>::max()<<"\n" ;
        tail = boost::shared_ptr<MarkedAtomicNodeValue>(new MarkedAtomicNodeValue(ElementType(),
            std::numeric_limits<KeyType>::max()));
        head = boost::shared_ptr<MarkedAtomicNodeValue>(new MarkedAtomicNodeValue(ElementType(),
            std::numeric_limits<KeyType>::min(), tail));

        //std::cout << tail<<" "<< tail->get_key() <<" "<< head->next()->get_key() <<" <- key in hext\n";
    }

    void insert(ElementType& element, KeyType key) {
        //std::cout <<"in insert\n";
        while (true) {
            //std::cout << "insert1: key=" << key << "\n";
            auto neighbours = find_pair(key);
            //std::cout << "2\n";
            auto pred = neighbours.first;
            auto curr = neighbours.second;
            //if(pred==curr)
              //std::cout<<"pred==curr\n";
            if (curr->get_key() == key) {
                return;//do nothing - maybe should update value instead
            } else {
                //std::cout <<"insertion\n";
                MarkedAtomicNodePtr node = boost::shared_ptr<MarkedAtomicNodeValue>(
                    new MarkedAtomicNodeValue(element, key, curr));
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
                //std::cout << "4\n";
            }
        }

    }

    void erase(KeyType key) {
        //std::cout <<"in erase\n";
        while (true) {
            auto neighbours = find_pair(key);
            auto pred = neighbours.first;
            auto curr = neighbours.second;
            if (curr->get_key() != key) {
              return; //false;
            } else {
                MarkedAtomicNodePtr succ = curr->next();
                if (!curr->compareAndSet(succ, succ, false, true)) continue;// retry;
                pred->compareAndSet(curr, succ);
                return ;//true;
            }
        }
    }


    std::pair<bool, ElementType> find(KeyType key) {
        //std::cout << "in find\n";
        auto w = find_pair(key);
        //std::cout << "find 2\n";
        return std::make_pair(w.second->get_key() == key, w.second->element());
        //return std::make_pair(false, ElementType());
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
              MarkedAtomicNodePtr > find_pair(KeyType key) {
      //std::cout << "find_pair " << key << "\n";
      //bool retry;
      while(true) {
        //retry = false;
        MarkedAtomicNodePtr& pred = head;
        //std::cout << (KeyType)head->get_key() << " " << tail->get_key() <<" -> data in find_pair\n";
        //auto head_data = head->get();
        ;
        //std::cout << head_data->key << " head->next()\n";
        MarkedAtomicNodePtr curr = head->next();
        MarkedAtomicNodePtr succ;
        bool marked = false;
        while (true) {
          //std::cout<<"find pair: inner cycle\n";
          succ = curr->next();
          marked = curr->marked();

          if (marked) { // Если curr логически удален
            //std::cout << "find_pair: marked "<<(curr == NULL)<<"\n";

              if (!pred->compareAndSet(curr, succ))
                  break;
              curr = succ;
          } else {
            //std::cout << "find_pair: non marked "<<
            //(curr == NULL) << " "<< pred << " "<<curr << " "<< curr->get_key()<<"\n";
            //std::cout << "in find_pair : " << curr->get_key() << " "<< key<<" " << (curr->get_key() >= key) << "\n";
            if (curr->get_key() >= key){//std::cout<<"returning find_pair\n";
                return std::make_pair(pred, curr);
            }
            //std::cout << "find_pair: non marked 2 "<<(curr == NULL)<<"\n";
            //std::cout << "find_pair : next \n";
            pred = curr;
            curr = succ;
          }
        }
      }
    }

  private:
    MarkedAtomicNodePtr head, tail;
};
