Reactive Extensions is something I really want to have in my C++ toolbelt and fortunatley RxCpp exists. RxCpp has evolved greatly since the first version, but I still think there are some things that can be done in order to make the implementation simpler and cleaner. After studying the implementation of RxJava I decided to try to implement a subset of Reactive Extensions in C++ using RxJava as a guide. I have tried to stay as close to the RxJava implementation as possible while still trying to write idiomatic C++. The result is this prototype.

Micro benchmarks
So far this prototype only implements two operators: range and map. I used these two to create a couple of (micro) benchmarks to compare the performance of this prototype to RxCpp. The benchmarks were built in release mode. The sourcecode for the benchmarks is listed below. The only difference between the source code used to test my prototype and RxCpp is that the invocation of range looks like this in my prototype: `range(1,1e6)` and like this in RxCpp: `rxcpp::sources::range<int>(1,1e6)`

Benchmark 1:
The first benchmark lets a million ints through two map operations.

```
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
```

Benchmark 2:
The second benchmark is very similar but generates strings of various sizes instead.

```
auto start = std::chrono::system_clock::now();

auto observable = range(1,1e6)
      .map([](const int& x) {
          auto length = x % 1000;
          return std::string(lenght, 'a');
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
```

Results
|        |Benchmark 1|Benchmark 2|
|--------|-----------|-----------|
|RxCpp   |18-24ms    |420-440ms  |
|AltRxCpp|9-14ms     |250-270ms  |

Note that I have done very litle analysis of these results, so I don't think any other conclusion than that the prototype does not perform terrible. Keep in mind also that the prototype is not complete.

Finally: Remember that this is truly an experiment and does not live up to any standards when it comes to testing, documentation, error handling and so on...

I am @tobiasfuruholm on twitter If you have any comments or questions...