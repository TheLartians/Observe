#pragma once

#include <lars/event.h>
#include <tuple>
#include <type_traits>
#include <utility>

#include <iostream>

namespace lars {

  template <class T> class ObservableValue {
  protected:
    T value;

  public:
    using OnChange = Event<const T &>;
    OnChange onChange;

    template <typename ... Args> ObservableValue(Args ... args):value(std::forward<Args>(args)...){
    }

    template <typename ... Args> void set(Args ... args){ 
      value = T(std::forward<Args>(args)...);
      onChange.emit(value);
    }
    
    const T & get()const{ 
      return value;
    }
    
    const T & operator*()const{ 
      return value;
    }
    
    const T * operator->()const{ 
      return &value;
    }
    
  };

  template <class T> ObservableValue(T) -> ObservableValue<T>;

  template <class T, typename ... D> class DependentObservableValue: public ObservableValue<T> {
  private:
    std::tuple<const D * ...> values;
    std::tuple<typename ObservableValue<D>::OnChange::Observer ...> observers;

    template <class H, size_t ... Idx> DependentObservableValue(
      std::index_sequence<Idx...> const &,
      const H &handler,
      const ObservableValue<D> & ... deps
    ):ObservableValue<T>(handler(deps.get()...)){
      
      values = std::make_tuple(&deps.get() ...);
      auto callHandler = [handler](const D * ... v){ return handler(*v...); };
      auto update = [this, callHandler](){
        this->set(std::apply(callHandler, values)); 
      };

      update();
      
      observers = (std::make_tuple(deps.onChange.createObserver([this,update](const auto &v){
        std::get<Idx>(values) = &v;
        update();
      })...));
    }
  
  public:

    template <class H> DependentObservableValue(
      const H &handler,
      const ObservableValue<D> & ... deps
    ): DependentObservableValue(std::make_index_sequence<sizeof...(D)>(), handler, deps...) { }

  };

  template<class F, typename ... D> DependentObservableValue(
    F,
    const ObservableValue<D> &...
  ) -> DependentObservableValue<typename std::invoke_result<F, const D &...>::type, D...>;

}