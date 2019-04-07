#include <catch2/catch.hpp>

#include <lars/event.h>

TEST_CASE("ClickEvent") {
  using namespace lars;
  
  using ClickEvent = Event<float,float>;
  
  struct GuiElement{
    ClickEvent clicked;
    void mouse_down(float x,float y){ clicked.notify(x, y); }
  };

  GuiElement A,B;
  ClickEvent::Observer observer_1;
  // pseudo-base class can hold any type of observer
  Observer observer_2;
  
  float ox = 0, oy = 0;
  size_t o1Count = 0, o2Count = 0, o3Count = 0, o4Count = 0;
  
  observer_1.observe(A.clicked,[&](float x,float y){ REQUIRE(ox == x); REQUIRE(oy == y); o1Count++; });
  observer_2.observe(B.clicked,[&](float x,float y){ REQUIRE(ox == x); REQUIRE(oy == y); o2Count++; });
  REQUIRE(A.clicked.observers_count() == 1);
  REQUIRE(B.clicked.observers_count() == 1);

  do {
    auto temporary_observer = A.clicked.create_observer([&](float x,float y){ REQUIRE(ox == x); REQUIRE(oy == y); o4Count++; });
    REQUIRE(A.clicked.observers_count() == 2);
    
    ox += 1;
    A.mouse_down(ox, oy);
    
    // anonymous observer
    B.clicked.connect([&](float x,float y){ REQUIRE(ox == x); REQUIRE(oy == y); o3Count++; });
    REQUIRE(A.clicked.observers_count() == 2);
    REQUIRE(B.clicked.observers_count() == 2);

    oy += 1;
    B.mouse_down(ox, oy);
  } while (0);
  
  observer_2 = std::move(observer_1);
  
  REQUIRE(A.clicked.observers_count() == 1);
  REQUIRE(B.clicked.observers_count() == 1);
  
  ox += 1;
  A.mouse_down(ox, oy);
  oy += 1;
  B.mouse_down(ox, oy);

  REQUIRE(o1Count == 2);
  REQUIRE(o2Count == 1);
  REQUIRE(o3Count == 2);
  REQUIRE(o4Count == 1);

}
