#pragma once

#include <memory>

template<class T>
using OnNext = std::function<void(const T&)>;

typedef std::function<void()> OnCompleted;

typedef std::function<void(std::exception_ptr e)> OnError;

template<class T>
class Observer {
public:
   Observer()
         : m_state(std::make_shared<State>(nullptr, nullptr, nullptr))
   {
   }

   Observer(OnNext<T> onNext, OnCompleted onCompleted = nullptr, OnError onError = nullptr)
         : m_state(std::make_shared<State>(std::move(onNext), std::move(onCompleted), std::move(onError)))
   {
   }

   void onNext(const T& t) const
   {
      if (m_state->m_onNext)
      {
         m_state->m_onNext(t);
      }
   }

   void onCompleted() const
   {
      if (m_state->m_onCompleted)
      {
         m_state->m_onCompleted();
      }
   }

   void onError(std::exception_ptr e) const
   {
      if (m_state->m_onError)
      {
         m_state->m_onError(e);
      }
   }

private:
   struct State
   {
      State(OnNext<T> onNext = nullptr, OnCompleted onCompleted = nullptr, OnError onError = nullptr)
            : m_onNext(std::move(onNext)),
              m_onCompleted(std::move(onCompleted)),
              m_onError(std::move(onError))
      {
      }

      OnNext<T> m_onNext;
      OnCompleted m_onCompleted;
      OnError m_onError;
   };

   std::shared_ptr<State> m_state;
};

