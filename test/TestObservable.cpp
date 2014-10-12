#include <gtest/gtest.h>
#include "rx/operators/Range.hpp"
#include "rx/Observable.hpp"
#include "rx/Subject.hpp"

#include <iostream>
#include <chrono>

namespace {

//! Subscribes to provided Observable and stores all
//! values that the Observable emits.
template<class T>
class Recorder
{
public:
   template<class U>
   static Recorder<U> create(Observable<U> o)
   {
      return Recorder<U>(o);
   }

   const std::vector<T>& toVector() const
   {
      return m_state->m_recording;
   }

   bool isCompleted() const
   {
      return m_state->m_isCompleted;
   }

   void unsubscribe()
   {
      m_state->m_compositeSubscription.unsubscribe();
   }

private:
   Recorder(Observable<T> o)
         : m_state(std::make_shared<State>())
   {
      auto shared_state = m_state;
      auto subscription = o.subscribe(Observer<T>(
         // onNext
         [shared_state](const T& t)
         {
            if (!shared_state->m_isCompleted)
            {
               shared_state->m_recording.push_back(t);
            }
         },
         // onCompleted
         [shared_state]()
         {
            shared_state->m_isCompleted = true;
         }));

      m_state->m_compositeSubscription.add(subscription);
   }

   void add(const T& t)
   {
      m_state->m_recording.push_back(t);
   }

   struct State
   {
      State()
            : m_isCompleted(false),
              m_recording()
      {
      }
      bool m_isCompleted;
      std::vector<T> m_recording;
      SubscriptionList m_compositeSubscription;
   };

   std::shared_ptr<State> m_state;
};

TEST(Observable, subscribe)
{
   // We'll use a subject to avoid boilerplate code
   auto s = Subject<int>::create();
   //auto s2 = s.passThrough();
   auto recorder = Recorder<int>::create(s);
   s.onNext(1);
   s.onNext(2);
   s.onNext(3);

   std::vector<int> expected
   { 1, 2, 3 };
   ASSERT_EQ(expected, recorder.toVector());
}

TEST(Observable, complete)
{
   auto s = Subject<int>::create();
   auto recorder = Recorder<int>::create(s);
   s.onNext(1);
   s.onNext(2);
   s.onNext(3);

   ASSERT_FALSE(recorder.isCompleted());

   s.onCompleted();

   ASSERT_TRUE(recorder.isCompleted());
}

TEST(Observable, completeUnsubscribesAutomatically)
{
   auto s = Subject<int>::create();
   auto recorder = Recorder<int>::create(s);
   s.onNext(1);
   s.onCompleted();
   s.onNext(2);

   std::vector<int> expected{ 1 };
   ASSERT_EQ(expected, recorder.toVector());
}

TEST(Observable, unsubscribe)
{
   auto s = Subject<int>::create();
   auto recorder = Recorder<int>::create(s);
   s.onNext(1);
   recorder.unsubscribe();
   s.onNext(2);

   std::vector<int> expected{ 1 };
   ASSERT_EQ(expected, recorder.toVector());
}

TEST(Observable, onError)
{
   auto s = Subject<int>::create();
   auto recorder = Recorder<int>::create(s);
   s.onNext(1);
   s.onError(std::make_exception_ptr(std::runtime_error("error")));
   s.onNext(2);

   std::vector<int> expected{ 1 };
   ASSERT_EQ(expected, recorder.toVector());
}

TEST(Observable, lift)
{
   // We'll use a subject to avoid boilerplate code
   auto s = Subject<int>::create();
   auto s2 = s.map([](int x){return x;});
   auto recorder = Recorder<int>::create(s2);
   s.onNext(1);
   s.onNext(2);
   s.onNext(3);

   std::vector<int> expected
   { 1, 2, 3 };
   ASSERT_EQ(expected, recorder.toVector());
}

// Performance measurements
TEST(Observable, subscribePerf)
{
   auto start = std::chrono::system_clock::now();
   auto CYCLE_COUNT = 1e6;

   range(1, CYCLE_COUNT).subscribe([](const int& x){
      //std::cout << x << std::endl;
   });

   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
       std::chrono::system_clock::now() - start);

   std::cout << "subscribePerf duration: " << duration.count() << " milliseconds" << std::endl;
}

TEST(Observable, subscribeString)
{
   // We'll use a subject to avoid boilerplate code
   auto s = Subject<std::string>::create();
   auto recorder = Recorder<std::string>::create(s);

   auto start = std::chrono::system_clock::now();
   auto CYCLE_COUNT = 1e6;

   for (int i = 0; i < CYCLE_COUNT; i++)
   {
     s.onNext("abcde");
   }

   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
       std::chrono::system_clock::now() - start);

   std::cout << "subscribePerf duration: " << duration.count() << " milliseconds" << std::endl;
}

TEST(range, rangeFromOneToTwo)
{
    auto observable = range(1,2);
    auto recorder = Recorder<int>::create(observable);

    std::vector<int> expected{ 1, 2 };
    ASSERT_EQ(expected, recorder.toVector());
}

TEST(Observable, map)
{
   auto observable = range(1,2)
         .map([](const int& x) {
            return x + 1;
         });

   auto recorder = Recorder<int>::create(observable);
   std::vector<int> expected{ 2, 3 };
   ASSERT_EQ(expected, recorder.toVector());
}

TEST(Observable, mapPerformance)
{
   auto start = std::chrono::system_clock::now();

   auto observable = range(1,1e6)
         .map([](const int& x) {
            return x + 1;
         })
         .map([](const int& x) {
            return x + 1;
         })
         .subscribe([](const int& x) {
            // do nothing
         });

   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
       std::chrono::system_clock::now() - start);

   std::cout << "map duration: " << duration.count() << " milliseconds" << std::endl;
}

TEST(Observable, mapPerformance2)
{
   auto start = std::chrono::system_clock::now();

   auto observable = range(1,1e6)
         .map([](const int& x) {
            return std::to_string(x);
         })
         .map([](const std::string& x) {
            return x;
         })
         .subscribe([](const std::string& x) {
            // do nothing
         });

   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
       std::chrono::system_clock::now() - start);

   std::cout << "map duration: " << duration.count() << " milliseconds" << std::endl;
}

TEST(Observable, mapPerformance3)
{
   auto start = std::chrono::system_clock::now();

   auto observable = range(1,1e6)
         .map([](const int& x) {
             auto length = x % 1000;
             return std::string(length, 'a');
         })
         .map([](const std::string& x) {
             return x;
         })
         .subscribe([](const std::string& x) {
             // do nothing
         });

   auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
       std::chrono::system_clock::now() - start);

   std::cout << "map duration: " << duration.count() << " milliseconds" << std::endl;
}

}
