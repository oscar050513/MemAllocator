#ifndef UTILS_H 
#define UTILS_H
#include <cstddef>
#include <thread>
#include <unistd.h>

namespace utils {
  std::size_t calculatePadding(std::size_t baseAddr, std::size_t alignment) {
    std::size_t padding = 0;
    if(alignment >= 1)
     {
       padding = (baseAddr/alignment + 1)*alignment - baseAddr;
     }
    return padding;
  }

  std::size_t calculatePaddingWithHeader(void* baseAddr, std::size_t alignment, std::size_t headerSize) {
    std::size_t baseAddrSize_t = (std::size_t)(baseAddr);
    std::size_t padding = calculatePadding(baseAddrSize_t,alignment);
    if(padding < headerSize)
    {
     std::size_t needSpace = headerSize - padding;
     if(needSpace%alignment==0)
     {
       padding += needSpace; 
     }
     else
     {
      padding += (needSpace/alignment + 1)*alignment;
     }
    }
    return padding;
  }

  inline void printClientInfo()
 {
  std::cout<<"pid : "<<getpid()<<" , tid : "<<std::this_thread::get_id()<<"\n";
 }

}

#endif  /* UTILS_H */
