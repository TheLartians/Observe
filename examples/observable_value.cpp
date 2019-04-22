/**
 * A small example that shows how to create and use ObservableValue and DependentObservableValue.
 */

#include <lars/observable_value.h>
#include <string>
#include <iostream>


int main() {
  using namespace lars;

  ObservableValue a = 1;
  a.onChange.connect([](auto v){ std::cout << "a changed to " << v << std::endl; });

  ObservableValue b = 2;
  b.onChange.connect([](auto v){ std::cout << "b changed to " << v << std::endl; });

  ObservableValue c = 3;
  c.onChange.connect([](auto v){ std::cout << "c changed to " << v << std::endl; });

  DependentObservableValue sum([](auto a, auto b){ return a+b; },a,b);
  DependentObservableValue prod([](auto a, auto b){ return a*b; },sum,c);

  DependentObservableValue resultString([](auto v){ return "The result is " + std::to_string(v); }, prod);
  resultString.onChange.connect([](auto &r){ std::cout << r << std::endl; });
  
  std::cout << resultString.get() << std::endl;

  a.set(2);
  b.set(3);
  c.set(4);
}