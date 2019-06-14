CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

set(THIS_TARGET mem_set)

include_directories(${MY_ROOT}/include)
aux_source_directory(. SRC_LIST)

link_directories(${PROTOBUF_LIB})
add_executable(${THIS_TARGET} ${SRC_LIST})
target_link_libraries(${THIS_TARGET} ${MY_LIB_DIR}/libpepper.a)
add_dependencies(${THIS_TARGET} pepper)
