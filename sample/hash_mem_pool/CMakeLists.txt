CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

include_directories(${MY_ROOT}/include)
aux_source_directory(. SRC_LIST)
add_executable(hash_mem_pool ${SRC_LIST})
