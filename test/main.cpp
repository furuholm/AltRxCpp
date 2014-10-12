
#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "rx/operators/Range.hpp"
#include "rx/Observable.hpp"
#include "rx/Subject.hpp"


int main(int argc, char** argv) {
   // The following line must be executed to initialize Google Mock
   // (and Google Test) before running the tests.
   ::testing::InitGoogleMock(&argc, argv);
   return RUN_ALL_TESTS();
}
