#pragma once

#include <functional>
#include <memory>
#include <list>

namespace lars{
  
  template <typename ... Args> struct event;
  
  class listener{
    public:
    struct base{};
    
    listener(){}
    listener(listener &&other){ std::swap(data,other.data); }
    template <typename L> listener(L && l):data(new L(std::move(l))){ }
    
    listener & operator=(const listener &other) = delete;
    listener & operator=(listener &&other){
      data.reset();
      std::swap(data,other.data);
      return *this;
    }
    template <typename L> listener & operator=(L && l){ data.reset(new L(std::move(l))); return *this; }
    
    template <typename H,typename ... Args> void observe(event<Args...> & the_event,H handler){
      data.reset(new typename event<Args...>::listener(the_event,handler));
    }
    
    private:
    std::unique_ptr<base> data;
  };
  
  template <typename ... Args> struct event:private std::shared_ptr<std::list<std::function<void(Args...)>>>{
    
    using handler = std::function<void(Args...)>;
    using listener_list = std::list<handler>;
    
    struct listener:public lars::listener::base{
      std::weak_ptr<listener_list> the_event;
      typename listener_list::iterator it;
      
      listener(){ }
      
      listener(event & s,handler f){
        observe(s,f);
      }
      
      listener(listener &&other){
        the_event = other.the_event;
        it = other.it;
        other.the_event.reset();
      }
      
      listener(const listener &other) = delete;
      
      listener & operator=(const listener &other) = delete;
      
      listener & operator=(listener &&other){
        reset();
        the_event = other.the_event;
        it = other.it;
        other.the_event.reset();
        return *this;
      }
      
      void observe(event & s,handler f){
        reset();
        the_event = s;
        it = s->insert(s->end(),f);
      }
      
      void reset(){
        if(!the_event.expired()) the_event.lock()->erase(it);
        the_event.reset();
      }
      
      ~listener(){ reset(); }
    };
    
    event():std::shared_ptr<listener_list>(std::make_shared<listener_list>()){ }
    event(const event &) = delete;
    event & operator=(const event &) = delete;
    
    void notify(Args... args){
      for(auto &f:**this) f(args...);
    }
    
    listener create_listener(handler h){
      return listener(*this,h);
    }
    
    void connect(handler h){
      this->get()->insert(this->get()->end(),h);
    }
    
  };
  
}