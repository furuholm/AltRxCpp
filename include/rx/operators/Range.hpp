#pragma once

#include "rx/Observable.hpp"

static OnSubscribeFunc<int> onSubscribeRange(int start, int end)
{
   return [start, end](Subscriber<int> s){
      auto o = s.getObserver();
      for (int i=start; i <= end; i++)
      {
         o.onNext(i);
      }
      o.onCompleted();
   };
}

static Observable<int> range(int start, int stop)
{
   return Observable<int>::create(onSubscribeRange(start, stop));
}
