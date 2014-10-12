#pragma once

#include <functional>
#include <memory>
#include <algorithm>
#include <list>

class Subscription
{
   typedef std::function<void()> UnsubscribeFunc;
public:
   Subscription();

   Subscription(UnsubscribeFunc unsubscribe);

   virtual void unsubscribe() const;

protected:

   class State
   {
   public:
      State();

      State(UnsubscribeFunc unsubscribe);

      virtual ~State() = default;

      virtual void unsubscribe();

   private:
      UnsubscribeFunc m_unsubscribe;
   };

   Subscription(std::unique_ptr<State> state);

   friend bool operator==(const Subscription& lhs, const Subscription& rhs);

protected:
   mutable std::shared_ptr<State> m_state;
};

bool operator==(const Subscription& lhs, const Subscription& rhs);


class SubscriptionList : public Subscription
{
public:
   SubscriptionList();

   void add(Subscription s);

   void unsubscribe() const override;

   void remove(Subscription s);

private:
   class State : public Subscription::State
   {
   public:
      void add(Subscription s);

      void unsubscribe() override;

      void remove(Subscription s);

   private:
      std::list<Subscription> m_subscriptions;
   };

   friend bool operator==(SubscriptionList& lhs, SubscriptionList& rhs);
};

bool operator==(SubscriptionList& lhs, SubscriptionList& rhs);
