#ifndef _LDK_EVENT_H_
#define _LDK_EVENT_H_

namespace ldk
{
  struct KeyboardEvent
  {
    enum Type
    {
      KEYBOARD_KEY_DOWN,
      KEYBOARD_KEY_UP,
    };

    Type type;
    uint16 key;     // ldk key code
    uint16 text;    // textual character related to this key
    uint8 repeat;   // >0 if this is a repeat
    uint8 isControlDown;
    uint8 isAltDown;
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
  };

  struct MouseMoveEvent
  {
    uint32 x;
    uint32 y;
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
    KEYBOARD_EVENT = 1 << 0,
    MOUSE_BUTTON_EVENT = 1 << 1,
    MOUSE_MOVEMENT_EVENT = 1 << 2,
    VIEW_EVENT = 1 << 3,
    QUIT_EVENT = 1 << 4
  };

  struct Event
  {
    EventType type;
    union
    {
      KeyboardEvent keyboardEvent;
      MouseButtonEvent mouseButtonEvent;
      MouseMoveEvent mouseMoveEvent;
      ViewEvent viewEvent;
    };
  };
}

#endif //_LDK_EVENT_H_
