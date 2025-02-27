#ifndef FIXEDBLOCKALLOCATOR_H
#define FIXEDBLOCKALLOCATOR_H
#define <cstddef>
#include "MemAllocator.h"

class FixedBlockAllocator : public MemAllocator {
  private:
    struct BlockHeader {
      std::size_t blockSize;
    };

  public:
    FixedBlockAllocator(std::size_t capacity, std::size_t chunk_size) noexcept;
    virtual ~FixedBlockAllocator() noexcept;
    virtual void* alloc() override;
    virtual void dealloc(void* ptr) override;
    void printFreeBlocks() const;
  private:
    void init();
    StackLinkedList<BlockHeader> d_freeBlockList; 
    std::size_t d_chunk_size;         
    void* d_dataStartAddr{nullptr};
};

#endif  /* FIXEDBLOCKALLOCATOR_H */


