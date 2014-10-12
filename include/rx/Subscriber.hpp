#pragma once

#include "rx/Observer.hpp"
#include "rx/Subscription.hpp"

template<class T>
class Subscriber
{
public:
   Subscriber(OnNext<T> onNext)
      : m_state(std::make_shared<State>(std::move(onNext)))
   {
   }

   Subscriber(Observer<T> destination)
      : m_state(std::make_shared<State>(std::move(destination)))
   {
   }

   Subscriber(Observer<T> destination, Subscription subscription)
      : m_state(std::make_shared<State>(std::move(destination), std::move(subscription)))
   {
   }

   Observer<T> getObserver() const
   {
       return m_state->m_destination;
   }

   Subscription getSubscription() const
   {
      return m_state->m_subscriptionList;
   }

   //! Used to register an unsubscribe callback.
   void add(Subscription s)
   {
      m_state->m_subscriptionList.add(s);
   }

protected:
   struct State {
      State(Observer<T> destination)
         : m_destination(std::move(destination))
      {
      }

      State(Observer<T> destination, Subscription subscription)
         : m_destination(std::move(destination))
      {
         m_subscriptionList.add(std::move(subscription));
      }

      Observer<T> m_destination;
      SubscriptionList m_subscriptionList;
   };

   Subscriber(std::unique_ptr<State> state)
         : m_state(std::move(state))
   {
   }

   std::shared_ptr<State>& state()
   {
      return m_state;
   }

private:
   std::shared_ptr<State> m_state;

   template<class R>
   friend bool operator==(const Subscriber<R>& lhs, const Subscriber<R>& rhs);
};

template<class R>
bool operator==(const Subscriber<R>& lhs, const Subscriber<R>& rhs)
{
   return lhs.m_state == rhs.m_state;
}
