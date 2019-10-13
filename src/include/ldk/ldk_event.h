#ifndef _LDK_EVENT_H_
#define _LDK_EVENT_H_

namespace ldk
{
  struct TextInputEvent
  {
    uint16 key;     // ldk key code
    uint16 text;    // textual character related to this key
    uint8 repeat;   // > 0 if this is a repeat
    uint8 isControlDown;
    uint8 isShiftDown;
  };

  struct KeyboardEvent
  {
    enum Type
    { KEYBOARD_KEY_DOWN,
      KEYBOARD_KEY_HOLD,
      KEYBOARD_KEY_UP,
    };

    Type type;
    uint16 key;     // ldk key code
    uint8 repeat;   // > 0 if this is a repeat
    uint8 isControlDown;
    uint8 isShiftDown;
  };

  struct MouseButtonEvent
  {
    enum Type
    {
      MOUSE_BUTTON_DOWN,
      MOUSE_BUTTON_UP
    };

    Type type;
    uint16 button;
    uint16 repeat;
    uint8 isControlDown;
    uint8 isShiftDown;
  };

  struct MouseMoveEvent
  {
    uint32 x;
    uint32 y;
  };

  struct MouseWheelEvent 
  {
    int32 delta;
    uint8 isControlDown;
    uint8 isShiftDown;
  };

  struct ViewEvent
  {
    enum Type
    {
      VIEW_RESIZED,
      VIEW_MINIMIZED,
      VIEW_MAXIMIZED,
      VIEW_FOCUS_LOST,
      VIEW_FOCUS_GAINED,
    };
   
    Type type;
    uint32 x;
    uint32 y;
    uint32 width;
    uint32 height;
  };

  enum EventType
  {
    TEXT_INPUT_EVENT,
    KEYBOARD_EVENT,
    KEYBOARD_TEXT_EVENT,
    MOUSE_BUTTON_EVENT,
    MOUSE_MOVE_EVENT,
    MOUSE_WHEEL_EVENT,
    VIEW_EVENT,
    QUIT_EVENT
  };

  struct Event
  {
    EventType type;
    union
    {
      TextInputEvent textInputEvent;
      KeyboardEvent keyboardEvent;
      MouseButtonEvent mouseButtonEvent;
      MouseMoveEvent mouseMoveEvent;
      MouseWheelEvent mouseWheelEvent;
      ViewEvent viewEvent;
    };
  };
}

#endif //_LDK_EVENT_H_
