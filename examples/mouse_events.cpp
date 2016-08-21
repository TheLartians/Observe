
#include <lars/event.h>
#include <iostream>

using namespace lars;
using namespace std;

using ClickEvent = Event<float,float>;

struct GuiElement{
  ClickEvent clicked;
  void mouse_down(float x,float y){ clicked.notify(x, y); }
};

int main(int argc, char **argv) {
  GuiElement A,B;
  ClickEvent::Listener listener_1;
  // pseudo-base class can hold any type of listener
  Listener listener_2;
  
  listener_1.observe(A.clicked,[](float x,float y){ cout << "l1 : A clicked at " << x << ", " << y << endl; });
  listener_2.observe(B.clicked,[](float x,float y){ cout << "l2 : B clicked at " << x << ", " << y << endl; });
  
  // anonymous listener
  B.clicked.connect([](float x,float y){ cout << "   : B clicked at " << x << ", " << y << endl; });
  
  {
  auto temporary_listener = A.clicked.create_listener([](float x,float y){ cout << "tmp: A clicked at " << x << ", " << y << endl; });
  cout << "A has two listeners, B has two listeners" << endl;
  A.mouse_down(1, 0);
  B.mouse_down(0, 1);
  }

  listener_2 = move(listener_1);
  
  cout << "A has one listener, B has one listener" << endl;
  A.mouse_down(2, 0);
  B.mouse_down(0, 2);
}

