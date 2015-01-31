#pragma once
template<class ElementType>
struct Node {
  Node(ElementType& element_, int key_, Node<ElementType> * next_ = NULL)
          : element(element_), key(key_), next(next_) {
  }

  ElementType element;
  int key;
  Node<ElementType>* next;

};

template<class ElementType>
class SyncList{
public:
  //method returns actual key
  virtual void insert(ElementType & element, int key) {

  }
  virtual void erase() {

  }
  std::pair<bool, ElementType&> find() {

  }
  virtual ~SyncList() {}
};

template<class ElementType>
class LockableList: public SyncList<ElementType>{
  public:
    LockableList() {
        head = NULL;
        tail = NULL;
    }
    
    void insert(ElementType& element, int key) {
        if (head == NULL) {
          head = new Node<ElementType>(element, key);
          tail = head;
          return;
        }
        if (head->key > key) {
          head = new Node<ElementType>(element, key, head);
          return;
        }
        auto list_iterator = head;
        for (; list_iterator != tail; list_iterator = list_iterator->next) {
          if (list_iterator->next->key > key)
            break;
        }
        if (list_iterator->key == key) {
          list_iterator->element = element;
          return;
        }
        list_iterator->next = new Node<ElementType>(element, key, list_iterator->next);
    }
    void erase(int key) {
      if (head->key == key) {
        auto temp = head;
        if (head == tail) {
          head = NULL;
          tail = NULL;
        } else {
          head = head->next;
        }
        delete temp;
      }
      for (auto list_iterator = head; list_iterator != tail; list_iterator = list_iterator->next) {
        if (list_iterator->next->key == key) {
          auto temp = list_iterator->next;
          list_iterator->next = temp->next;
          delete temp;
        }
        if (list_iterator->next->key > key)
          break;
      }
    }
    std::pair<bool, ElementType&> find(int key) {
      for (auto list_iterator = head; list_iterator != tail; list_iterator = list_iterator->next) {
        if (list_iterator->key == key)
          return std::make_pair(true, list_iterator->element);
        if (list_iterator->key > key)
          break;
      }
      return std::make_pair(false, ElementType());
    }
    ~LockableList() {

    }
  private:
    Node<ElementType> * head,
                      * tail;
};

template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:
    LockFreeList() {
      head = NULL;
      tail = NULL;
    }
    void insert(ElementType& element, int key) {
      if (head == NULL) {
        head = new Node<ElementType>(element, key);
        tail = head;
        return;
      }
      if (head->key > key) {
        head = new Node<ElementType>(element, key, head);
        return;
      }
      auto list_iterator = head;
      for ( ; list_iterator != tail; list_iterator = list_iterator->next) {
        if (list_iterator->next->key > key)
          break;
        }
        if (list_iterator->key == key) {
          list_iterator->element = element;
          return;
        }
        list_iterator->next = new Node<ElementType>(element, key, list_iterator->next);
      }
      void erase(int key) {
        if (head->key == key) {
          auto temp = head;
          if (head == tail) {
            head = NULL;
            tail = NULL;
          } else {
            head = head->next;
          }
          delete temp;
        }
        for (auto list_iterator = head; list_iterator != tail; list_iterator = list_iterator->next) {
          if (list_iterator->next->key == key) {
            auto temp = list_iterator->next;
            list_iterator->next = temp->next;
            delete temp;
          }
          if (list_iterator->next->key > key)
            break;
          }
        }
    std::pair<bool, ElementType&> find(int key) {
        for (auto list_iterator = head; list_iterator != tail; list_iterator = list_iterator->next) {
          if (list_iterator->key == key)
            return std::make_pair(true, list_iterator->element);
          if (list_iterator->key > key)
            break;
        }
        return std::make_pair(false, ElementType());
    }

    ~LockFreeList() {

    }
  private:
    Node<ElementType > * head, * tail;
};
