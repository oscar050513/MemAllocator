#ifndef GENERALMEMALLOCATORLL_H
#define GENERALMEMALLOCATORLL_H
#include <cstdint>
#include <mutex>
#include <atomic>
#include "MemAllocator.h"
#include "SinglyLinkedList.h"

class GeneralMemAllocatorLL : public MemAllocator {
 private:
  struct FreeHeader {
   std::size_t blockSize;
  };
  struct AllocatedHeader {
   std::size_t dataBlockSize;
   std::size_t padding;
  };

 public:
  enum class SelectPolicy : uint8_t {
     FIRST_FIT = 0,
     BEST_FIT
  };
  using Node = typename SinglyLinkedList<FreeHeader>::Node;
  GeneralMemAllocatorLL(std::size_t capacity, SelectPolicy policy = SelectPolicy::FIRST_FIT) noexcept;
  virtual ~GeneralMemAllocatorLL() noexcept; 
  virtual void* alloc(const std::size_t size, const std::size_t alignment = 1) override;
  virtual void dealloc(void* ptr) override;
  void printFreeBlocks() const;
 private:
  void merge(Node* pre,Node* cur);
  void init();
  std::pair<Node*,Node*> findBest(std::size_t size, std::size_t alignment,std::size_t& padding); 
  std::pair<Node*,Node*> findFirst(std::size_t size, std::size_t alignment,std::size_t& padding);  
  std::pair<Node*,Node*> findAvailMem(std::size_t size, std::size_t alignment,std::size_t& padding); 
 private:
  SinglyLinkedList<FreeHeader> d_freeBlockList;  
  SelectPolicy d_policy;
  void* d_dataStartAddr{nullptr};
  mutable std::mutex d_mtx;
};

#endif  /* GENERALMEMALLOCATORLL_H */
