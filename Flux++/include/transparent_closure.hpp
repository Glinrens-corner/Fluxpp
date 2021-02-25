#ifndef TRANSPARENT_CLOSURE_HPP
#define TRANSPARENT_CLOSURE_HPP
#include <cstring>
#include <new>
#include <cstdlib>
#include <utility>
#include <type_traits>

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

/*
 *two versions of a Fun can be comp
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
  
  // Metaprogramming tests
  namespace transparent_closure{
    namespace test {
      // is tansparent  effectively alialises to true_type or false_type
      // this is different from check_transparency
      
      template<class T>
      struct is_transparent : std::false_type{ };
      
      
      template<>
      struct is_transparent<bool> :std::true_type{};
      template<>
      struct is_transparent<char> :std::true_type{};
      template<>
      struct is_transparent<unsigned char> :std::true_type{};
      template<>
      struct is_transparent<signed char> :std::true_type{};

      template<>
      struct is_transparent<int> :std::true_type{};
      template<>
      struct is_transparent<unsigned int> :std::true_type{};
      //      template<>
      //      struct is_transparent<signed int> :std::true_type{};

      template<>
      struct is_transparent<short int> :std::true_type{};
      template<>
      struct is_transparent<unsigned short int> :std::true_type{};
      //      template<>
      //      struct is_transparent<signed short int> :std::true_type{};
      
      template<>
      struct is_transparent<long int> :std::true_type{};
      template<>
      struct is_transparent<unsigned long int> :std::true_type{};
      //      template<>
      //      struct is_transparent<signed long int> :std::true_type{};


      template<>
      struct is_transparent<long long int> :std::true_type{};
      template<>
      struct is_transparent<unsigned long long int> :std::true_type{};
      //      template<>
      //      struct is_transparent<signed long long int> :std::true_type{};

      template<>
      struct is_transparent<float> :std::true_type{};
      template<>
      struct is_transparent<double> :std::true_type{};
      
      template<>
      struct is_transparent<long double> :std::true_type{};
      template<>
      struct is_transparent<wchar_t> :std::true_type{};
     
      

    }
      
    namespace error{
      class ignore{};
      template<class T, class enable=void>
      struct is_not_transparent{
      };
      
      template<class T >
      struct is_not_transparent<T, typename std::enable_if<test::is_transparent<T>::value, void>::type>{
	using type = std::false_type;
      };
      
    
      
    }

    namespace test{
      // check_transparent generates a compiltime error if T is not transparent
      template<class T>
      using check_transparency = typename  error::is_not_transparent<T>::type;

      

      
	
    }
  }

  //MemCompareInfo
  // ClosureBase
  // Fun
  namespace transparent_closure{
    struct  MemCompareInfo{
      const void* obj;
      std::size_t size;
    };

    template<class return_t , class ...Args_t>
    struct ClosureBase{
      virtual return_t operator()(Args_t... )const =0;
      virtual MemCompareInfo get_mem_compare_info()const=0;
      virtual ~ClosureBase(){};
    };
    
    template<class return_t, class ... Args_t>
    class Fun{
    private:
            using test_tuple_t = std::tuple<test::check_transparency<Args_t>...>; 
    public:
      Fun( ClosureBase<return_t, Args_t...>* closure ): closure(closure){};
      MemCompareInfo get_mem_compare_info() const {
	return this->closure->get_mem_compare_info();
      };
      
      return_t operator()(Args_t... args)const{
        return this->closure->operator()(args...);
      };
      
      ~Fun(){
	this->closure->~ClosureBase<return_t, Args_t...>();
	std::free(this->closure );
      };
    private:
      ClosureBase<return_t, Args_t...>* closure;
    };

  }

  // ClosureContainer
  namespace transparent_closure{
    template< class current_function_signature, class ...closed_t>
    class ClosureContainer;

    //BaseContainer (wraps only a function pointer)
    // the function has no arguments
    template<class return_t  >
    class ClosureContainer<
      FunctionSignature<return_t>>{
    public:
      explicit ClosureContainer(return_t(*fn)( ) ):fn(fn){};
      return_t operator( )( )const {
	return (*fn)( );
      }
      
    private:
      return_t (*fn)( );
    };

    //BaseContainer (wraps only a function pointer)
    // the function has at least one argument
    template<class return_t, class first_t, class ...Arg_t  >
    class ClosureContainer<
      FunctionSignature<return_t,first_t, Arg_t...>>{
    public:
      explicit ClosureContainer(return_t(*fn)(first_t, Arg_t... ) ):fn(fn){};
      return_t operator( )(first_t first, Arg_t...args )const {
	return (*fn)(first, args... );
      }

      
      decltype(auto) bind(first_t closed_arg) {
	return ClosureContainer<FunctionSignature<return_t, Arg_t...>,first_t>(*this, closed_arg);  
      }
    private:
      return_t (*fn)(first_t,Arg_t... );
    };



    // Closure
    // no arguments, at least one closed over argument
    template<
      class return_t,
      class first_closure_t,
      class ...closure_t >
    class ClosureContainer<
      FunctionSignature<return_t>,
      first_closure_t,
      closure_t...>: ClosureContainer<
      FunctionSignature<return_t,first_closure_t>,
      closure_t...>
    {
    public:
      ClosureContainer(ClosureContainer<
		       FunctionSignature<return_t,first_closure_t>,
		       closure_t...> closure,
		       first_closure_t first): ClosureContainer<
	FunctionSignature<return_t,first_closure_t>,
	closure_t...>(closure),first(first){}; 

      //      ClosureHolder<>
      return_t operator()()const{
	return ClosureContainer<
	  FunctionSignature<return_t,first_closure_t>,
	  closure_t...>::operator()(this->first );
	
      };

    private:
      first_closure_t first;
    };


    // Closure with at least one argument
    //    and at least one enclosed value
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

      return_t operator()(first_arg_t first, Args_t... args)const{
	return ClosureContainer<
	  FunctionSignature<return_t,first_closure_t, first_arg_t,Args_t...>,
	  closure_t...>::operator()(this->first,first,args... );
	
      };

    private:
      first_closure_t first;
    };

  };

  // ClosureHolder
  namespace transparent_closure{
    
    template <class ...M>
    struct ClosureHolder;
    
    template<class return_t, class ...T, class ...M>
    struct ClosureHolder<FunctionSignature<return_t, T...>, M...>:ClosureBase<return_t, T...>{
    private:
      using closure_container_t = ClosureContainer<FunctionSignature<return_t,T...>, M...>;
    public:
      explicit ClosureHolder(closure_container_t closure_container):closure_container(std::move(closure_container)){};
      
      return_t operator()(T...args )const{
	return this->closure_container(args... );
      };
      
      MemCompareInfo get_mem_compare_info()const{
	MemCompareInfo info{
	  .obj  = static_cast<const void*>(&(this->closure_container) ),
	    .size = sizeof(closure_container_t)
	};
	return info;
      };
    private:
      closure_container_t closure_container;
    };

  }
  
  //Closure
  namespace transparent_closure{
    
    template <class ...T>
    class Closure;

    template<class T>
    struct fitting_closure;

    template<class ...T>
    struct fitting_closure<ClosureContainer<T...>> {
      using type = Closure<T...>;
    };
    
    template <class return_t, class ...Args_t, class ...Closed_t >
    class Closure<FunctionSignature<return_t, Args_t...>,  Closed_t...>{
    private:
      using test_tuple_t = std::tuple<test::check_transparency<Args_t>...,test::check_transparency<Closed_t>...>; 
      using closure_container_t = ClosureContainer<FunctionSignature<return_t, Args_t...>,  Closed_t...>;
      using closure_holder_t = ClosureHolder<FunctionSignature<return_t, Args_t...>,  Closed_t...>;
    public:
      explicit Closure(closure_container_t closure_container) : closure_container( std::move(closure_container)){};
      
      return_t operator()(Args_t... args)const{
	return this->closure_container(args...);
      }

      template<class T, typename std::enable_if<(sizeof...(Args_t)>= 1), bool>::type=true>
      decltype(auto) bind(T arg){
	return typename fitting_closure<decltype(this->closure_container.bind(arg))>::type(this->closure_container.bind(arg));
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
  }
  
  //  helper functions and classes
  namespace transparent_closure{

    template<class return_t, class ...T>
    decltype(auto) closure_from_fp(return_t(*fp)(T... ) ){
      using test_t = std::tuple<test::check_transparency<T>...>;
      using signature_t = FunctionSignature<return_t, T...>;
      return Closure<signature_t>(ClosureContainer<signature_t>(fp));

    };
    
    template<class return_t, class ...T>
    struct ClosureMaker {
      using test_t = std::tuple<test::check_transparency<T>...>;
      template <class M>
      static Closure<FunctionSignature<return_t,T...>>  make(M m){
	// this gives horrible error messages.
	return_t (*fp)(T...) = m;
	return Closure<FunctionSignature<return_t, T...>>( ClosureContainer<FunctionSignature<return_t, T...>> (fp));
      }
    };
  }; // transparent_closure

  // compare 
  namespace transparent_closure {
    
    template<class Fun1_t, class Fun2_t>
    bool is_identical(Fun1_t& fun1, Fun2_t& fun2 ){
      MemCompareInfo info1 = fun1.get_mem_compare_info();
      MemCompareInfo info2 = fun2.get_mem_compare_info();
      if (info1.size != info2.size)return false;
      // ASSM: info1.size == info2.size
      if(std::memcmp(info1.obj,info2.obj, info2.size) !=0 ) return false;
      return true;
    };
    
    template<class Fun1_t, class Fun2_t>
    bool is_updated(Fun1_t& fun1, Fun2_t& fun2 ){
      return ! is_identical<Fun1_t, Fun2_t>(fun1, fun2 );
    };
    
    
  }; // transparent_closure
  
} // fluxpp

#endif //TRANSPARENT_CLOSURE_HPP
