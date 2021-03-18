
#include "command_visitor.hpp"
#include "render_data.hpp"
namespace fluxpp{
  namespace visitors {

    void CommandVisitor::visit_screen(uuid_t screen_uuid){
      ScreenNode const& screen = this->tree_->screen_at(screen_uuid);
      std::vector<uuid_t> children = screen.children();
      this->commands_->emplace_back(this->backend_aifc_->get_root_node_command(screen_uuid, std::move(children) ));
      for (uuid_t child_uuid :screen.children()){
	this->visit_window(screen_uuid, child_uuid);
      };
    }
    void CommandVisitor::visit_window(uuid_t screen_uuid, uuid_t window_uuid){
      WindowNode const& window = this->tree_->window_at(window_uuid);
      std::vector<uuid_t> children = window.children();
      this->commands_->emplace_back(this->backend_aifc_->get_window_node_command(screen_uuid, window_uuid, std::move(children) ));
      for (uuid_t child_uuid :window.children()){
	this->visit_widget(window_uuid, child_uuid);
      };


    }
    void CommandVisitor::visit_widget(uuid_t parent_uuid, uuid_t widget_uuid){
      WidgetNode const& widget = this->tree_->widget_at(widget_uuid);
      std::vector<uuid_t> children = widget.children();
      this->commands_->emplace_back(widget.widget().accept(parent_uuid, widget_uuid, std::move(children), *this));
      for (uuid_t child_uuid :widget.children()){
	this->visit_widget(widget_uuid, child_uuid);
      };
    }
    
    std::unique_ptr<backend::DrawCommandBase> CommandVisitor::transform(
	  uuid_t parent_uuid,
	  uuid_t widget_uuid,
	  const widgets::builtin::TextWidget& widget){
      return this->backend_aifc_->get_draw_text_command(parent_uuid, widget_uuid);
    }
    std::unique_ptr<backend::DrawCommandBase> CommandVisitor::transform(
	  uuid_t parent_uuid,
	  uuid_t widget_uuid,
	  const widgets::builtin::ColorWidget& widget){
      return this->backend_aifc_->get_draw_color_command(parent_uuid, widget_uuid);
    }
    
    
  };
};
