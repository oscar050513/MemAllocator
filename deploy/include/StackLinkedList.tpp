#include "StackLinkedList.h"

template <typename T>
void StackLinkedList<T>::push(Node* allocatedNode) {
  allocatedNode->next = d_topNode;
  d_topNode = allocatedNode;
}

template <typename T>
typename StackLinkedList<T>::Node* StackLinkedList<T>::pop() {
  if(d_topNode == nullptr)
  {
    return nullptr;
  }
 Node* ret = d_topNode;
 d_topNode = d_topNode->next; 
 ret->next = nullptr;
 return ret;
}

