#ifndef LAMBDA_HPP
#define LAMBDA_HPP

#include <tuple>

namespace fluxpp {


  namespace lambda {
    using std::tuple;

    template <class return_t, class ...argument_t>
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
    
    template< class current_function_signature, class ...closed_t>
    class ClosureHolder;
    
    template<class return_t, class ...Arg_t  >
    class ClosureHolder<
      FunctionSignature<return_t,Arg_t...>>{
    public:
      ClosureHolder(return_t(*fn)(Arg_t... ) ):fn(fn){};
      return_t operator( )(Arg_t...args ){
	return (*fn)(args... );

      }
    private:
      return_t (*fn)(Arg_t... );
    };


    template<class return_t, class first_t, class ...Arg_t  >
    class ClosureHolder<
      FunctionSignature<return_t,first_t, Arg_t...>>{
    public:
      ClosureHolder(return_t(*fn)(first_t, Arg_t... ) ):fn(fn){};
      return_t operator( )(first_t first, Arg_t...args ){
	return (*fn)(first, args... );
      }

      decltype(auto) bind(first_t closed_arg) {
	return ClosureHolder<FunctionSignature<return_t, Arg_t...>,first_t>(*this, closed_arg);  
      }
    private:
      return_t (*fn)(first_t,Arg_t... );
    };



    
    template<
      class return_t,
      class first_closure_t,
      class ...Args,
      class ...closure_t >
    class ClosureHolder<
      FunctionSignature<return_t,Args...>,
      first_closure_t,
      closure_t...>: ClosureHolder<
      FunctionSignature<return_t,first_closure_t, Args...>,
      closure_t...>
    {
    public:
      ClosureHolder(ClosureHolder<
		    FunctionSignature<return_t,first_closure_t, Args...>,
		    closure_t...> closure,
		    first_closure_t first): ClosureHolder<
		    FunctionSignature<return_t,first_closure_t, Args...>,
	closure_t...>(closure),first(first){}; 

      //      ClosureHolder<>
      return_t operator()(Args... args){
	return ClosureHolder<
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
    class ClosureHolder<
      FunctionSignature<return_t, first_arg_t,Args_t...>,
      first_closure_t,
      closure_t...>: ClosureHolder<
      FunctionSignature<return_t,first_closure_t,first_arg_t, Args_t...>,
      closure_t...>
    {
    public:
      ClosureHolder(ClosureHolder<
		    FunctionSignature<return_t,first_closure_t, first_arg_t,Args_t...>,
		    closure_t...> closure,
		    first_closure_t first): ClosureHolder<
		    FunctionSignature<return_t,first_closure_t, first_arg_t, Args_t...>,
	closure_t...>(closure),first(first){}; 

      decltype(auto) bind(first_arg_t closed_arg) {
	return ClosureHolder<FunctionSignature<return_t, Args_t...>,
			     first_arg_t,
			     first_closure_t,
			     closure_t...>(*this, closed_arg);  
      }
      //      ClosureHolder<>
      return_t operator()(first_arg_t first, Args_t... args){
	return ClosureHolder<
	  FunctionSignature<return_t,first_closure_t, first_arg_t,Args_t...>,
	  closure_t...>::operator()(this->first,first,args... );
	
      };

    private:
      first_closure_t first;
    };


    
    
    
    template <class Ret, class ...T>
    class BaseFun {
      //      virtual Ret operator ()(T...)=0;
    };

    
    template <class T>
    struct get_base_fun;

    template <class return_t,
	      class ...Args_t,
	      class ...Closures_t>
    struct get_base_fun<ClosureHolder<FunctionSignature< return_t, Args_t...>, Closures_t... >>{
      using type = BaseFun<return_t,Args_t...>;
    };

    template<class closure_holder_t>
    class Fun:  get_base_fun<closure_holder_t>::type{
    public:
      Fun(closure_holder_t closure) : closure(closure){}

    private:
      closure_holder_t closure;
    };

    template<class return_t,class ...Args_t>
    decltype(auto)
    make_fun( return_t(*fn)(Args_t...)) {
      using closure_holder_t =  ClosureHolder<FunctionSignature<return_t, Args_t...>>;
      return new Fun<closure_holder_t>(closure_holder_t(fn));
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
