#pragma once

template<class ElementType>
class SyncList{
public:
  virtual void insert() {

  }
  virtual void erase() {

  }
  virtual void find() {

  }
  virtual ~SyncList() {}
};

template<class ElementType>
class LockableList: public SyncList<ElementType>{
  public:
    LockableList() {

    }
    void insert() {

    }
    void erase() {

    }
    void find() {

    }
    ~LockableList() {

    }
};

template<class ElementType>
class LockFreeList: public SyncList<ElementType> {
  public:
    LockFreeList() {

    }
    void insert() {

    }
    void erase() {

    }
    void find() {

    }

    ~LockFreeList() {

    }
};
