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
 private:
  Node* d_topNode {nullptr};
};


#endif /* STACKLINKEDLIST_H  */
