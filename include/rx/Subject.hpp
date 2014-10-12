#pragma once

#include <memory>
#include <list>
#include <algorithm>

#include "rx/Observer.hpp"
#include "rx/Subscriber.hpp"

template<class T>
class SubjectSubscriptionManager
{
public:
   SubjectSubscriptionManager()
         : m_state(std::make_shared<State>())
   {

   }

   OnSubscribeFunc<T> createOnSubcribeFunc()
   {
      auto shared_state = m_state;
      return [shared_state](Subscriber<T> subscriber)
      {
         shared_state->m_subscribers.push_back(subscriber);

         subscriber.add(Subscription(
               [shared_state, subscriber]()
               {
                  shared_state->removeSubscriber(subscriber);
               }));
      };
   }

   std::function<void(const T&)> createOnNextFunc()
   {
      auto shared_state = m_state;
      return [shared_state](const T& t)
      {
         for(auto& s: shared_state->m_subscribers)
         {
            s.getObserver().onNext(t);
         }
      };
   }

   std::function<void()> createOnCompletedFunc()
   {
      auto shared_state = m_state;
      return [shared_state]()
      {
         for(auto& s: shared_state->m_subscribers)
         {
            s.getObserver().onCompleted();
         }
         shared_state->removeAllSubscribers();
      };
   }

   std::function<void(std::exception_ptr e)> createOnErrorFunc()
   {
      auto shared_state = m_state;
      return [shared_state](std::exception_ptr e)
      {
         for(auto& s: shared_state->m_subscribers)
         {
            s.getObserver().onError(e);
         }
         shared_state->removeAllSubscribers();
      };
   }
private:

   struct State
   {
      void removeSubscriber(const Subscriber<T>& subscriber)
      {
         auto it = std::find(std::begin(m_subscribers), std::end(m_subscribers),
               subscriber);

         if (it != m_subscribers.end())
         {
            m_subscribers.erase(it);
         }
      }

      void removeAllSubscribers()
      {
         m_subscribers.clear();
      }

      std::list<Subscriber<T>> m_subscribers;
   };

   std::shared_ptr<State> m_state;
};

template<class T>
class Subject: public Observable<T>, public Observer<T>
{
public:
   static Subject<T> create()
   {
      SubjectSubscriptionManager<T> subscriptionManager;

      auto onSubscribe = subscriptionManager.createOnSubcribeFunc();
      auto onNext = subscriptionManager.createOnNextFunc();
      auto onCompleted = subscriptionManager.createOnCompletedFunc();
      auto onError = subscriptionManager.createOnErrorFunc();

      return Subject(onSubscribe, onNext, onCompleted, onError);
   }

private:

   Subject(OnSubscribeFunc<T> onSubscribe, OnNext<T> onNext, OnCompleted onCompleted, OnError onError)
         : Observable<T>(onSubscribe),
           Observer<T>(onNext, onCompleted, onError)
   {

   }

   OnSubscribeFunc<T> m_onSubscribe;
};
