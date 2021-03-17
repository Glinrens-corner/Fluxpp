#ifndef FLUXPP_COMMAND_VISITOR_HPP
#define FLUXPP_COMMAND_VISITOR_HPP
//#include "widget_node.hpp"
#include "widget_fwd.hpp"
#include "backend/base_backend.hpp"

namespace fluxpp{
  class RenderTreeData;

  namespace visitors {
    using backend::DrawCommandBase;
    class CommandVisitor{
    public:
      
      CommandVisitor(
	  backend::AsynchronousBackendInterfaceBase* backend_aifc,
	  std::vector<std::unique_ptr<backend::DrawCommandBase>> * commands,
	  const RenderTreeData *  tree):
	tree_(tree),
	backend_aifc_(backend_aifc),
	commands_(commands){};
      
      void visit_screen(uuid_t screen_uuid);
      void visit_window(uuid_t parent_uuid, uuid_t window_uuid );
      
      void visit_widget(uuid_t parent_uuid, uuid_t widget_uuid );

      template<class T1, class T2>
      std::unique_ptr<DrawCommandBase> transform(
	  uuid_t parent_uuid,
	  uuid_t widget_uuid,
	  const widgets::Widget<T1, T2>& widget){
	return this->backend_aifc_->get_node_command(parent_uuid, widget_uuid);
      };
      
      std::unique_ptr<DrawCommandBase> transform(
	  uuid_t parent_uuid,
	  uuid_t widget_uuid,
	  const widgets::builtin::TextWidget& widget);
      
      std::unique_ptr<DrawCommandBase> transform(
	  uuid_t parent_uuid,
	  uuid_t widget_uuid,
	  const widgets::builtin::ColorWidget& widget);
      
    private:
      const RenderTreeData * tree_;
      backend::AsynchronousBackendInterfaceBase* backend_aifc_;
      std::vector<std::unique_ptr<backend::DrawCommandBase>>* commands_;
    };
    

  } // visitors
}// fluxpp



#endif //FLUXPP_COMMAND_VISITOR_HPP
