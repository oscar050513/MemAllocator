#ifndef STACKLINKEDLIST_H
#define STACKLINKEDLIST_H



template <typename T>
struct StackLinkedList {
 public:
  struct Node {
   T data;
   Node* next;
  };
  StackLinkedList() = default;
  ~StackLinkedList() = default;
 public: 
  void push(Node* allocatedNode);
  Node* pop();
  Node* getTop() const { return d_topNode;  }
 private:
  Node* d_topNode {nullptr};
};

#include "StackLinkedList.tpp"

#endif /* STACKLINKEDLIST_H  */
