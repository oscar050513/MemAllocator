#include <iostream>
#include <cstddef>
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include "FixedBlockAllocator.h"

FixedBlockAllocator::FixedBlockAllocator(std::size_t capacity, std::size_t chunk_size) noexcept: MemAllocator(capacity)
{
  d_chunk_size = chunk_size;
  init();  
}

void FixedBlockAllocator::init()
{
  d_dataStartAddr = malloc(d_totalMem);
  void* curAddr = d_dataStartAddr; 
  assert("total capacity should be multiplier of chunk_size" && (d_totalMem%d_chunk_size == 0));
  assert("chunk_size should be bigger than header_size" && (d_chunk_size > sizeof(BlockHeader)));
  const int cnt = d_totalMem/d_chunk_size;
  for(int i = 0;i<cnt;++i)
  {
    Node* block = reinterpret_cast<Node*>(curAddr);
    block->next = nullptr;
    d_freeBlockList.push(block);
    curAddr = reinterpret_cast<void*>((std::size_t)curAddr + d_chunk_size);
  }
}

FixedBlockAllocator::~FixedBlockAllocator() noexcept
{
  free(d_dataStartAddr);
  d_dataStartAddr = nullptr;  
}

void* FixedBlockAllocator::alloc(const std::size_t size, const std::size_t alignment)
{
  std::lock_guard<std::mutex> lck(d_mtx);
  Node* newNode=d_freeBlockList.pop();
  assert("freeBlockList is empty, all memory has been used" && newNode != nullptr);
  d_usedMem += d_chunk_size;
  d_peakMem = std::max(d_peakMem,d_usedMem); 
  return reinterpret_cast<void*>(newNode);
}

void FixedBlockAllocator::dealloc(void* memAddr)
{
  std::lock_guard<std::mutex> lck(d_mtx);
  Node* allocNode = reinterpret_cast<Node*>(memAddr);
  d_usedMem -= d_chunk_size;
  allocNode->next = nullptr;
  d_freeBlockList.push(allocNode);
}

void FixedBlockAllocator::printFreeBlocks() const
{
  std::lock_guard<std::mutex> lck(d_mtx);
  auto header = d_freeBlockList.getTop();
  while(header!=nullptr)
  {
    std::cout<<header<<"->";
    header = header->next;
  }
  std::cout<<"END\n";
}

