#ifndef FLUXPP_EVENT_SYSTEM_HPP
#define FLUXPP_EVENT_SYSTEM_HPP
#include <map>
#include <string>
#include <string_view>
#include <optional>
#include <functional>
#include <cassert>
#include <mutex>
#include "id.hpp"


namespace fluxpp {
  namespace event_system{
    struct StringIdTag{};

    class LocalStringMapper;
    
    class GlobalStringMapper{
    private:
      friend LocalStringMapper;
    public:
      using id_t = fluxpp::id::Id<StringIdTag>;
    private:
      GlobalStringMapper(){};
    public:
      GlobalStringMapper(const GlobalStringMapper &) = delete;
      void operator=(const GlobalStringMapper&) = delete; 
    public:
      static GlobalStringMapper& get_instance(){
        static GlobalStringMapper instance;
        return instance;
      };
      
      LocalStringMapper create_local_mapper( )const;
      
    public:
      id_t map_string(std::string name);
      bool is_sealed()const{return this->is_sealed_;};
    private: // only the local mapper has access to these.
      std::optional<id_t> id_by_string(const std::string& name)const;
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t id )const;
    private:
      fluxpp::id::SequentialIdGenerator<StringIdTag> generator_{};
      std::map<id_t, std::string> string_by_id_{};
      std::map<std::string_view, id_t> id_by_string_{};
      mutable bool is_sealed_ = false;
    };

    class ConstLocalStringMapperInterface;
    class LocalStringMapperInterface;
    
    class LocalStringMapper{
    private:
      friend class ConstLocalStringMapperInterface;
      friend class LocalStringMapperInterface;
    public:
      using id_t = fluxpp::id::Id<StringIdTag>;
    public:
      LocalStringMapper(const GlobalStringMapper* global_mapper)
        :global_mapper_(global_mapper)
        ,generator_(global_mapper->generator_){
        assert(this->global_mapper_->is_sealed());
      };
      
      LocalStringMapperInterface get_interface(); 

      ConstLocalStringMapperInterface get_interface()const;
    private:
      std::optional<id_t> id_by_string(const std::string& )const;
      
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t)const;

      id_t map_string( std::string);

      
    private:
      id_t map_string_core( std::string);

      void lock()const{this->lock_.lock(); };
      
      void unlock()const{this->lock_.unlock(); };
      
    private:
      const GlobalStringMapper* global_mapper_; 
      fluxpp::id::SequentialIdGenerator<StringIdTag> generator_{};
      std::map<id_t, std::string> string_by_id_{};
      std::map<std::string_view, id_t> id_by_string_{};
      mutable std::mutex  lock_{};
    };

    class LocalStringMapperInterface{
    private:
      friend class LocalStringMapper;
    public:
      using id_t = LocalStringMapper::id_t;
    private:
      LocalStringMapperInterface(LocalStringMapper* mapper):mapper_(mapper){};
    public:
      LocalStringMapperInterface():mapper_(nullptr){};

      LocalStringMapperInterface(const LocalStringMapperInterface& ) = delete;

      LocalStringMapperInterface(LocalStringMapperInterface&& other ){
        std::swap(this->mapper_, other.mapper_);
      };
      
      LocalStringMapperInterface& operator=(const LocalStringMapperInterface& ) = delete;
      
      LocalStringMapperInterface& operator=(LocalStringMapperInterface&& other){
        std::swap(this->mapper_, other.mapper_);
      };
      
      ~LocalStringMapperInterface(){
        if(this->mapper_){
          this->mapper_->unlock();
        };
      };
    public:
      std::optional<id_t> id_by_string(const std::string& )const;
      
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t)const;

      id_t map_string(std::string);
      
    private:
      LocalStringMapper * mapper_;
    };
    
    class ConstLocalStringMapperInterface{
    private:
      friend class LocalStringMapper;
    public:
      using id_t = LocalStringMapper::id_t;
    private:
      ConstLocalStringMapperInterface(const LocalStringMapper* mapper):mapper_(mapper){};
    public:
      
      ConstLocalStringMapperInterface():mapper_(nullptr){};
      
      ConstLocalStringMapperInterface(const ConstLocalStringMapperInterface& ) = delete;
      
      ConstLocalStringMapperInterface(ConstLocalStringMapperInterface&& other ){
        std::swap(this->mapper_, other.mapper_);
      };
      
      ConstLocalStringMapperInterface& operator=(const ConstLocalStringMapperInterface& ) = delete;
      
      ConstLocalStringMapperInterface& operator=(ConstLocalStringMapperInterface&& other){
        std::swap(this->mapper_, other.mapper_);
      };
      
      ~ConstLocalStringMapperInterface(){
        if(this->mapper_){
          this->mapper_->unlock();
        };
      };
    public:
      std::optional<id_t> id_by_string(const std::string& )const;
      
      std::optional<std::reference_wrapper<const std::string>> string_by_id(id_t)const;
           
    private:
      const LocalStringMapper * mapper_;
    };

  } // event_system

} // fluxpp






#endif // FLUXPP_EVENT_SYSTEM_HPP
