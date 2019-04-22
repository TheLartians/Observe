[![Build Status](https://travis-ci.com/TheLartians/Event.svg?branch=master)](https://travis-ci.com/TheLartians/Event)

# lars::Event

A thread-safe C++11 event-listener template.

# Example



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
