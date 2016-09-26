#pragma once

#include <functional>
#include <memory>
#include <list>

namespace lars{
  
  template <typename ... Args> class Event;
  
  class Listener{
    public:
    struct Base{ virtual ~Base(){} };
    
    Listener(){}
    Listener(Listener &&other){ std::swap(data,other.data); }
    template <typename L> Listener(L && l):data(new L(std::move(l))){ }
    
    Listener & operator=(const Listener &other) = delete;
    Listener & operator=(Listener &&other){
      data.reset();
      std::swap(data,other.data);
      return *this;
    }
    
    template <typename L> Listener & operator=(L && l){ data.reset(new L(std::move(l))); return *this; }
    
    template <typename H,typename ... Args> void observe(Event<Args...> & the_event,H Handler){
      data.reset(new typename Event<Args...>::Listener(the_event,Handler));
    }
    
    void reset(){ data.reset(); }
    operator bool(){ return bool(data); }
    
    private:
    std::unique_ptr<Base> data;
  };
  
  template <typename ... Args> class Event:private std::shared_ptr<Event<Args...>*>{
    
    using Handler = std::function<void(Args...)>;
    using ListenerList = std::list<Handler>;
    using iterator = typename ListenerList::iterator;
    
    mutable ListenerList listeners;

    iterator insert_handler(Handler h)const{
      return listeners.insert(listeners.end(),h);
    }
    
    void erase_handler(const iterator &it)const{
      listeners.erase(it);
    }
    
    public:
    
    struct Listener:public lars::Listener::Base{
      std::weak_ptr<Event*> the_event;
      typename ListenerList::iterator it;
      
      Listener(){ }
      
      Listener(const Event & s,Handler f){
        observe(s,f);
      }
      
      Listener(Listener &&other){
        the_event = other.the_event;
        it = other.it;
        other.the_event.reset();
      }
      
      Listener(const Listener &other) = delete;
      
      Listener & operator=(const Listener &other) = delete;
      
      Listener & operator=(Listener &&other){
        reset();
        the_event = other.the_event;
        it = other.it;
        other.the_event.reset();
        return *this;
      }
      
      void observe(const Event & s,Handler h){
        reset();
        the_event = s;
        it = s.insert_handler(h);
      }
      
      void reset(){
        if(!the_event.expired()) (*the_event.lock())->erase_handler(it);
        the_event.reset();
      }
      
      ~Listener(){ reset(); }
    };
    
    Event():std::shared_ptr<Event*>(std::make_shared<Event*>(this)){ }
    
    Event(const Event &) = delete;
    Event(Event &&) = default;
    
    Event & operator=(const Event &) = delete;
    Event & operator=(Event &&) = default;
    
    void notify(Args... args)const{
      for(auto &f:listeners) f(args...);
    }
    
    Listener create_listener(Handler h)const{
      return Listener(*this,h);
    }
    
    void connect(Handler h)const{
      insert_handler(h);
    }
    
  };
  
  template <class T> class ObservableValue{
    T value;
    public:
    Event<const T &> on_change;
    
    template <typename ... Args> ObservableValue(Args ... args):value(args...){}
    
    const T & get()const{ return value; }
    void set(const T &other){ value = other; on_change.notify(value); }
  };
  
  template <class T> using SharedObservableValue = std::shared_ptr<ObservableValue<T>>;
  
  template <class T,typename ... Args> SharedObservableValue<T> make_shared_observable_value(Args ... args){
    return std::make_shared<ObservableValue<T>>(args...);
  }
  
}






