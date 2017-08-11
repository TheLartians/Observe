#include <lars/event.h>

#include <mutex>

namespace lars{

  template <typename ... Args> class ThreadSafeEvent:private std::shared_ptr<ThreadSafeEvent<Args...>*>{
    
    using Handler = std::function<void(const Args &...)>;
    using ObserverList = std::list<Handler>;
    using iterator = typename ObserverList::iterator;
    mutable std::mutex mutex;
    
    mutable ObserverList observers;
    
    iterator insert_handler(Handler h)const{
      std::lock_guard<std::mutex> guard(mutex);
      return observers.insert(observers.end(),h);
    }
    
    void erase_handler(const iterator &it)const{
      std::lock_guard<std::mutex> guard(mutex);
      observers.erase(it);
    }
    
  public:
    
    struct Observer:public lars::Observer::Base{
      std::weak_ptr<ThreadSafeEvent*> the_event;
      typename ObserverList::iterator it;
      
      Observer(){ }
      
      Observer(const ThreadSafeEvent & s,Handler f){
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
      
      void observe(const ThreadSafeEvent & s,Handler h){
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
    
    ThreadSafeEvent():std::shared_ptr<ThreadSafeEvent*>(std::make_shared<ThreadSafeEvent*>(this)){ }
    
    ThreadSafeEvent(const ThreadSafeEvent &) = delete;
    ThreadSafeEvent(ThreadSafeEvent &&) = default;
    
    ThreadSafeEvent & operator=(const ThreadSafeEvent &) = delete;
    ThreadSafeEvent & operator=(ThreadSafeEvent &&) = default;
    
    void notify(Args... args)const{
      decltype(observers) safe_observers;
      {
        std::lock_guard<std::mutex> guard(mutex);
        safe_observers = observers;
      }
      for(auto &f:safe_observers) f(args...);
    }
    
    Observer create_observer(Handler h)const{
      return Observer(*this,h);
    }
    
    void connect(Handler h)const{
      insert_handler(h);
    }
    
    void transfer_observers_to(ThreadSafeEvent &other){
      for(auto &observer:observers) observer->the_event = &other;
      other.observers.splice(other.observers.end(),observers.begin(),observers.end());
    }
    
    void clear_observers(){
      observers.clear();
    }
    
  };

  
}

