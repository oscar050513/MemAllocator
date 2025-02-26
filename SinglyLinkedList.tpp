#include "SinglyLinkedList.h"

template <typename T>
void SinglyLinkedList<T>::insert(Node* prev, Node* newNode) {
if(prev == nullptr)
{
 newNode->next = head;
 head = newNode;  
}
else
{
  Node* tmp = prev->next;
  prev->next = newNode;
  newNode->next = tmp;
}
}

template <typename T>
void SinglyLinkedList<T>::remove(Node* prev, Node* toDeleteNode) {
if(prev==nullptr)
{
  head = toDeleteNode->next;
}
else
{
  prev->next = toDeleteNode->next;
  toDeleteNode->next = nullptr;
}
}


