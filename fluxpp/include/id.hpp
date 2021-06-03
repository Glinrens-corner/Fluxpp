#ifndef FLUXPP_ID_HPP
#define FLUXPP_ID_HPP

#include <type_traits>
#include <cstdint>
#include <functional>

namespace fluxpp{
  namespace id{
    using id_base_type = uint64_t;

    template<class tag_t>
    class SequentialIdGenerator;
    
    template <class tag_t>
    struct Id{
    private:
      friend class SequentialIdGenerator<tag_t>;
    public:
      using value_type = id_base_type;
      using tag_type = tag_t;
    private:
      Id(id_base_type value ):value_(value){}
    public:
      id_base_type value()const{return this->value_;};
    private:
      id_base_type value_;
    };

    template<class tag_t>
    bool operator< (Id<tag_t> left, Id<tag_t>right){
      return left.value()<right.value();
    };
    
    template<class tag_t>
    bool operator== (Id<tag_t>left, Id<tag_t>right){
      return left.value() == right.value();
    };


    template<class tag_t>
    bool operator!= (Id<tag_t>left, Id<tag_t>right){
      return left.value() != right.value();
    };

    
    template<class tag_t>
    class SequentialIdGenerator{
    public:
      Id<tag_t> get_id(){ return Id<tag_t>(this->current_++ );};
    private:
      id_base_type current_{0};
    };
    
  } // id
} //fluxpp

template<class tag_t>
struct std::hash<fluxpp::id::Id<tag_t>>{

  std::size_t operator()(fluxpp::id::Id<tag_t> id)const{
    return std::hash<typename fluxpp::id::Id<tag_t>::value_type>{}(id.value() );
  };
};

#endif // FLUXPP_ID_HPP
