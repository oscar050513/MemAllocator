#ifndef FIXEDBLOCKALLOCATOR_H
#define FIXEDBLOCKALLOCATOR_H
#include <cstddef>
#include <mutex>
#include "MemAllocator.h"
#include "StackLinkedList.h"


class FixedBlockAllocator : public MemAllocator {
  private:
    struct BlockHeader {
    };

  public:
    using Node = typename StackLinkedList<BlockHeader>::Node;
    FixedBlockAllocator(std::size_t capacity, std::size_t chunk_size) noexcept;
    virtual ~FixedBlockAllocator() noexcept;
    virtual void* alloc(const std::size_t size=1, const std::size_t alignment = 1) override;
    virtual void dealloc(void* ptr) override;
    void printFreeBlocks() const;
  private:
    void init();
    StackLinkedList<BlockHeader> d_freeBlockList; 
    std::size_t d_chunk_size;         
    void* d_dataStartAddr{nullptr};
    mutable std::mutex d_mtx;
};

#endif  /* FIXEDBLOCKALLOCATOR_H */


