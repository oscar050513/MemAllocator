#include <memory>
#include <iostream>
#include <thread>
#include <vector>
#include <unistd.h>
#include <atomic>
#include "GeneralMemAllocatorLL.h"

std::atomic<int> counter {0};

static std::unique_ptr<GeneralMemAllocatorLL>& getAllocator()
{
  static std::unique_ptr<GeneralMemAllocatorLL> allocator(new GeneralMemAllocatorLL(2000));
  return allocator;
}

inline void printClientInfo()
{
  std::cout<<"pid : "<<getpid()<<" , tid : "<<std::this_thread::get_id()<<"\n";
}
void testAlloc(std::size_t execTimes)
{
   //printClientInfo();
   while(execTimes > 0)
   {
     //getAllocator()->printFreeBlocks();
     //std::cout<<"counter is : "<<counter.load()<<"\n";
     getAllocator()->alloc(10);
     counter.fetch_add(1);
     --execTimes;
   }
}



int main()
{
  //std::unique_ptr<GeneralMemAllocatorLL> a(new GeneralMemAllocatorLL(1000));
  //getAllocator()->printFreeBlocks();
  //getAllocator()->printFreeBlocks();
  const int n = 10;
  std::vector<std::thread> threads;
  for(int i=1;i<=n;++i)
  {
    threads.emplace_back(testAlloc,5);
  }
  for(auto& t : threads)
  {
    t.join();
  }
  //getAllocator()->printFreeBlocks();
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
  
  return 0;
}
