#include "event_system.hpp"
#include "error.hpp"
#include <iostream>
#include <cassert>

namespace fluxpp{


  namespace event_system{
    GlobalStringMapper::id_t GlobalStringMapper::map_string(std::string name){
      if (this->is_sealed_){
        throw fluxpp::error::sealed_access_exception();
      };
      {
        auto it =this->id_by_string_.find(name);
        if(it != this->id_by_string_.end()){
          return it->second;
        };
      };
      id_t new_id = this->generator_.get_id();
      auto [it, inserted] = this->string_by_id_
        .insert({new_id,
              std::move(name)});
      assert(inserted);
      std::string_view name_view{it->second} ;
      this->id_by_string_.insert({
          name_view,
            new_id});
      return new_id;
    };

    std::optional<GlobalStringMapper::id_t> GlobalStringMapper::id_by_string(const std::string&name)const{
      auto it =  this->id_by_string_.find(name);
      if ( it == this->id_by_string_.end()){
        return {};
      } else {
        return it->second;
      };
    };

    std::optional<std::reference_wrapper< const std::string>> GlobalStringMapper::string_by_id( GlobalStringMapper::id_t id)const{
      auto it = this->string_by_id_.find(id);
      if(it == this->string_by_id_.end()){
        return {};
      } else {
        return std::cref(it->second);
      };
    };

    LocalStringMapper GlobalStringMapper::create_local_mapper() const{
      this->is_sealed_ = true;
      return LocalStringMapper(this );
    };

    LocalStringMapperInterface LocalStringMapper::get_interface(){
      this->lock();
      return LocalStringMapperInterface(this);
    };
    
    ConstLocalStringMapperInterface LocalStringMapper::get_interface()const{
      this->lock();
      return ConstLocalStringMapperInterface(this);
    };

    auto  ConstLocalStringMapperInterface::id_by_string(const std::string& name)const->std::optional<id_t>{
      return this->mapper_->id_by_string(name);
    };

    auto  LocalStringMapperInterface::id_by_string(const std::string& name)const->std::optional<id_t>{
      return this->mapper_->id_by_string(name);
    };


    std::optional<std::reference_wrapper<const std::string>> ConstLocalStringMapperInterface::string_by_id(id_t id)const{
      return this->mapper_->string_by_id(id);
    };

    std::optional<std::reference_wrapper<const std::string>> LocalStringMapperInterface::string_by_id(id_t id)const{
      return this->mapper_->string_by_id(id);
    };

    LocalStringMapperInterface::id_t LocalStringMapperInterface::map_string( std::string name){
      return this->mapper_->map_string( name);
    };

    auto  LocalStringMapper::id_by_string(const std::string& name)const->std::optional<id_t>{
      auto global_val = this->global_mapper_->id_by_string(name);
      if(global_val ){
        return global_val;
      } else {
        auto it = this->id_by_string_.find(name);
        if(it== this->id_by_string_.end()){
          return {};
        } else {
          return it->second;
        };
      };
    };
    
    std::optional<std::reference_wrapper<const std::string>> LocalStringMapper::string_by_id(id_t id )const{
      auto global_val = this->global_mapper_->string_by_id(id);
      if(global_val){
        return global_val;
      }else {
        auto it = this->string_by_id_.find(id);
        if(it == this->string_by_id_.end()){
          return {};
        } else {
          return std::cref(it->second);
        };
      };
    };

    auto LocalStringMapper::map_string(std::string name )->id_t{
      auto preset_val = this->id_by_string( name);
      if (preset_val.has_value()){
        return preset_val.value();
      }else {
        return this->map_string_core(name);
      };
    };
    
     auto LocalStringMapper::map_string_core(std::string name)->id_t{
      id_t new_id = this->generator_.get_id();
      auto [it, inserted] = this->string_by_id_
        .insert({new_id,
              std::move(name)});
      assert(inserted);
      std::string_view name_view{it->second} ;
      this->id_by_string_.insert({
          name_view,
            new_id});
      return new_id;
    };
  }// event_system
}// fluxpp
