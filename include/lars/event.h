#pragma once

#include <functional>
#include <memory>
#include <list>
#include <vector>
#include <utility>

namespace lars{
  
  template <typename ... Args> class Event;
  
  class Observer{
  public:
    struct Base{ virtual ~Base(){} };
    
    Observer(){}
    Observer(Observer &&other){ std::swap(data,other.data); }
    template <typename L> Observer(L && l):data(new L(std::move(l))){ }
    
    Observer & operator=(const Observer &other) = delete;

    Observer & operator=(Observer &&other){
      data.reset();
      std::swap(data,other.data);
      return *this;
    }
    
    template <typename L> Observer & operator=(L && l){ 
      data.reset(new L(std::move(l))); 
      return *this; 
    }
    
    template <typename H,typename ... Args> void observe(Event<Args...> & event,H handler){
      data.reset(new typename Event<Args...>::Observer(event,handler));
    }
    
    void reset(){ data.reset(); }
    operator bool() const { return bool(data); }
    
  private:
    std::unique_ptr<Base> data;
  };
  
  template <typename ... Args> class Event{
  private:

    using Handler = std::function<void(const Args &...)>;
    using HandlerList = std::list<Handler>;
    using EventPointer = std::shared_ptr<const Event *>;
    using WeakEventPointer = std::weak_ptr<const Event *>;
    using iterator = typename HandlerList::iterator;
    
    mutable HandlerList observers;
    EventPointer self;

    iterator addHandler(Handler h)const{
      return observers.insert(observers.end(),h);
    }
    
    void eraseHandler(const iterator &it)const{
      observers.erase(it);
    }
    
  public:
    
    struct Observer:public lars::Observer::Base{
      WeakEventPointer parent;
      typename HandlerList::iterator it;
      
      Observer(){ }
      
      Observer(const EventPointer & s,Handler f){
        observe(s,f);
      }
      
      Observer(Observer &&other){
        parent = other.parent;
        it = other.it;
        other.parent.reset();
      }
      
      Observer(const Observer &other) = delete;
      
      Observer & operator=(const Observer &other) = delete;
      
      Observer & operator=(Observer &&other){
        reset();
        parent = other.parent;
        it = other.it;
        other.parent.reset();
        return *this;
      }
      
      void observe(const EventPointer & s,Handler h){
        reset();
        parent = s;
        it = (*s)->addHandler(h);
      }
      
      void reset(){
        if(!parent.expired()) (*parent.lock())->eraseHandler(it);
        parent.reset();
      }
      
      ~Observer(){ reset(); }
    };
    
    Event():self(std::make_shared<const Event *>(this)){
    }
    
    Event(const Event &) = delete;
    Event(Event &&other){
      *this = other;
    }
    
    Event & operator=(const Event &) = delete;

    Event & operator=(Event &&other){
      self = std::move(other.self);
      *self = this;
      observers = std::move(other.observers);
    }
    
    void notify(Args ... args) const {
      for(auto it = observers.begin();it != observers.end();){
        auto &f = *it;
        auto next = it;
        ++next;
        f(args...);
        it = next;
      }
    }
    
    Observer createObserver(const Handler &h)const{
      return Observer(self,h);
    }
    
    void connect(const Handler &h)const{
      addHandler(h);
    }
    
    void clearObservers(){
      observers.clear();
    }
    
    size_t observerCount() const {
      return observers.size();
    }

  };
  
}
