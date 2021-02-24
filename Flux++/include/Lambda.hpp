#ifndef LAMBDA_HPP
#define LAMBDA_HPP
#include <cstring>
#include <new>
#include <cstdlib>
#include <utility>

/*
 *  Ok a little explanation: 
 *   FunctionSignature is simply a holder class for the variadic Arguments, to separate them in variadic argument lists of other classes
 *  
 *   Closure and Fun are the Interface classes. Users might need to type them out
 *   
 *   Closure<FunctionSignature<return_t, Args_t...>, Closed_t...>
 *     this class represents a Closure where arguments are closed over.
 *     it holds a ClosureContainer
 * 
 *   Fun<return_t, Args_t...>
 *     this class represents a closure where the closed over elements are erased.
 *     it holds a pointer to a ClosureBase / ClosureHolder
 *
 *
 *     naming for the internal classes is not very intuitive.
 *     
 *    ClosureContainer is the container within a Closure or a Fun(ClosureHolder ) that contains the actual function pointer as well as all bound values.
 *
 *    ClosureHolder and ClosureBase implement the erasure of the closed over types.
 *    ClosureBase the Baseclass is only dependent on the  signature of the operator() 
 *    ClosureHolder is dependent on also the closed-over types and implements ClosureBase's methods.
 *
 * 
 */



namespace fluxpp {

  // FunctionSignature
  namespace transparent_closure {

    template<class return_t, class ...argument_t>
    struct FunctionSignature{
    public:
      using function_ptr_type = return_t(*)(argument_t...); 
      using return_type = return_t;
    };
    
  }
  
  namespace transparent_closure{
    
    struct  MemCompareInfo{
      void* obj;
      std::size_t size;
    };
    

    

    template<class return_t , class ...Args_t>
    struct ClosureBase{
      virtual return_t operator()(Args_t... ) =0;
      virtual  MemCompareInfo get_mem_compare_info()=0;
    };
    
    template<class return_t, class ... Args_t>
    class Fun{
      Fun( ClosureBase<return_t, Args_t...>* closure ): closure(closure){};
      MemCompareInfo get_mem_compare_info() {
	return this->closure->get_mem_compare_info();
      };
      
      return_t operator()(Args_t... args){
        return this->closure->operator()(args...);
      };
      
      ~Fun(){
	this->closure->~ClosureBase<return_t, Args_t...>();
	std::free(this->closure );
      };
    private:
      ClosureBase<return_t, Args_t...>* closure;
    };

    
    template< class current_function_signature, class ...closed_t>
    class ClosureContainer;

    //BaseContainer (wraps only a function pointer)
    // the function has no arguments
    template<class return_t, class ...Arg_t  >
    class ClosureContainer<
      FunctionSignature<return_t,Arg_t...>>{
    public:
      ClosureContainer() =default;
      ClosureContainer(return_t(*fn)(Arg_t... ) ):fn(fn){};
      return_t operator( )(Arg_t...args ){
	return (*fn)(args... );
      }
      //      Fun<return_t, Arg_t...> as_fun(){
      //	auto p = new ClosureHolder<>;
      // }
    private:
      return_t (*fn)(Arg_t... );
    };

    //BaseContainer (wraps only a function pointer)
    // the function has at least one argument
    template<class return_t, class first_t, class ...Arg_t  >
    class ClosureContainer<
      FunctionSignature<return_t,first_t, Arg_t...>>{
    public:
      ClosureContainer() =default;
      ClosureContainer(return_t(*fn)(first_t, Arg_t... ) ):fn(fn){};
      return_t operator( )(first_t first, Arg_t...args ){
	return (*fn)(first, args... );
      }

      
      decltype(auto) bind(first_t closed_arg) {
	return ClosureContainer<FunctionSignature<return_t, Arg_t...>,first_t>(*this, closed_arg);  
      }
    private:
      return_t (*fn)(first_t,Arg_t... );
    };



    // Closure
    template<
      class return_t,
      class first_closure_t,
      class ...Args,
      class ...closure_t >
    class ClosureContainer<
      FunctionSignature<return_t,Args...>,
      first_closure_t,
      closure_t...>: ClosureContainer<
      FunctionSignature<return_t,first_closure_t, Args...>,
      closure_t...>
    {
    public:
      ClosureContainer() =default;
      ClosureContainer(ClosureContainer<
		       FunctionSignature<return_t,first_closure_t, Args...>,
		       closure_t...> closure,
		       first_closure_t first): ClosureContainer<
	FunctionSignature<return_t,first_closure_t, Args...>,
	closure_t...>(closure),first(first){}; 

      //      ClosureHolder<>
      return_t operator()(Args... args){
	return ClosureContainer<
	  FunctionSignature<return_t,first_closure_t, Args...>,
	  closure_t...>::operator()(this->first,args... );
	
      };

    private:
      first_closure_t first;
    };


