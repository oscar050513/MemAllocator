#include <cstdlib>
#include <utility>
#include <cassert>
#include <iostream>
#include "GeneralMemAllocatorLL.h"
#include "utils.h"

GeneralMemAllocatorLL::GeneralMemAllocatorLL(std::size_t capacity, SelectPolicy policy) noexcept : MemAllocator(capacity) 
{
  d_policy = policy;
  init();
}

GeneralMemAllocatorLL::~GeneralMemAllocatorLL() noexcept
{
  free(d_dataStartAddr);
  d_dataStartAddr = nullptr;
}

void GeneralMemAllocatorLL::init()
{ 
  d_dataStartAddr = malloc(d_totalMem); 
  Node* head = (Node*)(d_dataStartAddr);
  head->data.blockSize = d_totalMem;
  head->next = nullptr;
  d_freeBlockList.insert(nullptr, head);   
}

void GeneralMemAllocatorLL::printFreeBlocks() const
{
  std::lock_guard<std::mutex> lck(d_mtx);
  Node* head = d_freeBlockList.getHead();
  while(head)
  {
    std::cout<<"FreeBlock startAddr: "<<head<<", blockSize is "<<head->data.blockSize<<"\n";
    head=head->next;
  }
}

std::pair<GeneralMemAllocatorLL::Node*,GeneralMemAllocatorLL::Node*> GeneralMemAllocatorLL::findBest(std::size_t size, std::size_t alignment,std::size_t& padding)
{
  Node* prev = nullptr;
  Node* cur = d_freeBlockList.getHead();
  Node* bestBlock = nullptr;
  Node* bestBlockPre = nullptr;
  std::size_t minDiff = ~0;
  while(cur)
  {
    std::size_t padding_tmp = utils::calculatePaddingWithHeader((void *)cur,alignment,sizeof(AllocatedHeader));
    //std::cout<<"calculated padding is : "<<padding_tmp<<std::endl;
    std::size_t totalReq = padding_tmp + size;
    if(cur->data.blockSize>=totalReq)
    {
      if(cur->data.blockSize-totalReq < minDiff)
      {
        bestBlock = cur;
        bestBlockPre = prev;
        minDiff = cur->data.blockSize-totalReq;
        padding = padding_tmp;
      }
    }
    prev = cur;
    cur = cur->next;
  }
 return std::make_pair(bestBlockPre,bestBlock);
}

std::pair<GeneralMemAllocatorLL::Node*,GeneralMemAllocatorLL::Node*> GeneralMemAllocatorLL::findFirst(std::size_t size, std::size_t alignment,std::size_t& padding)
{
  Node* prev = nullptr;
  Node* cur = d_freeBlockList.getHead();
  while(cur)
  {
    std::size_t padding_tmp = utils::calculatePaddingWithHeader((void *)cur,alignment,sizeof(AllocatedHeader));
    //std::cout<<"calculated padding is : "<<padding_tmp<<std::endl;
    std::size_t totalReq = padding_tmp + size;
    if(cur->data.blockSize>=totalReq)
    {
      padding = padding_tmp;
      break;
    }
    prev = cur;
    cur = cur->next;
  } 
  return std::make_pair(prev,cur);
}

std::pair<GeneralMemAllocatorLL::Node*,GeneralMemAllocatorLL::Node*> GeneralMemAllocatorLL::findAvailMem(std::size_t size, std::size_t alignment,std::size_t& padding)
{
  std::pair<Node*,Node*> ret;
  switch(d_policy) {
  case SelectPolicy::FIRST_FIT:
   ret = std::move(findFirst(size,alignment,padding)); 
   break;
  case SelectPolicy::BEST_FIT:
   ret = std::move(findBest(size,alignment,padding));
   break;
  default:
   assert("INVALID MEM SEARCH POLICY SPECIFIED" && 0 );
   break; 
 }
 return ret;
}


void* GeneralMemAllocatorLL::alloc(std::size_t size,std::size_t alignment)
{
   std::lock_guard<std::mutex> lck(d_mtx);
   utils::printClientInfo();
   std::cout<<"allocated mem is :"<<d_usedMem<<"\n";
   std::size_t padding;
   auto ret = findAvailMem(size,alignment,padding);
   Node* prev = ret.first;
   Node* cur = ret.second;
   assert("Fail to allocate memory" && cur!=nullptr);
   std::size_t rest = cur->data.blockSize - size - padding;
   //create freenode to hold remaining memory from the matched node
   Node* newNode {nullptr};
   if(rest>0)
   {
     newNode = reinterpret_cast<Node*>(reinterpret_cast<std::size_t>(cur) + size + padding);
     newNode->data.blockSize = rest;
     newNode->next = nullptr;
     d_freeBlockList.insert(cur,newNode);     
  } 
  // remove the match freenode from list and setup allocation header
  d_freeBlockList.remove(prev,cur);
  void* dataAddr = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(cur) + padding);
  AllocatedHeader* allocHeader = reinterpret_cast<AllocatedHeader*>(reinterpret_cast<std::size_t>(dataAddr) - sizeof(AllocatedHeader));
  allocHeader->dataBlockSize = size;
  allocHeader->padding = padding - sizeof(AllocatedHeader);
  d_usedMem += size + padding;
  if(d_usedMem > d_peakMem)
  {
    d_peakMem = d_usedMem;
  }
  return dataAddr;
}

void GeneralMemAllocatorLL::merge(Node* prev,Node* cur)
{
  if(cur->next != nullptr && reinterpret_cast<std::size_t>(cur) + cur->data.blockSize == reinterpret_cast<std::size_t>(cur->next))
  {
    cur->data.blockSize+=cur->next->data.blockSize;
    d_freeBlockList.remove(cur,cur->next);
  }
  if(prev != nullptr && reinterpret_cast<std::size_t>(prev) + prev->data.blockSize == reinterpret_cast<std::size_t>(cur))
  {
    prev->data.blockSize+=cur->data.blockSize;
    d_freeBlockList.remove(prev,cur);
  }
}

void GeneralMemAllocatorLL::dealloc(void* dataAddr)
{
  std::lock_guard<std::mutex> lck(d_mtx);
  AllocatedHeader* allocHeader = reinterpret_cast<AllocatedHeader*>(reinterpret_cast<std::size_t>(dataAddr) - sizeof(AllocatedHeader));
  std::size_t totalSize = allocHeader->dataBlockSize + allocHeader->padding + sizeof(AllocatedHeader);
  void* blockStart = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(dataAddr) - allocHeader->padding - sizeof(AllocatedHeader));
  Node* newNode = reinterpret_cast<Node*>(blockStart);
  newNode->data.blockSize = totalSize;
  newNode->next=nullptr;
  Node* prev = nullptr;
  Node* cur = d_freeBlockList.getHead();
  while(cur)
  {
    if(reinterpret_cast<std::size_t>(blockStart) < reinterpret_cast<std::size_t>(cur))
    {
      break;
    }
   prev = cur;
   cur=cur->next;
  } 
  d_freeBlockList.insert(prev, newNode);
  this->merge(prev,newNode);
  d_usedMem -= totalSize;
}


