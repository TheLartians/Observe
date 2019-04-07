
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
  ClickEvent::Observer observer_1;
  // pseudo-base class can hold any type of observer
  Observer observer_2;
  
  observer_1.observe(A.clicked,[](float x,float y){ 
    cout << "observer 1 : A clicked at " << x << ", " << y << endl;
  });

  observer_2.observe(B.clicked,[](float x,float y){
    cout << "observer 2 : B clicked at " << x << ", " << y << endl;
  });
  
  // anonymous observer
  B.clicked.connect([](float x,float y){
    cout << "   : B clicked at " << x << ", " << y << endl;
  });
  
  {
  auto temporary_observer = A.clicked.create_observer([](float x,float y){
    cout << "tmp observer: A clicked at " << x << ", " << y << endl;
  });
  cout << "A has two observers, B has two observers" << endl;
  A.mouse_down(1, 0);
  B.mouse_down(0, 1);
  }

  observer_2 = move(observer_1);
  
  cout << "A has one observer, B has one observer" << endl;
  A.mouse_down(2, 0);
  B.mouse_down(0, 2);
}

