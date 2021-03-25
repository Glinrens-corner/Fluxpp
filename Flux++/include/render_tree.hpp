#ifndef FLUXPP_RENDER_TREE_HPP
#define FLUXPP_RENDER_TREE_HPP
#import <queue>
#import "gui_event.hpp"
#import "widget.hpp"

namespace fluxpp{
  class RenderTreeImpl;
  
  class SynchronousRenderTreeInterface{
  public:
    explicit SynchronousRenderTreeInterface(RenderTreeImpl * impl):impl(impl){};
    SynchronousRenderTreeInterface(const SynchronousRenderTreeInterface& ) = delete;
    SynchronousRenderTreeInterface(SynchronousRenderTreeInterface&& old):impl(old.impl){old.impl=nullptr;};
    SynchronousRenderTreeInterface& operator=(const SynchronousRenderTreeInterface& ) = delete;
    SynchronousRenderTreeInterface& operator=(SynchronousRenderTreeInterface&& old){
      auto tmp = old.impl;
      old.impl = this->impl;
      this->impl = tmp;
      return *this;
    };
    ~SynchronousRenderTreeInterface();
  public:
    std::vector<std::unique_ptr<backend::DrawCommandBase> >extract_draw_commands();
    void dispatch_event(events::ButtonPressEvent event );
  private:
    RenderTreeImpl* impl;
  };
  
  class RenderTree{
  public:
    RenderTree(
	std::unique_ptr<widgets::application::ApplicationBase>,
	std::queue<AppEvent>*,
	backend::BaseBackend*,
	state::State*);
    RenderTree(const RenderTree &) = delete;
    SynchronousRenderTreeInterface get_synchronous_interface();
    
    ~RenderTree();
  private:
    RenderTreeImpl* impl;
  };
}// fluxpp
#endif //FLUXPP_RENDER_TREE_HPP
