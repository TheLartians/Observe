#pragma once

#include <functional>
#include <memory>
#include <list>
#include <vector>
#include <utility>

#include <iostream>

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
    
    void clear_observer(){ observers.clear(); }
    void pop_observer(){ observers.pop_back(); }
    
  };
  
  template <typename ... Args> class Event:private std::shared_ptr<Event<Args...>*>{
    
    using Handler = std::function<void(const Args &...)>;
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
    
    void transfer_observers_to(Event &other){
      for(auto &observer:observers) observer->the_event = &other;
      other.observers.splice(other.observers.end(),observers.begin(),observers.end());
    }
    
    void clear_observers(){
      observers.clear();
    }
    
  };
  
  struct ObservableValueBase{
    Event<> on_set;
  };
  
  struct ObservableValueWithChangeEventBase:public ObservableValueBase{
    Event<> on_change;
  };
  
  template <class T,class Base = ObservableValueBase> class ObservableValue:public Base{
    T value;
    public:
    std::function<void(T &)> converter = [](T &){};
    
    template <typename ... Args> ObservableValue(Args ... args):value(std::forward<Args>(args)...){}
    
    const T & get()const{ return value; }
    
    operator const T &()const{ return get(); }
    
    void set(const T &other){ value = other; converter(value); Base::on_set.notify(); }
    void set(T &&other){ value = std::forward<T>(other); converter(value); Base::on_set.notify(); }
    ObservableValue & operator=(const T &other){ set(other); }
    ObservableValue & operator=(T &&other){ set(std::forward<T>(other)); }
    
    void set_silently(const T &other){ value = other; converter(value); }
    void set_silently(T &&other){ value = std::forward<T>(other); converter(value); }
  };
  
  template <class T,class Base = ObservableValueWithChangeEventBase> class ObservableValueWithChangeEvent:public ObservableValue<T,Base>{
  private:
    T previous_value = T();
    
    void init(){
      this->on_set.connect([this](){
        if(previous_value != this->get()){ Base::on_change.notify(); previous_value = this->get(); }
      });
    }
    
  public:
    template <typename ... Args> ObservableValueWithChangeEvent(Args ... args):ObservableValue<T,Base>(std::forward<Args>(args)...){ init(); }
  };

  template <class T,class Base = ObservableValueBase> class SharedObservableValue:public std::shared_ptr<ObservableValue<T,Base>>{
  public:
    using std::shared_ptr<ObservableValue<T,Base>>::shared_ptr;
  };
  
  template <class T,typename ... Args,class Base = ObservableValueBase> SharedObservableValue<T,Base> make_shared_observable_value(Args ... args){
    return std::make_shared<ObservableValue<T,Base>>(args...);
  }
  
  
}






