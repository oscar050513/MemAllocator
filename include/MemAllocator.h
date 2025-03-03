#ifndef MEMALLOCATOR_H
#define MEMALLOCATOR_H
#include <cstddef>

class MemAllocator {
 public:
  MemAllocator(std::size_t capacity) : d_totalMem(capacity),d_usedMem(0),d_peakMem(0) {}
  virtual ~MemAllocator() { 
       d_totalMem = 0;
       d_usedMem = 0;
   }
  virtual void* alloc(const std::size_t size, const std::size_t alignment = 1) = 0;
  virtual void dealloc(void* ptr) = 0;
 protected:
  std::size_t d_totalMem;
  std::size_t d_usedMem;
  std::size_t d_peakMem;

};

#endif  /* MEMALLOCATOR_H */
