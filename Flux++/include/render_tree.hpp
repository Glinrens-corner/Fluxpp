#ifndef FLUXPP_RENDER_TREE_HPP
#define FLUXPP_RENDER_TREE_HPP
#import <queue>
#import "widget.hpp"

namespace fluxpp{
  //   class WidgetNode;
  
  class RenderTree{
  public:
    RenderTree(
	std::unique_ptr<widgets::application::ApplicationBase>,
	std::queue<AppEvent>*,
	backend::BaseBackend*,
	state::State*);
    RenderTree(const RenderTree &) = delete;
    void prepare_render(bool render_all);
    ~RenderTree();
  private:
    class RenderTreeImpl;
    RenderTreeImpl* impl;
  };
}// fluxpp
#endif //FLUXPP_RENDER_TREE_HPP
