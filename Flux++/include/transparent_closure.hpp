#ifndef TRANSPARENT_CLOSURE_HPP
#define TRANSPARENT_CLOSURE_HPP
#include <cstring>
#include <new>
#include <cstdlib>
#include <utility>
#include <type_traits>
#include <cassert>


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

// we simply assume that all scalar types are ok with being 64 byte aligned

#define MAX_SCALAR_ALIGNMENT 64

namespace fluxpp {

  // FunctionSignature
  namespace transparent_closure {
    namespace detail{
      
      class StackAllocator{

      public:
	StackAllocator( ):size(0),max_size(256){
	  this->stack_base = reinterpret_cast<char*>(std::aligned_alloc(MAX_SCALAR_ALIGNMENT,256));
	  if (!this->stack_base)throw std::bad_alloc();

	  
	};
	
	template<class T>
        void * get_new(){
	  static_assert( alignof(T)<MAX_SCALAR_ALIGNMENT, "unsupported alignment");
	  std::size_t old_size =  this->size;
	  std::size_t new_size =  this->size + this->calculate_size_increase<T>();
	  while (new_size > this->max_size) {
	    this->reallocate();
	  };
	  assert(this->max_size >= new_size);
	  this->size = new_size;
	  return reinterpret_cast<void*>(this->stack_base+old_size);
	}

	template<class T>
	T* get_last(){
	  assert(this->size >= this->calculate_size_increase<T>());
	  return reinterpret_cast<T*>( this->stack_base+ ( this->size - this->calculate_size_increase<T>()) );
	}
	
	template<class T>
	void pop_last(){
	  assert(this->size>= this->calculate_size_increase<T>());
	  this->size -= this->calculate_size_increase<T>();
	}

	~StackAllocator(){
	  std::free(this->stack_base);
	};

	
      private:
	template <class T>
	constexpr std::size_t calculate_size_increase() const{
	  return (sizeof(T)/MAX_SCALAR_ALIGNMENT+ (sizeof(T) %MAX_SCALAR_ALIGNMENT==0?0:1))*MAX_SCALAR_ALIGNMENT; 
	}

	void reallocate() {
	  std::size_t new_max_size = 2*this->max_size;
	  char *  new_base = reinterpret_cast<char*>(std::aligned_alloc(MAX_SCALAR_ALIGNMENT,new_max_size));
	  if (!new_base)throw std::bad_alloc();
	  std::memcpy(new_base, this->stack_base, this->max_size );
	  
	  std::free(this->stack_base);

	  this->stack_base = new_base ;
	  this->max_size = new_max_size;

	}

      public:// for testing
	constexpr static std::size_t get_init_max_size(){return 256; };
	std::size_t get_size()const{return this->size;};
	std::size_t get_max_size()const{return this->max_size;};
        char* get_stack_base()const{return this->stack_base;};
      private:
	char * stack_base;
	std::size_t size;
	std::size_t max_size;
      };
    };

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
      const void* next_obj;
      MemCompareInfo(*continuation_fn)(detail::StackAllocator&, const void*);
      const void* obj;
      std::size_t size;
    };

    using mem_compare_continuation_fn_t = decltype(MemCompareInfo::continuation_fn);
    
    struct ComparisonIteratorBase {
      const void * next_obj;
      mem_compare_continuation_fn_t  continuation_fn;
    };

    template<class return_t , class ...Args_t>
    struct ClosureBase{
      virtual return_t operator()(Args_t... )const =0;
      virtual MemCompareInfo get_mem_compare_info(const void* next_obj,
						  mem_compare_continuation_fn_t continuation,
						  detail::StackAllocator& stack)const=0;
      virtual ~ClosureBase(){};
    };
    
