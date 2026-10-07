cmake_minimum_required(VERSION 3.21)
project(EnderalBookMarkers VERSION 0.1.0 LANGUAGES CXX)

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

if(WIN32)
    find_package(CommonLibSSE CONFIG REQUIRED)
    add_commonlibsse_plugin(EnderalBookMarkers
        SOURCES
            src/Plugin.cpp
            src/BookContext.cpp
            src/BookMarkerState.cpp
    )
    target_compile_features(EnderalBookMarkers PRIVATE cxx_std_23)
    target_precompile_headers(EnderalBookMarkers PRIVATE PCH.h)
endif()
