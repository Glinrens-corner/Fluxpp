#ifndef LAMBDA_HPP
#define LAMBDA_HPP
#include <cstring>
#include <tuple>

namespace fluxpp {

  namespace lambda {
    using std::tuple;

    template<class return_t, class ...argument_t>
    struct FunctionSignature{
    public:
      using function_ptr_type = return_t(*)(argument_t...); 
      using return_type = return_t;
    };
    
    template <class return_t, class first_t, class ...argument_t>
    struct FunctionSignature<return_t, first_t, argument_t...>{
    public:
      using function_ptr_type = return_t(*)(argument_t...); 
      using return_type = return_t;
      using first_type = first_t;
    };

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
    private:
      ClosureBase<return_t, Args_t...>* closure;
    };

    
    template< class current_function_signature, class ...closed_t>
    class Closure;
    
    template<class return_t, class ...Arg_t  >
    class Closure<
      FunctionSignature<return_t,Arg_t...>>{
    public:
      Closure() =default;
      Closure(return_t(*fn)(Arg_t... ) ):fn(fn){};
      return_t operator( )(Arg_t...args ){
	return (*fn)(args... );
      }
      //      Fun<return_t, Arg_t...> as_fun(){
      //	auto p = new ClosureHolder<>;
      // }
    private:
      return_t (*fn)(Arg_t... );
    };


    template<class return_t, class first_t, class ...Arg_t  >
    class Closure<
      FunctionSignature<return_t,first_t, Arg_t...>>{
    public:
      Closure() =default;
      Closure(return_t(*fn)(first_t, Arg_t... ) ):fn(fn){};
      return_t operator( )(first_t first, Arg_t...args ){
	return (*fn)(first, args... );
      }

      
      decltype(auto) bind(first_t closed_arg) {
	return Closure<FunctionSignature<return_t, Arg_t...>,first_t>(*this, closed_arg);  
      }
    private:
      return_t (*fn)(first_t,Arg_t... );
    };



    
    template<
      class return_t,
      class first_closure_t,
      class ...Args,
      class ...closure_t >
    class Closure<
      FunctionSignature<return_t,Args...>,
      first_closure_t,
      closure_t...>: Closure<
      FunctionSignature<return_t,first_closure_t, Args...>,
      closure_t...>
    {
    public:
      Closure() =default;
      Closure(Closure<
		    FunctionSignature<return_t,first_closure_t, Args...>,
		    closure_t...> closure,
		    first_closure_t first): Closure<
		    FunctionSignature<return_t,first_closure_t, Args...>,
	closure_t...>(closure),first(first){}; 

      //      ClosureHolder<>
      return_t operator()(Args... args){
	return Closure<
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
    class Closure<
      FunctionSignature<return_t, first_arg_t,Args_t...>,
      first_closure_t,
      closure_t...>: Closure<
      FunctionSignature<return_t,first_closure_t,first_arg_t, Args_t...>,
      closure_t...>
    {
    public:
      Closure() =default;
      Closure(Closure<
		    FunctionSignature<return_t,first_closure_t, first_arg_t,Args_t...>,
		    closure_t...> closure,
		    first_closure_t first): Closure<
		    FunctionSignature<return_t,first_closure_t, first_arg_t, Args_t...>,
	closure_t...>(closure),first(first){}; 

      decltype(auto) bind(first_arg_t closed_arg) {
	return Closure<FunctionSignature<return_t, Args_t...>,
		       first_arg_t,
		       first_closure_t,
		       closure_t...>(*this, closed_arg);  
      }
      //      Closure<>
      return_t operator()(first_arg_t first, Args_t... args){
	return Closure<
	  FunctionSignature<return_t,first_closure_t, first_arg_t,Args_t...>,
	  closure_t...>::operator()(this->first,first,args... );
	
      };

    private:
      first_closure_t first;
    };


    // function_decayer decays lambdas to function pointers.
    // this only works with captureless lambdas.
    

    template<class ...T>
    struct ClosureMaker {
      template <class M>
      static Closure<FunctionSignature<T...>>  make(M m){
	return Closure<FunctionSignature<T...>>( m);
      };
    };
    
    template <class ...M>
    struct ClosureHolder;
    
    template<class return_t, class ...T, class ...M>
    struct ClosureHolder<FunctionSignature<return_t, T...>, M...>:ClosureBase<return_t, T...>{
    private:
      using closure_t = Closure<FunctionSignature<return_t,T...>, M...>;
    public:
      ClosureHolder(closure_t closure){
	std::memset( static_cast<void*>(&(this->closure)), 0,sizeof(closure_t));
	this->closure = closure;

      };
      return_t operator()(T...args ){
	return this->closure(args... );
      };
      MemCompareInfo get_mem_compare_info(){
	MemCompareInfo info{};
	info.obj = static_cast<void*>(&(this->closure) );
	info.size = sizeof(closure_t);
	return info;
      };
    private:
      closure_t closure;
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
