include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(generalAllocator STATIC GeneralMemAllocatorLL.cc)
add_library(fixedAllocator STATIC FixedBlockAllocator.cc)

add_executable(testAllocator main.cc)

target_link_libraries(testAllocator generalAllocator fixedAllocator)
