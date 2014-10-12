#pragma once

#include <functional>

#include "rx/Observer.hpp"
#include "rx/Subscription.hpp"
#include "rx/Subscriber.hpp"
#include "rx/SafeSubscriber.hpp"
#include "rx/operators/Map.hpp"
#include "rx/operators/Range.hpp"

template <class T>
using OnSubscribeFunc = std::function<void(Subscriber<T>)>;

template<class T>
class Observable
{
public:
   Subscription subscribe(Observer<T> observer)
   {
      auto subscriber = Subscriber<T>(observer);
      auto safeSubscriber = createSafeSubscriber(subscriber);
      m_state->onSubscribe(safeSubscriber);
      return safeSubscriber.getSubscription();
   }

   Subscription subscribe(OnNext<T> onNext)
   {
      auto observer = Observer<T>(std::move(onNext));
      return subscribe(observer);
   }

   static Observable create(OnSubscribeFunc<T> onSubscribe)
   {
      return Observable(std::move(onSubscribe));
   }

   template<class R>
   Observable<R> lift(std::function<Subscriber<T>(Subscriber<R>)> liftFunc)
   {
      auto shared_state = m_state;
      return Observable<R>::create([shared_state, liftFunc](Subscriber<R> o){
         shared_state->onSubscribe(liftFunc(o));
      });
   };

   template<class Callable>
   auto map(Callable transformer)
      -> Observable<typename std::result_of<Callable(T)>::type>
   {
      typedef typename std::result_of<Callable(T)>::type R;

      return lift<R>([transformer](Subscriber<R> subscriber){
         auto observer = subscriber.getObserver();

         return Subscriber<T>(
                  createOperatorMap<T, R, Callable>(observer, transformer));
      });
   }

protected:
   // Only use if you need to subclass Observable, otherwise use create
   Observable(OnSubscribeFunc<T> onSubscribeFunc)
         : m_state(std::make_shared<State>(std::move(onSubscribeFunc)))
   {
   }

private:
   class State
   {
   public:
      State(OnSubscribeFunc<T> onSubscribeFunc)
            : m_onSubscribeFunc(std::move(onSubscribeFunc))
      {
      }

      void onSubscribe(Subscriber<T> observer)
      {
         m_onSubscribeFunc(observer);
      }

   private:
      OnSubscribeFunc<T> m_onSubscribeFunc;
   };

   std::shared_ptr<State> m_state;
};
