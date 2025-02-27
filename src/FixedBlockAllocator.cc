#include <cstddef>
#include <cstdlib>
#include <cassert>
#include "FixedBlockAllocator.h"

FixedBlockAllocator::FixedBlockAllocator(std::size_t capacity, std::size_t chunk_size) : MemAllocator(capacity)
{
  d_chunk_size = chunk_size;
  init();  
}

void FixedBlockAllocator::init()
{
  d_dataStartAddr = malloc(d_totalMem);
  void* curAddr = d_dataStartAddr; 
  assert("total capacity should be multiplier of chunk_size" && (capacity%chunk_size == 0));
  assert("chunk_size should be bigger than header_size" && (d_chunk_size > sizeof(BlockHeader)));
  const int cnt = capacity/chunk_size;
  for(int i = 0;i<cnt;++i)
  {
    Node* block = reinterpret_cast<Node*>(curAddr);
    block->data.blockSize = d_chunk_size;
    block->next = nullptr;
    d_freeBlockList.push(block);
    curAddr = reinterpret_cast<void*>((std::size_t)curAddr + d_chunk_size);
  }
}

FixedBlockAllocator::~FixedBlockAllocator()
{
  free(d_dataStartAddr);
  d_dataStartAddr = nullptr;  
}

void* FixedBlockAllocator::alloc()
{
  Node* newNode=d_freeBlockList.pop();
  void* dataAddr = reinterpret_cast<void*>(reinterpret_cast<std::size_t>(newNode) + sizeof(BlockHeader));
  return dataAddr;     
}

void FixedBlockAllocator::dealloc(void* memAddr)
{
  Node* allocNode = reinterpret_cast<Node*>(reinterpret_cast<std::size_t>(memAddr) - sizeof(BlockHeader));
  allocNode->data.blockSize = d_chunk_size;
  allocNode->next = nullptr;
  d_freeBlockList.push(allocNode);
}