    template<
      class return_t,
      class first_closure_t,
      class first_arg_t,
      class ...Args_t,
      class ...closure_t >
    class ClosureContainer<
      FunctionSignature<return_t, first_arg_t,Args_t...>,
      first_closure_t,
      closure_t...>: ClosureContainer<
      FunctionSignature<return_t,first_closure_t,first_arg_t, Args_t...>,
      closure_t...>
    {
    public:
      ClosureContainer() =default;
      ClosureContainer(ClosureContainer<
		    FunctionSignature<return_t,first_closure_t, first_arg_t,Args_t...>,
		    closure_t...> closure,
		    first_closure_t first): ClosureContainer<
		    FunctionSignature<return_t,first_closure_t, first_arg_t, Args_t...>,
	closure_t...>(closure),first(first){}; 

      decltype(auto) bind(first_arg_t closed_arg) {
	return ClosureContainer<FunctionSignature<return_t, Args_t...>,
		       first_arg_t,
		       first_closure_t,
		       closure_t...>(*this, closed_arg);  
      }
      //      Closure<>
      return_t operator()(first_arg_t first, Args_t... args){
	return ClosureContainer<
	  FunctionSignature<return_t,first_closure_t, first_arg_t,Args_t...>,
	  closure_t...>::operator()(this->first,first,args... );
	
      };

    private:
      first_closure_t first;
    };



    
    template <class ...M>
    struct ClosureHolder;
    
    template<class return_t, class ...T, class ...M>
    struct ClosureHolder<FunctionSignature<return_t, T...>, M...>:ClosureBase<return_t, T...>{
    private:
      using closure_container_t = ClosureContainer<FunctionSignature<return_t,T...>, M...>;
    public:
      ClosureHolder(closure_container_t closure_container):closure_container(std::move(closure_container)){};
      
      return_t operator()(T...args ){
	return this->closure_container(args... );
      };
      
      MemCompareInfo get_mem_compare_info(){
	MemCompareInfo info{};
	info.obj = static_cast<void*>(&(this->closure_container) );
	info.size = sizeof(closure_container_t);
	return info;
      };
    private:
      closure_container_t closure_container;
    };


    
    template <class ...T>
    class Closure;
    
    template <class return_t, class ...Args_t, class ...Closed_t >
    class Closure<FunctionSignature<return_t, Args_t...>,  Closed_t...>{
    private:
      using closure_container_t = ClosureContainer<FunctionSignature<return_t, Args_t...>,  Closed_t...>;
      using closure_holder_t = ClosureHolder<FunctionSignature<return_t, Args_t...>,  Closed_t...>;
    public:
      Closure(closure_container_t closure_container) : closure_container( std::move(closure_container)){};
      
      return_t operator()(Args_t... args){
	return this->closure_container(args...);
      }

      template<class T>
      decltype(auto) bind(T arg){
	return this->closure_container.bind(arg);
      }

      Fun<return_t, Args_t...> as_fun(){
	void* memory_vptr = std::aligned_alloc(alignof(closure_holder_t), sizeof(closure_holder_t));
	if ( ! memory_vptr){
	  std::bad_alloc exc;
	  throw exc;
	};
	std::memset(memory_vptr, 0,sizeof(closure_holder_t));
	auto closure_holder_ptr =  new ( memory_vptr) closure_holder_t(this->closure_container);
	return Fun<return_t, Args_t...>( closure_holder_ptr );
      }
    private:
      closure_container_t closure_container ;

    };
      

    
    template<class ...T>
    struct ClosureMaker {
      template <class M>
      static Closure<FunctionSignature<T...>>  make(M m){
	return Closure<FunctionSignature<T...>>( ClosureContainer<FunctionSignature<T...>> (m));
      };
    };
    
    
    
    
    /*
    template <class T>
    struct get_base_fun;
    
    template< class current_function_signature,
	      class closure_tuple_t,
	      class return_t , class...T>
    struct get_base_fun<
      ClosureHolder<
	FunctionSignature<return_t,T...>,
	current_function_signature,
	closure_tuple_t> >{
      using type = BaseFun<Ret,T...>;
    };

    
    template<   class closure_holder_t>
    class Fun ;
    
    
    template <class current_function_signature,
	      class closure_tuple_t,
	      class return_t , class...T>
    class Fun < ClosureHolder<
	FunctionSignature<return_t,T...>,
	current_function_signature,
		  closure_tuple_t>>: BaseFun<return_t, ...T> {
    private:
      using closure_holder_t = ClosureHolder<
      FunctionSignature<return_t,T...>,
      current_function_signature,
      closure_tuple_t>;
    public:
      Fun(closure_holder_t closure ): closure(closure){};
      Ret operator()(T...args) override{
	return closure(args... );
      }
    private:
      closure_holder_t closure;
    };

    
    
    CaptureHolder<class T...> with(T... captured){
      return CaptureHolder<class T...>(captured...);
    };

   
    */


  }




}
#endif //LAMBDA_HPP
