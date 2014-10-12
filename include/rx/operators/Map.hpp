#pragma once

#include "rx/Observer.hpp"

template<class T, class R, class Transformer>
Observer<T> createOperatorMap(Observer<R> o, Transformer transformer)
{
   return Observer<T>(
      // onNext
      [o, transformer](const T& t) {
         o.onNext(transformer(t));
      },
      // onCompleted
      [o]() {
         o.onCompleted();
      },
      // onError
      [o](std::exception_ptr e) {
         o.onError(e);
      });
}
