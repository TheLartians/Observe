#include <catch2/catch.hpp>

#include <lars/observable_value.h>

TEST_CASE("Observable Value") {
  using namespace lars;

  ObservableValue<int> value(0);

  unsigned total = 0;
  value.onChange.connect([&](auto &v){ total+=v; });
  REQUIRE(*value == 0);
  value.set(1);
  value.set(2);
  REQUIRE(total == 3);
  REQUIRE(*value == 2);
}

TEST_CASE("Dependent Observable Value") {
  using namespace lars;

  ObservableValue a(1);
  ObservableValue b(2);
  DependentObservableValue sum([](auto a, auto b){ return a+b; },a,b);

  REQUIRE(*sum == 3);
  a.set(2);
  REQUIRE(*sum == 4);

  ObservableValue c(3);
  DependentObservableValue prod([](auto a, auto b){ return a*b; },sum,c);

  REQUIRE(*prod == 12);
  a.set(1);
  REQUIRE(*prod == 9);
  b.set(4);
  REQUIRE(*prod == 15);
  c.set(2);
  REQUIRE(*prod == 10);
  
  // TODO: test moving an observable value
}