#include <gtest/gtest.h>
#include "rx/Subscription.hpp"

namespace {

TEST(Subscription, ReferenceEquality)
{
   auto s1 = Subscription();
   auto s2 = s1;

   ASSERT_EQ(s1, s2);

   std::list<Subscription> subscriptions;
   subscriptions.push_back(s1);
   auto it = std::find(std::begin(subscriptions), std::end(subscriptions), s1);

   ASSERT_FALSE(it == subscriptions.end());
}

}
