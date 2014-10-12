#pragma once

#include "rx/Subscriber.hpp"
#include <memory>

class OnErrorNotImplementedException : public std::runtime_error
{
public:
   OnErrorNotImplementedException(const std::string& what)
      : std::runtime_error(what)
   {
   }
};

template<class T>
struct SafeObserverState
{
   SafeObserverState(Observer<T> observer, Subscription subscription)
      : m_observer(std::move(observer))
      , m_subscription(std::move(subscription))
      , m_isFinished(false)
   {
   }

   Observer<T> m_observer;
   Subscription m_subscription;
   bool m_isFinished;
};

template<class T>
void onError(const std::shared_ptr<SafeObserverState<T>>& state, std::exception_ptr e)
{
   if (!state->m_isFinished)
   {
      state->m_isFinished = true; // TODO: Use CAS to make threadsafe

      try
      {
         state->m_observer.onError(e);
      }
      catch (const OnErrorNotImplementedException& ex)
      {
         state->m_subscription.unsubscribe();
         std::rethrow_exception(e);
      }

      state->m_subscription.unsubscribe();
   }
}

template<class T>
Observer<T> createSafeObserver(Subscriber<T> actual)
{
   auto state = std::make_shared<SafeObserverState<T>>(actual.getObserver(), actual.getSubscription());

   return Observer<T>(
      //onNext
      [state](const T& t) {
         try
         {
            if (!state->m_isFinished)
            {
               state->m_observer.onNext(t);
            }
         }
         catch (...)
         {
            onError(state, std::current_exception());
         }
      },
      //onCompleted
      [state]() {
         if (!state->m_isFinished)
         {
            state->m_isFinished = true; // TODO: Use CAS to make threadsafe

            try
            {
               state->m_observer.onCompleted();
            }
            catch (...)
            {
               auto e = std::current_exception();

               try
               {
                  state->m_observer.onError(e);
               }
               catch (const OnErrorNotImplementedException& ex)
               {
                  state->m_subscription.unsubscribe();
                  std::rethrow_exception(e);
               }
            }

            state->m_subscription.unsubscribe();
         }
      },
      //onError
      [state](std::exception_ptr e) {
         onError(state, std::move(e));
      });
}

template<class T>
Subscriber<T> createSafeSubscriber(const Subscriber<T>& actual)
{
   return Subscriber<T>(createSafeObserver(actual), actual.getSubscription());
}
