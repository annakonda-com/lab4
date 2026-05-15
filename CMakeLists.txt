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
        include/Common/Option.h
        include/Common/IEnumerator.h
        cli/interface.h
        include/Common/Pair.h
        include/Sequences/SequenceFunctions.h
        include/Sequences/ArraySequenceBuilder.h
        include/Sequences/Algorithms.h
        include/Sequences/SequenceEnumerator.h
        include/LazySequence/LazySequence.h
        include/LazySequence/Cardinal.h
        include/LazySequence/Generators.h
        include/Stream/ReadOnlyStream.h
        include/Stream/WriteOnlyStream.h
        include/Base/BinarySearchTree.h
        include/Stream/Specialized/IndexBuilder.h
        include/Stream/Specialized/FileReadOnlyStream.h
        include/Stream/Specialized/SequenceReadOnlyStream.h
        include/LazySequence/TuringMachine.h
)

include_directories(third_party/googletest/include)

add_executable(tests
        tests/test_dynamic_array.cpp
        tests/test_linked_list.cpp
        tests/test_sequence.cpp
        tests/test_lazy_sequence.cpp
        tests/test_streams.cpp
        tests/test_turing_machine.cpp
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