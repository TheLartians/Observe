#pragma once

#include <functional>
#include <memory>
#include <list>
#include <vector>

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
    
    template <typename L> Observer & operator=(L && l){ data.reset(new L(std::move(l))); return *this; }
    
    template <typename H,typename ... Args> void observe(Event<Args...> & event,H handler){
      data.reset(new typename Event<Args...>::Observer(event,handler));
    }
    
    void reset(){ data.reset(); }
    operator bool(){ return bool(data); }
    
    private:
    std::unique_ptr<Base> data;
  };
  
  class MultiOberserver{
  protected:
    std::vector<Observer> observers;
  public:
    void add_observer(Observer && l){ observers.emplace_back(std::move(l)); }
    template <typename H,typename ... Args> void observe(Event<Args...> & event,H handler){
      observers.emplace_back();
      observers.back().observe(event,handler);
    }
  };
  
  template <typename ... Args> class Event:private std::shared_ptr<Event<Args...>*>{
    
    using Handler = std::function<void(Args...)>;
    using ObserverList = std::list<Handler>;
    using iterator = typename ObserverList::iterator;
    
    mutable ObserverList observers;

    iterator insert_handler(Handler h)const{
      return observers.insert(observers.end(),h);
    }
    
    void erase_handler(const iterator &it)const{
      observers.erase(it);
    }
    
    public:
    
    struct Observer:public lars::Observer::Base{
      std::weak_ptr<Event*> the_event;
      typename ObserverList::iterator it;
      
      Observer(){ }
      
      Observer(const Event & s,Handler f){
        observe(s,f);
      }
      
      Observer(Observer &&other){
        the_event = other.the_event;
        it = other.it;
        other.the_event.reset();
      }
      
      Observer(const Observer &other) = delete;
      
      Observer & operator=(const Observer &other) = delete;
      
      Observer & operator=(Observer &&other){
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
      
      ~Observer(){ reset(); }
    };
    
    Event():std::shared_ptr<Event*>(std::make_shared<Event*>(this)){ }
    
    Event(const Event &) = delete;
    Event(Event &&) = default;
    
    Event & operator=(const Event &) = delete;
    Event & operator=(Event &&) = default;
    
    void notify(Args... args)const{
      for(auto &f:observers) f(args...);
    }
    
    Observer create_observer(Handler h)const{
      return Observer(*this,h);
    }
    
    void connect(Handler h)const{
      insert_handler(h);
    }
    
  };
  
  template <class T> class ObservableValue{
    T value;
    public:
    Event<const T &> on_change;
    std::function<void(T &)> converter = [](T &){};
    
    template <typename ... Args> ObservableValue(Args ... args):value(args...){}
    
    const T & get()const{ return value; }
    operator const T &()const{ return get(); }
    void set(const T &other){ value = other; formatter(value); on_change.notify(value); }
  };
  
  template <class T> using SharedObservableValue = std::shared_ptr<ObservableValue<T>>;
  
  template <class T,typename ... Args> SharedObservableValue<T> make_shared_observable_value(Args ... args){
    return std::make_shared<ObservableValue<T>>(args...);
  }
  
  
}






