#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H

template <typename T>
struct SinglyLinkedList {
  public:
   struct Node {
    T data;
    Node* next;
   };
  public:
   SinglyLinkedList() = default ;
   ~SinglyLinkedList() = default;
   void insert(Node* prev,Node* newNode);
   void remove(Node* prev,Node* toDeleteNode);
   Node* getHead() const { return head;  }
  private:
   Node* head {nullptr};
};

#include "SinglyLinkedList.tpp"

#endif /* SINGLYLINKEDLIST_H  */
