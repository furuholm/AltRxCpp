#include "rx/Subscription.hpp"

Subscription::Subscription()
   : m_state(nullptr)
{
}


Subscription::Subscription(Subscription::UnsubscribeFunc unsubscribe)
   : m_state(std::make_shared<State>(std::move(unsubscribe)))
{
}


void Subscription::unsubscribe() const
{
   if (m_state)
   {
      m_state->unsubscribe();
   }
}


Subscription::State::State()
   : m_unsubscribe(nullptr)
{
}


Subscription::State::State(Subscription::UnsubscribeFunc unsubscribe)
   : m_unsubscribe(std::move(unsubscribe))
{
}


void Subscription::State::unsubscribe()
{
   if (m_unsubscribe)
   {
      m_unsubscribe();
   }
}


Subscription::Subscription(std::unique_ptr<Subscription::State> state)
   : m_state(std::move(state))
{
}


SubscriptionList::SubscriptionList()
   : Subscription(std::unique_ptr<State>(new State()))
{
}

void SubscriptionList::add(Subscription s)
{
   auto derived_state =
         std::static_pointer_cast<State>(m_state);
   derived_state->add(std::move(s));
}

void SubscriptionList::unsubscribe() const
{
   auto derived_state =
         std::static_pointer_cast<State>(m_state);
   derived_state->unsubscribe();
}

void SubscriptionList::remove(Subscription s)
{
   auto derived_state =
         std::static_pointer_cast<State>(m_state);
   derived_state->remove(s);
}


void SubscriptionList::State::add(Subscription s)
{
   m_subscriptions.push_back(std::move(s));
}

void SubscriptionList::State::unsubscribe()
{
   for (auto& s : m_subscriptions)
   {
      s.unsubscribe();
   }
   m_subscriptions.clear();
}

void SubscriptionList::State::remove(Subscription s)
{
   auto it = std::find(std::begin(m_subscriptions),
                       std::end(m_subscriptions), s);

   if (it != m_subscriptions.end())
   {
      m_subscriptions.erase(it);
      s.unsubscribe();
   }
}


bool operator==(const Subscription &lhs, const Subscription &rhs)
{
   return lhs.m_state == rhs.m_state;
}


bool operator==(SubscriptionList &lhs, SubscriptionList &rhs)
{
   return lhs.m_state == rhs.m_state;
}
