
#include "events.h"
#include <iostream>

using click_event = lars::event<float,float>;

struct gui_element{
  click_event clicked;
  void mouse_down(float x,float y){ clicked.notify(x, y); }
};

int main(int argc, char **argv) {
  gui_element A,B;
  click_event::listener listener_1;
  // pseudo-base class can hold any type of listener
  lars::listener listener_2;
  
  listener_1.observe(A.clicked,[](float x,float y){ std::cout << "l1 : A clicked at " << x << ", " << y << std::endl; });
  listener_2.observe(B.clicked,[](float x,float y){ std::cout << "l2 : B clicked at " << x << ", " << y << std::endl; });
  
  // Anonymous listener
  B.clicked.connect([](float x,float y){ std::cout << "   : B clicked at " << x << ", " << y << std::endl; });
  
  {
  auto temporary_listener = A.clicked.create_listener([](float x,float y){ std::cout << "tmp: A clicked at " << x << ", " << y << std::endl; });
  std::cout << "A has two listeners, B has two listeners" << std::endl;
  A.mouse_down(1, 0);
  B.mouse_down(0, 1);
  }

  listener_2 = std::move(listener_1);
  
<<<<<<< HEAD
  std::cout << "A has one listener, B has one listener" << std::endl;
=======
  // A has one listener, B has one listener
>>>>>>> 35b69e5148a3ba644715d27e972d7775da56f510
  A.mouse_down(2, 0);
  B.mouse_down(0, 2);
}

