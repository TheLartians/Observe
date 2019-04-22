#include <catch2/catch.hpp>

#include <lars/event.h>

using namespace lars;

// instantiate template for coverage
template class lars::Event<>;

TEST_CASE("Event"){

  SECTION("connect and observe"){
    lars::Event<> event;
    REQUIRE(event.observerCount() == 0);
    unsigned connectCount = 0, observeCount = 0;
    event.connect([&](){ connectCount++; });
    
    SECTION("reset observer"){
      lars::Event<>::Observer observer;
      observer = event.createObserver([&](){ observeCount++; });
      for (int i=0; i<10; ++i) { event.emit(); }
      REQUIRE(event.observerCount() == 2);
      observer.reset();
      REQUIRE(event.observerCount() == 1);
      for (int i=0; i<10; ++i) { event.emit(); }
      REQUIRE(observeCount == 10);
      REQUIRE(connectCount == 20);
    }

    SECTION("scoped observer"){
      SECTION("lars::Observer"){
        lars::Observer observer;
        observer.observe(event, [&](){ observeCount++; });
        REQUIRE(event.observerCount() == 2);
        for (int i=0; i<10; ++i) { event.emit(); }
      }
      SECTION("lars::Event<>::Observer"){
        lars::Event<>::Observer observer;
        observer.observe(event, [&](){ observeCount++; });
        REQUIRE(event.observerCount() == 2);
        for (int i=0; i<10; ++i) { event.emit(); }
      }
      REQUIRE(event.observerCount() == 1);
      for (int i=0; i<10; ++i) { event.emit(); }
      REQUIRE(observeCount == 10);
      REQUIRE(connectCount == 20);
    }

    SECTION("clear observers"){
      lars::Observer observer = event.createObserver([&](){ observeCount++; });
      event.clearObservers();
      REQUIRE(event.observerCount() == 0);
      event.emit();
      REQUIRE(connectCount == 0);
    }
  }

  SECTION("scoped event"){
  }

  SECTION("emit data"){
    lars::Event<int, int> event;
    int sum = 0;
    event.connect([&](auto a, auto b){ sum = a + b; });
    event.emit(2,3);
    REQUIRE(sum == 5);
  }

  SECTION("move"){
    lars::Observer observer;
    int result = 0;
    lars::Event<int> event;
    {
      lars::Event<int> tmpEvent;
      observer = tmpEvent.createObserver([&](auto i){ result = i; });
      tmpEvent.emit(5);
      REQUIRE(result == 5);
      event = Event(std::move(tmpEvent));
      REQUIRE(tmpEvent.observerCount() == 0);
    }
    REQUIRE(event.observerCount() == 1);
    event.emit(3);
    REQUIRE(result == 3);
    observer.reset();
    REQUIRE(event.observerCount() == 0);
  }

}