    template<class return_t, class ... Args_t>
    class Fun{
    private:
            using test_tuple_t = std::tuple<test::check_transparency<Args_t>...>; 
    public:
      Fun( ClosureBase<return_t, Args_t...>* closure ): closure(closure){};
      MemCompareInfo get_mem_compare_info(void* next_obj,
					  mem_compare_continuation_fn_t continuation,
					  detail::StackAllocator& stack) const {
	return this->closure->get_mem_compare_info(next_obj, continuation, stack);
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
      
      MemCompareInfo get_mem_compare_info(const void* next_obj,
					  mem_compare_continuation_fn_t continuation,
					  detail::StackAllocator& stack)const{
	// saving 
        new (stack.get_new<ComparisonIteratorBase>()) ComparisonIteratorBase{
	  .next_obj = next_obj,  .
	  continuation_fn = continuation};
	MemCompareInfo info{
	  .next_obj = static_cast<const void*>(this),
	    .continuation_fn = &ClosureHolder<FunctionSignature<return_t, T...>, M...>::continue_mem_compare_info,
	    .obj  = static_cast<const void*>(&(this->closure_container) ),
	    .size = sizeof(closure_container_t)
	};
	return info;
      };

      static MemCompareInfo continue_mem_compare_info(detail::StackAllocator& stack,
						      const void* obj){
	MemCompareInfo info{
	  .next_obj = stack.get_last<ComparisonIteratorBase>()->next_obj,
	    .continuation_fn = stack.get_last<ComparisonIteratorBase>()-> continuation_fn,
	    .obj  = nullptr,
	    .size =0
	};
	stack.pop_last<ComparisonIteratorBase>();
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
  }; 

  // compare 
  namespace transparent_closure {

    namespace detail {
      
      bool is_identical_object(MemCompareInfo& info1, MemCompareInfo& info2);
    }
    template<class Fun1_t, class Fun2_t>
    bool is_identical(Fun1_t& fun1, Fun2_t& fun2 ){
      constexpr auto is_null = [](const void* ptr)->bool {return ! ptr;}; 
      
      if (! std::is_same<Fun1_t, Fun2_t>::value ) return false;

      
      
      auto stack1 = detail::StackAllocator{};
      auto stack2 = detail::StackAllocator{};
      MemCompareInfo info1 = fun1.get_mem_compare_info(nullptr,nullptr,stack1);
      MemCompareInfo info2 = fun2.get_mem_compare_info(nullptr,nullptr,stack2);
 
      while (true) {
	if ( is_null( info1.next_obj) or is_null(info2.next_obj) ){
	  // next_obj ( and continuation_fn ) can only be null if
	  //   the highest level of the object tree has been handled and the the algorithm returns the pointer this function has above given it.
#ifndef NDEBUG
	  if (is_null(info1.next_obj)){
	    assert(stack1.get_size() == 0);
	    assert(! info1.continuation_fn);
	    
	  };
	  if (is_null(info2.next_obj)){
	    assert(stack2.get_size() == 0);
	    assert(!info2.continuation_fn);
	  };
#endif

	  if ( not ( is_null(info1.next_obj) and is_null(info2.next_obj))) return false;
	  return true;
	};
	
	if (is_null(info1.obj) or is_null(info2.obj) ) {
	  // the obj being null indicates, that a level has been handled and
	  // the next higher level needs to continue.
	  //   TODO theoretically the function which returns this info could themselves call the continuation.
	  // however I believe this way we get a somewhat more sensible stack trace... 
#ifndef NDEBUG
	  if (is_null(info1.obj)){
	    assert(info1.size == 0);
	  };
	  if (is_null(info2.next_obj)){
	    assert(info2.size == 0);
	  };
#endif
	  if ( not ( is_null(info1.obj) and is_null(info2.obj))) return false;
	  assert(  info1.continuation_fn);
	  assert(  info1.next_obj);  
	  info1 = info1.continuation_fn(stack1, info1.next_obj );
	  
	  assert(  info2.continuation_fn);
	  assert(  info2.next_obj);
	  info2 = info2.continuation_fn(stack2, info2.next_obj );
	}; 
	if (! detail::is_identical_object( info1,info2)) return false;
	assert(  info1.continuation_fn);
	assert(  info1.next_obj);  
	info1 = info1.continuation_fn(stack1, info1.next_obj );
	
	assert(  info2.continuation_fn);
	assert(  info2.next_obj);
	info2 = info2.continuation_fn(stack2, info2.next_obj );

      };
    }
    template<class Fun1_t, class Fun2_t>
    bool is_updated(Fun1_t& fun1, Fun2_t& fun2 ){
      return ! is_identical<Fun1_t, Fun2_t>(fun1, fun2 );
    };
    
    
  }; // transparent_closure
  
} // fluxpp

#endif //TRANSPARENT_CLOSURE_HPP
