cmake_minimum_required(VERSION 3.10)
project(lab4)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party/googletest)
enable_testing()

add_executable(lab4 src/main.cpp
        include/Base/DynamicArray.h
        include/Base/LinkedList.h
        include/Sequences/Sequence.h
        include/Sequences/ArraySequence.h
        include/Sequences/ListSequence.h
        include/Specialized/BitSequence.h
        include/Common/Option.h
        include/Common/IEnumerator.h
        cli/interface.h
        include/Common/Pair.h
        include/Sequences/SequenceFunctions.h
        include/Sequences/ArraySequenceBuilder.h
        include/Specialized/Queue.h
        include/Specialized/Stack.h
        include/Specialized/Vector.h
        include/Specialized/Deque.h
        include/Sequences/Algorithms.h
        include/Sequences/SequenceEnumerator.h
        include/LazySequence/IGenerator.h
        include/LazySequence/LazySequence.h
        include/Stream/RWStreams.h
        include/Stream/BufferReadOnlyStream.h
        include/Stream/FilteredGenerator.h
)

include_directories(third_party/googletest/include)

add_executable(tests
        tests/test_dynamic_array.cpp
        tests/test_linked_list.cpp
        tests/test_sequence.cpp
        tests/test_queue.cpp
        tests/test_deque.cpp
        tests/test_stack.cpp
        tests/test_vector.cpp
)

target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(tests gtest gtest_main)

add_test(NAME Lab4Tests COMMAND tests)

target_include_directories(lab4 PUBLIC
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/Base
        ${CMAKE_SOURCE_DIR}/include/Common
        ${CMAKE_SOURCE_DIR}/include/Sequences
        ${CMAKE_SOURCE_DIR}/include/Specialized
)