#ifndef FLUXPP_GUI_EVENT_HPP
#define FLUXPP_GUI_EVENT_HPP
#include <cstdint>

namespace fluxpp{
  namespace events{
    namespace detail{
      struct NativeConverter;
    }

    enum KeyMask : uint16_t{
      Shift  = (1L<<0 ),
      Lock   = (1L<<1 ),
      Control= (1L<<2 ),
      Mod1   = (1L<<3 ),
      Mod2   = (1L<<4 ),
      Mod3   = (1L<<5 ),
      Mod4   = (1L<<6 ),
      Mod5   = (1L<<7 )
    };
      
    enum ButtonMask:uint16_t{
      button_1=(1L<<8),
      button_2=(1L<<9),
      button_3=(1L<<10),
      button_4=(1L<<11),
      button_5=(1L<<12),
      button_any = (1L<<8)+(1L<<9)+(1L<<10)+(1L<<11)+(1L<<12)
    };

    using KeyOrButtonMask = uint16_t;

    
    enum class EventType{
      key_press,
      key_release,
      button_press,
      button_release,
      motion_notify,
      enter_notify,
      leave_notify,
      focus_in,
      focus_out,
      keymap_notify
    };

    struct Coordinate{      
    public:
      int16_t x =0;
      int16_t y =0;
    };

    template <EventType event_type_>
    class EventBase {
    public:
      EventBase( uint16_t sequence ):
	sequence_(sequence){}; 
      EventType event_type()const{return event_type_; };
      uint64_t sequence()const{return this->sequence_; };
      // time()
    protected:
      uint64_t sequence_;
      // time_
    };
    

    template <EventType event_type_>
    class KeyEvent_:public EventBase<event_type_>{
      friend detail::NativeConverter;
    public:
      Coordinate event_coordinate()const{return this->event_coordinate_ ;};
      //      uint32_t symbol(); //  TODO, what about emoji keyboards? they can have more modifiers.
      uint8_t key_code()const{return this->key_code_; };
      KeyOrButtonMask state()const{return this->state_; };
      bool same_screen()const{return this->same_screen_;};
    private:
      Coordinate event_coordinate_;
      KeyOrButtonMask state_;
      uint8_t key_code_;
      bool same_screen_;
      void * impl_ ;
    };

    using KeyPressEvent= KeyEvent_<EventType::key_press>;
    using KeyReleaseEvent= KeyEvent_<EventType::key_release>;

    
    template <EventType event_type_>
    class ButtonEvent_:public EventBase<event_type_>{
      friend detail::NativeConverter;
    public:
      Coordinate event_coordinate()const{return this->event_coordinate_ ;};
      //      uint32_t symbol(); //  TODO, what about emoji keyboards? they can have more modifiers.
      uint8_t button()const{return this->button_; };
      KeyOrButtonMask state()const{return this->state_; };
      bool same_screen()const{return this->same_screen_;};
    private:
      Coordinate event_coordinate_;
      KeyOrButtonMask state_;
      uint8_t button_;
      bool same_screen_;
      void * impl_ ;
    };
    
    using  ButtonPressEvent=ButtonEvent_<EventType::button_press>;
    using  ButtonReleaseEvent=ButtonEvent_<EventType::button_release>;


    class MotionNotifyEvent: public EventBase<EventType::motion_notify>{
      friend detail::NativeConverter;
    public:
      Coordinate event_coordinate()const{return this->event_coordinate_ ;};
      //      uint32_t symbol(); //  TODO, what about emoji keyboards? they can have more modifiers.
      uint8_t button()const{return this->button_; };
      KeyOrButtonMask state()const{return this->state_; };
      bool same_screen()const{return this->same_screen_;};
    private:
      Coordinate event_coordinate_;
      KeyOrButtonMask state_;
      uint8_t button_;
      bool same_screen_;
      void * data_ ;
    };

    template <EventType event_type_>
    class PointerEvent_ : EventBase<event_type_>{
      Coordinate event_coordinate()const{return this->event_coordinate_ ;};
      bool same_screen()const{return this->same_screen_;};
      KeyOrButtonMask state()const{return this->state_; };
      bool focus()const {return this->focus ;};
    private:
      Coordinate event_coordinate_;
      KeyOrButtonMask state_;
      bool focus_;
      bool same_screen_;
      void * data ;
    };
    
    using EnterNotifyEvent = PointerEvent_<EventType::enter_notify>;
    using LeaveNotifyEvent = PointerEvent_<EventType::leave_notify>;

    template <EventType event_type_>
    class InputFocusEvent_: public EventBase<event_type_>{
    public:
    private:
      void * data;
    };
    
    using FocusInEvent = InputFocusEvent_<EventType::focus_in>;
    using FocusOutEvent = InputFocusEvent_<EventType::focus_out>;

  } // events
}// fluxpp


#endif //FLUXPP_GUI_EVENT_HPP
