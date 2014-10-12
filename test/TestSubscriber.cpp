#include <gtest/gtest.h>
#include "rx/Subscriber.hpp"

namespace {


TEST(Subscriber, ReferenceEquality)
{
   auto s1 = Subscriber<int>([](const int& x){
      // do nothing
   });

   auto s2 = s1;

   ASSERT_EQ(s1, s1);

   std::list<Subscriber<int>> subscribers;
   subscribers.push_back(s1);
   auto it = std::find(std::begin(subscribers), std::end(subscribers), s1);

   ASSERT_FALSE(it == subscribers.end());
}

}
