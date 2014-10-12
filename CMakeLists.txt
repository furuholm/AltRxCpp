project(AltRxCpp)
cmake_minimum_required(VERSION 2.8)

add_library({PROJECT_NAME} include/rx/Observable.hpp
                           include/rx/Observer.hpp
                           include/rx/Subject.hpp
                           include/rx/SafeSubscriber.hpp
                           include/rx/Subscriber.hpp
                           include/rx/Subscription.hpp
                           include/rx/operators/Map.hpp
                           include/rx/operators/Range.hpp
                           src/rx/Subscription.cpp)

include_directories(include)

add_definitions(-std=c++0x)

set(GMOCK_DIR "gmock-1.7.0"
    CACHE PATH "The path to the GoogleMock test framework.")

add_subdirectory(${GMOCK_DIR} ${CMAKE_BINARY_DIR}/gmock)

include_directories(SYSTEM ${GMOCK_DIR}/gtest/include
                           ${GMOCK_DIR}/include
                           include)

add_executable(RxTest test/main.cpp
                      test/TestObservable.cpp
                      test/TestSubscriber.cpp
                      test/TestSubscription.cpp)

target_link_libraries(RxTest gmock_main {PROJECT_NAME})

add_test(RxTest RxTest)

set(CMAKE_BUILD_TYPE Release)
