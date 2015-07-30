
#include "events.h"
#include <iostream>

using click_event = lars::event<float,float>;

struct gui_element{
  click_event clicked;
  void mouse_down(float x,float y){ clicked.notify(x, y); }
};

int main(int argc, char **argv) {
  gui_element A,B;
  click_event::listener listener_1,listener_2;
  
  listener_1.observe(A.clicked,[](float x,float y){ std::cout << "l1 : A clicked at " << x << ", " << y << std::endl; });
  listener_2.observe(B.clicked,[](float x,float y){ std::cout << "l2 : B clicked at " << x << ", " << y << std::endl; });
  
  B.clicked.connect([](float x,float y){ std::cout << "   : B clicked at " << x << ", " << y << std::endl; });
  
  {
  auto temporary_listener = A.clicked.create_listener([](float x,float y){ std::cout << "tmp: A clicked at " << x << ", " << y << std::endl; });
  // A has two listeners, B has two listeners
  A.mouse_down(1, 0);
  B.mouse_down(0, 1);
  }

  listener_2 = std::move(listener_1);
  
  // A has one listener, B has one listener
  A.mouse_down(2, 0);
  B.mouse_down(0, 2);
}

