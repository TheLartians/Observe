[![Build Status](https://travis-ci.com/TheLartians/Event.svg?branch=master)](https://travis-ci.com/TheLartians/Event)

# lars::Event

A thread-safe C++11 event-listener template and observable value implementation.

# Example

## lars::Event

```c++
lars::Event<float,float> onClick;
auto observer = onClick.createObserver([](auto x, auto y){ handleClick(x,y); });
onClick.emit(0,0); // emits event to all observers
observer.reset(); // removes observer from event
```

## lars::ObservableValue

```c++
ObservableValue a = 1;
ObservableValue b = 2;
DependentObservableValue sum([](auto a, auto b){ return a+b; },a,b);
DependentObservableValue resultString([](auto v){ return "The result is " + std::to_string(v); }, sum);
resultString.onChange.connect([](auto &r){ std::cout << r << std::endl; });
```

# Installation and usage

With [CPM](https://github.com/TheLartians/CPM), lars::Event can be added to your project by adding the following to your projects' `CMakeLists.txt`.

```cmake
CPMAddPackage(
  NAME LarsEvent
  VERSION 2.0
  GIT_REPOSITORY https://github.com/TheLartians/Event.git
)

target_link_libraries(myProject LarsEvent)
```

Alternatively, download the repository include it via `add_subdirectory`. Installing lars::Event will make it findable in CMake's `find_package`.
