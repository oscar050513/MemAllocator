#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <atomic>
#include "GeneralMemAllocatorLL.h"
#include "FixedBlockAllocator.h" 

std::atomic<int> counter {0};

static std::unique_ptr<GeneralMemAllocatorLL>& getGeneralAllocator()
{
  static std::unique_ptr<GeneralMemAllocatorLL> allocator(new GeneralMemAllocatorLL(2000));
  return allocator;
}

static std::unique_ptr<FixedBlockAllocator>& getFixedAllocator()
{
  static std::unique_ptr<FixedBlockAllocator> allocator(new FixedBlockAllocator(2000,50));
  return allocator;
}

void testAlloc(std::size_t execTimes)
{
   //printClientInfo();
   while(execTimes > 0)
   {
     //getAllocator()->printFreeBlocks();
     //std::cout<<"counter is : "<<counter.load()<<"\n";
     getGeneralAllocator()->alloc(10);
     counter.fetch_add(1);
     --execTimes;
   }
}

void testFixedSizeMemAllocator() {
  auto& allocator = getFixedAllocator();
  auto* p1 = allocator->alloc();
  std::cout<<"first allocated block is : "<<p1<<"\n";
  allocator->printFreeBlocks();

  allocator->dealloc(p1);
  allocator->printFreeBlocks(); 
}


int main()
{

  const int n = 10;
  std::vector<std::thread> threads;
  for(int i=1;i<=n;++i)
  {
    threads.emplace_back(testFixedSizeMemAllocator);
  }
  for(auto& t : threads)
  {
    t.join();
  }
  /*
  void* p1 = a->alloc(10);
  a->printFreeBlocks();
  std::cout<<"allocated p1 is : "<<p1<<"\n";

  void* p2 = a->alloc(20);
  a->printFreeBlocks();
  std::cout<<"allocated p2 is : "<<p2<<"\n";

  //std::cout<<(std::size_t)p2 - (std::size_t)p1<<std::endl;
  a->dealloc(p1);
  a->printFreeBlocks();

  a->dealloc(p2);
  a->printFreeBlocks();
  */
  //testFixedSizeMemAllocator(); 
  return 0;
}
