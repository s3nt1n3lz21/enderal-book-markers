cmake_minimum_required(VERSION 3.21)
project(EnderalBookMarkersTests LANGUAGES CXX)

include(CTest)

add_library(book_marker_state
    src/BookContext.cpp
    src/BookMarkerState.cpp
)
target_include_directories(book_marker_state PUBLIC src)
target_compile_features(book_marker_state PUBLIC cxx_std_20)

add_executable(book_marker_state_tests
    tests/BookMarkerStateTests.cpp
)
target_link_libraries(book_marker_state_tests PRIVATE book_marker_state)

if(BUILD_TESTING)
    add_test(NAME book_marker_state_tests COMMAND book_marker_state_tests)
endif()
