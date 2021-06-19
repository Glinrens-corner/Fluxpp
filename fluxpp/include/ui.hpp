#ifndef FLUXPP_UI_HPP
#define FLUXPP_UI_HPP
#include <map>
#include "abstract_backend.hpp"
#include "event_system.hpp"
#include "bootstrap_ui.hpp"
namespace fluxpp{

  class UI{
  private:
    using backend_t = fluxpp::backend::AbstractBackend;
    using bootstrap_ui_t = fluxpp::bootstrap_ui::BootstrapUi;
    using local_string_mapper_t = fluxpp::event_system::LocalStringMapper;
    using global_string_mapper_t = fluxpp::event_system::GlobalStringMapper;
    using event_system_t = fluxpp::event_system::EventSystem;
    
  public:
    template<class component_t>
    static UI create(
        const std::unordered_map<global_string_mapper_t::id_t , backend_t*>& backends,
        bootstrap_ui_t *,
        component_t );
  private:
    std::unique_ptr<local_string_mapper_t> local_string_mapper_;
    std::unique_ptr<event_system_t> event_system_;
    backend_t * backend_;
  };

  template<class component_t >
  inline UI UI::create(
      const std::unordered_map<global_string_mapper_t::id_t , backend_t*>& backends,
      bootstrap_ui_t * bootstrap_ui_t,
      component_t base_component){
    using namespace ranges;
    auto local_string_mapper =
      std::make_unique<local_string_mapper_t>(
      global_string_mapper_t
      ::get_instance()
      .create_local_mapper());
    using id_t = global_string_mapper_t::id_t;
    auto trf_range = backends | transform([](const std::pair<id_t, backend_t*>&  input)->id_t{return input.first; });;
    auto event_system = std::unique_ptr<event_system_t>(
        event_system_t::create(std::vector<global_string_mapper_t::id_t>(trf_range.begin(),trf_range.end()  ), local_string_mapper.get())
    );
    

  };
} //fluxpp

#endif // FLUXPP_UI_HPP
