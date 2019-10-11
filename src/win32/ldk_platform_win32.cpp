#ifndef _LDK_ENGINE_
#define _LDK_ENGINE_
#endif // _LDK_ENGINE_

// Win32 specifics
#include "tchar.h"
#include <windowsx.h>
#include <windows.h>
#include <winuser.h>
#include <tchar.h>
#include <objbase.h>

#include "ldk_xinput_win32.h"
#include "ldk_xaudio2_win32.h"

// undefine annoying leftover macros from windows.h
#ifdef near
#undef near
#endif

#ifdef far
#undef far
#endif

#define LDK_MAX_AUDIO_BUFFER 16
#define LDK_WINDOW_CLASS "LDK_WINDOW_CLASS"
#define LDK_WINDOW_STYLE WS_OVERLAPPEDWINDOW

#include <ldk/gl/glcorearb.h>
#include <ldk/gl/wglext.h>

PFNGLGETSTRINGPROC glGetString;
PFNGLENABLEPROC glEnable;
PFNGLDISABLEPROC glDisable;
PFNGLCLEARPROC glClear;
PFNGLCLEARCOLORPROC glClearColor;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
PFNGLGENBUFFERSPROC glGenBuffers;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLBUFFERSUBDATAPROC glBufferSubData;
PFNGLBINDATTRIBLOCATIONPROC glBindAttribLocation;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLGETERRORPROC glGetError;
PFNGLGETPROGRAMIVPROC glGetProgramiv;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLCREATESHADERPROC glCreateShader;
PFNGLSHADERSOURCEPROC glShaderSource;
PFNGLCOMPILESHADERPROC glCompileShader;
PFNGLCREATEPROGRAMPROC glCreateProgram;
PFNGLATTACHSHADERPROC glAttachShader;
PFNGLLINKPROGRAMPROC glLinkProgram;
PFNGLDELETESHADERPROC glDeleteShader;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLBUFFERDATAPROC glBufferData;
PFNGLMAPBUFFERPROC glMapBuffer;
PFNGLUNMAPBUFFERPROC glUnmapBuffer;
PFNGLDRAWELEMENTSPROC glDrawElements;
PFNGLUSEPROGRAMPROC glUseProgram;
PFNGLFLUSHPROC glFlush;
PFNGLVIEWPORTPROC glViewport;
PFNGLGENTEXTURESPROC glGenTextures;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLBINDTEXTUREPROC glBindTexture;
PFNGLTEXPARAMETERFPROC glTexParameteri;
PFNGLTEXIMAGE2DPROC glTexImage2D;
PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
PFNGLBINDBUFFERBASEPROC glBindBufferBase;
PFNGLGETUNIFORMBLOCKINDEXPROC glGetUniformBlockIndex;
PFNGLSCISSORPROC glScissor;
PFNGLDEPTHFUNCPROC glDepthFunc;
PFNGLBLENDFUNCPROC glBlendFunc;
PFNGLDEPTHMASKPROC glDepthMask;
PFNGLPOLYGONMODEPROC glPolygonMode;
PFNGLPOLYGONOFFSETPROC glPolygonOffset;
PFNGLLINEWIDTHPROC glLineWidth;
PFNGLUNIFORMBLOCKBINDINGPROC glUniformBlockBinding;
PFNGLCULLFACEPROC glCullFace;
PFNGLBLENDCOLORPROC glBlendColor;
PFNGLGETACTIVEUNIFORMPROC glGetActiveUniform;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation;
PFNGLDETACHSHADERPROC glDetachShader;
PFNGLGETACTIVEATTRIBPROC glGetActiveAttrib;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLFENCESYNCPROC glFenceSync;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
PFNGLDELETESYNCPROC glDeleteSync;
PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
PFNGLACTIVETEXTUREPROC glActiveTexture;
PFNGLDRAWARRAYSPROC glDrawArrays;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM2IPROC glUniform2i;
PFNGLUNIFORM3IPROC glUniform3i;
PFNGLUNIFORM4IPROC glUniform4i;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM2FPROC glUniform2f;
PFNGLUNIFORM3FPROC glUniform3f;
PFNGLUNIFORM4FPROC glUniform4f;

//TODO: Use a custom container with custom memory allocation
#include <map>

namespace ldk 
{
  namespace platform 
  {
    static struct LDKWin32
    {
      KeyboardState					keyboardState;
      MouseState						mouseState;
      JoystickState					gamepadState[LDK_MAX_JOYSTICKS];

      //TODO: add window data to the win32 window instance so there is no need for this map
      std::map<HWND, LDKWindow*> 	windowList;
      BoundAudio boundBufferList[LDK_MAX_AUDIO_BUFFER];
      uint32 boundBufferCount = 0;

      // timer data
      LARGE_INTEGER ticksPerSecond;
      LARGE_INTEGER ticksSinceEngineStartup;
    } _platform;

    struct SharedLib
    {
      HMODULE handle;
    };

    /* platform specific window */
    struct LDKWindow
    {
      LDKPlatformEventHandleFunc eventCallback;
      HINSTANCE hInstance;
      HWND hwnd;
      HDC dc;
      HGLRC rc;
      bool closeFlag;
      bool fullscreenFlag;
      LONG defaultStyle;
      RECT defaultRect;
      RECT clientRect;
      ldk::Event event;    // current event;
    };

    static HINSTANCE _appInstance;

    LDKWindow* findWindowByHandle(HWND hwnd) 
    {
      return _platform.windowList[hwnd];
    }

    static bool dummyEventHandler(LDKWindow* window, const ldk::Event* event)
    {
      return false;
    }

    static void ldk_win32_calculateClientRect(LDKWindow* window)
    {
      // Retrieve the screen coordinates of the client area, 
      // and convert them into client coordinates. 

      GetClientRect(window->hwnd, &window->clientRect); 
      RECT& clientRect = window->clientRect; 

      // Retrieve the screen coordinates of the client area, 
      // and convert them into client coordinates. 
      POINT upperLeft = {clientRect.left, clientRect.top};
      POINT bottomRight = {clientRect.right + 1, clientRect.bottom + 1};
      ClientToScreen(window->hwnd, &upperLeft); 
      ClientToScreen(window->hwnd, &bottomRight); 
      window->clientRect = { upperLeft.x, upperLeft.y, bottomRight.x, bottomRight.y};
    }

    LRESULT CALLBACK ldk_win32_windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
      switch(uMsg)
      {
        case WM_ACTIVATE:
          {
            break;
            LDKWindow* window = findWindowByHandle(hwnd);
            if(window)
            {
              // reset modifier key state if the window get inactive
              if (LOWORD(wParam) == WA_INACTIVE)
              {
                window->event.viewEvent.type = ldk::ViewEvent::VIEW_FOCUS_LOST;
              }
              else
              {
                window->event.viewEvent.type = ldk::ViewEvent::VIEW_FOCUS_GAINED;
              }

              window->event.type = ldk::EventType::VIEW_EVENT;
              window->eventCallback(window, (const ldk::Event*) &window->event);
            }
          }
          break;

        case WM_CLOSE:
          {
            LDKWindow* window = findWindowByHandle(hwnd);
            window->event.type = ldk::EventType::QUIT_EVENT;
            ldk::platform::setWindowCloseFlag(window, true);
          }

        case WM_SIZE:
          {
            LDKWindow* window = findWindowByHandle(hwnd);
            if(window)
            {
              ldk_win32_calculateClientRect(window);
              window->event.type = ldk::EventType::VIEW_EVENT;
              window->event.viewEvent.width = LOWORD(lParam);
              window->event.viewEvent.height = HIWORD(lParam);
              window->event.viewEvent.x = window->clientRect.left;
              window->event.viewEvent.y = window->clientRect.top;
              window->eventCallback(window, (const ldk::Event*) &window->event);
            }
          }
          break;

        default:
          return DefWindowProc(hwnd, uMsg, wParam, lParam);	
          break;
      }
      return true;
    }

    static bool ldk_win32_makeContextCurrent(ldk::platform::LDKWindow* window)
    {
      //wglMakeCurrent(window->dc, NULL);
      if (!wglMakeCurrent(window->dc, window->rc))
      {
        LogError("Could not make render context current for window");
        return false;
      }

      return true;
    }

    static bool ldk_win32_registerWindowClass(HINSTANCE hInstance)
    {
      WNDCLASS windowClass = {};
      windowClass.style = CS_OWNDC;
      windowClass.lpfnWndProc = ldk_win32_windowProc;
      windowClass.hInstance = hInstance;
      windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
      windowClass.lpszClassName = LDK_WINDOW_CLASS;
      return RegisterClass(&windowClass) != 0;
    }

    static bool ldk_win32_createWindow(
        ldk::platform::LDKWindow* window,
        DWORD style,
        uint32 width,
        uint32 height,
        HINSTANCE hInstance,
        TCHAR* title)
    {

      window->hwnd = CreateWindowEx(NULL, 
          TEXT(LDK_WINDOW_CLASS),
          title,
          style,
          CW_USEDEFAULT,
          CW_USEDEFAULT,
          width,
          height,
          NULL,
          NULL,
          hInstance,
          NULL);

      if (!window->hwnd)
        return false;

      window->dc = GetDC(window->hwnd);

      ldk_win32_calculateClientRect(window);

      return window->dc != NULL;
    }

    static void* ldk_win32_getGlFunctionPointer(const char* functionName)
    {
      static HMODULE opengl32dll = GetModuleHandleA("OpenGL32.dll");
      void* functionPtr = wglGetProcAddress(functionName);
      if( functionPtr == (void*)0x1 || functionPtr == (void*) 0x02 ||
          functionPtr == (void*) 0x3 || functionPtr == (void*) -1 ||
          functionPtr == (void*) 0x0)
      {
        functionPtr = GetProcAddress(opengl32dll, functionName);
        if(!functionPtr)
        {
          LogError("Could not get GL function pointer");
          LogError(functionName);
          return nullptr;
        }
      }

      return functionPtr;
    }

    static bool ldk_win32_initOpenGL(ldk::platform::LDKWindow& gameWindow, HINSTANCE hInstance, int major, int minor, uint32 colorBits = 32, uint32 depthBits = 24)
    {
      ldk::platform::LDKWindow dummyWindow = {};
      if (! ldk_win32_createWindow(&dummyWindow, LDK_WINDOW_STYLE, 0,0,hInstance,TEXT("")) )
      {
        LogError("Could not create a dummy window for openGl initialization");
        return false;
      }

      PIXELFORMATDESCRIPTOR pfd = {};
      pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
      pfd.nVersion = 1;
      pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER ;
      pfd.iPixelType = PFD_TYPE_RGBA;
      pfd.cColorBits = colorBits;
      pfd.cDepthBits = depthBits;
      pfd.iLayerType = PFD_MAIN_PLANE;

      int pfId = ChoosePixelFormat(dummyWindow.dc, &pfd);
      if (pfId == 0)
      {
        LogError("Could not find a matching pixel format for GL dummy window");
        return false;
      }

      if (! SetPixelFormat(dummyWindow.dc, pfId, &pfd))
      {
        LogError("Could not set the pixel format for the Gl dummy window");
        return false;
      }

      dummyWindow.rc = wglCreateContext(dummyWindow.dc);
      if (!dummyWindow.rc)
      {
        LogError("Could not create a dummy OpenGl context");
        return false;
      }

      if (!wglMakeCurrent(dummyWindow.dc, dummyWindow.rc))
      {
        LogError("Could not make dummy OpenGL context current");
        return false;
      }

      bool success = true;

#define FETCH_GL_FUNC(type, name) success = success &&\
      (name = (type) ldk::platform::ldk_win32_getGlFunctionPointer((const char*)#name))
      FETCH_GL_FUNC(PFNGLGETSTRINGPROC, glGetString);
      FETCH_GL_FUNC(PFNGLENABLEPROC, glEnable);
      FETCH_GL_FUNC(PFNGLDISABLEPROC, glDisable);
      FETCH_GL_FUNC(PFNGLCLEARPROC, glClear);
      FETCH_GL_FUNC(PFNGLCLEARCOLORPROC, glClearColor);
      FETCH_GL_FUNC(PFNWGLCREATECONTEXTATTRIBSARBPROC, wglCreateContextAttribsARB);
      FETCH_GL_FUNC(PFNWGLCHOOSEPIXELFORMATARBPROC, wglChoosePixelFormatARB);
      FETCH_GL_FUNC(PFNGLGENBUFFERSPROC, glGenBuffers);
      FETCH_GL_FUNC(PFNGLBINDBUFFERPROC, glBindBuffer);
      FETCH_GL_FUNC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
      FETCH_GL_FUNC(PFNGLBUFFERSUBDATAPROC, glBufferSubData);
      FETCH_GL_FUNC(PFNGLBINDATTRIBLOCATIONPROC, glBindAttribLocation);
      FETCH_GL_FUNC(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer);
      FETCH_GL_FUNC(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray);
      FETCH_GL_FUNC(PFNGLGETERRORPROC, glGetError);
      FETCH_GL_FUNC(PFNGLGETPROGRAMIVPROC, glGetProgramiv);
      FETCH_GL_FUNC(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog);
      FETCH_GL_FUNC(PFNGLGETSHADERIVPROC, glGetShaderiv);
      FETCH_GL_FUNC(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog);
      FETCH_GL_FUNC(PFNGLCREATESHADERPROC, glCreateShader);
      FETCH_GL_FUNC(PFNGLSHADERSOURCEPROC, glShaderSource);
      FETCH_GL_FUNC(PFNGLCOMPILESHADERPROC, glCompileShader);
      FETCH_GL_FUNC(PFNGLCREATEPROGRAMPROC, glCreateProgram);
      FETCH_GL_FUNC(PFNGLATTACHSHADERPROC, glAttachShader);
      FETCH_GL_FUNC(PFNGLLINKPROGRAMPROC, glLinkProgram);
      FETCH_GL_FUNC(PFNGLDELETESHADERPROC, glDeleteShader);
      FETCH_GL_FUNC(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays);
      FETCH_GL_FUNC(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray);
      FETCH_GL_FUNC(PFNGLBUFFERDATAPROC, glBufferData);
      FETCH_GL_FUNC(PFNGLMAPBUFFERPROC, glMapBuffer);
      FETCH_GL_FUNC(PFNGLUNMAPBUFFERPROC, glUnmapBuffer);
      FETCH_GL_FUNC(PFNGLDRAWELEMENTSPROC, glDrawElements);
      FETCH_GL_FUNC(PFNGLUSEPROGRAMPROC, glUseProgram);
      FETCH_GL_FUNC(PFNGLFLUSHPROC, glFlush);
      FETCH_GL_FUNC(PFNGLVIEWPORTPROC, glViewport);
      FETCH_GL_FUNC(PFNGLGENTEXTURESPROC, glGenTextures);
      FETCH_GL_FUNC(PFNGLDELETETEXTURESPROC, glDeleteTextures);
      FETCH_GL_FUNC(PFNGLBINDTEXTUREPROC, glBindTexture);
      FETCH_GL_FUNC(PFNGLTEXPARAMETERFPROC, glTexParameteri);
      FETCH_GL_FUNC(PFNGLTEXIMAGE2DPROC, glTexImage2D);
      FETCH_GL_FUNC(PFNGLGENERATEMIPMAPPROC, glGenerateMipmap);
      FETCH_GL_FUNC(PFNGLBINDBUFFERBASEPROC, glBindBufferBase);
      FETCH_GL_FUNC(PFNGLGETUNIFORMBLOCKINDEXPROC, glGetUniformBlockIndex);
      FETCH_GL_FUNC(PFNGLSCISSORPROC, glScissor);
      FETCH_GL_FUNC(PFNGLDEPTHFUNCPROC, glDepthFunc);
      FETCH_GL_FUNC(PFNGLBLENDFUNCPROC, glBlendFunc);
      FETCH_GL_FUNC(PFNGLDEPTHMASKPROC, glDepthMask);
      FETCH_GL_FUNC(PFNGLDELETEBUFFERSPROC, glDeleteBuffers);
      FETCH_GL_FUNC(PFNGLPOLYGONMODEPROC, glPolygonMode);
      FETCH_GL_FUNC(PFNGLPOLYGONOFFSETPROC, glPolygonOffset);
      FETCH_GL_FUNC(PFNGLLINEWIDTHPROC, glLineWidth);
      FETCH_GL_FUNC(PFNGLUNIFORMBLOCKBINDINGPROC, glUniformBlockBinding);
      FETCH_GL_FUNC(PFNGLCULLFACEPROC, glCullFace);
      FETCH_GL_FUNC(PFNGLBLENDCOLORPROC, glBlendColor);
      FETCH_GL_FUNC(PFNGLGETACTIVEUNIFORMPROC, glGetActiveUniform);
      FETCH_GL_FUNC(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation);
      FETCH_GL_FUNC(PFNGLDETACHSHADERPROC, glDetachShader);
      FETCH_GL_FUNC(PFNGLGETACTIVEATTRIBPROC, glGetActiveAttrib);
      FETCH_GL_FUNC(PFNGLGETATTRIBLOCATIONPROC, glGetAttribLocation);
      FETCH_GL_FUNC(PFNGLFENCESYNCPROC, glFenceSync);
      FETCH_GL_FUNC(PFNGLDELETEPROGRAMPROC, glDeleteProgram);
      FETCH_GL_FUNC(PFNGLCLIENTWAITSYNCPROC, glClientWaitSync);
      FETCH_GL_FUNC(PFNGLDELETESYNCPROC, glDeleteSync);
      FETCH_GL_FUNC(PFNGLMAPBUFFERRANGEPROC, glMapBufferRange);
      FETCH_GL_FUNC(PFNGLACTIVETEXTUREPROC, glActiveTexture);
      FETCH_GL_FUNC(PFNGLDRAWARRAYSPROC, glDrawArrays);
      FETCH_GL_FUNC(PFNGLDISABLEVERTEXATTRIBARRAYPROC, glDisableVertexAttribArray);
      FETCH_GL_FUNC(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv);
      FETCH_GL_FUNC(PFNGLUNIFORM1IPROC, glUniform1i);
      FETCH_GL_FUNC(PFNGLUNIFORM2IPROC, glUniform2i);
      FETCH_GL_FUNC(PFNGLUNIFORM3IPROC, glUniform3i);
      FETCH_GL_FUNC(PFNGLUNIFORM4IPROC, glUniform4i);
      FETCH_GL_FUNC(PFNGLUNIFORM1FPROC, glUniform1f);
      FETCH_GL_FUNC(PFNGLUNIFORM2FPROC, glUniform2f);
      FETCH_GL_FUNC(PFNGLUNIFORM3FPROC, glUniform3f);
      FETCH_GL_FUNC(PFNGLUNIFORM4FPROC, glUniform4f);
#undef FETCH_GL_FUNC

      if (!success)
      {
        LogError("Could not fetch all necessary OpenGL function pointers");
        return false;
      }

      wglMakeCurrent(0,0);
      wglDeleteContext(dummyWindow.rc);
      DestroyWindow(dummyWindow.hwnd);

      // specify OPENGL attributes for pixel format
      const int pixelFormatAttribList[] =
      {
        WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
        WGL_COLOR_BITS_ARB, 32,
        WGL_DEPTH_BITS_ARB, 8,
        0
      };

      pfd = {};
      int numPixelFormats = 0;
      wglChoosePixelFormatARB(
          gameWindow.dc,
          pixelFormatAttribList,
          nullptr,
          1,
          &pfId,
          (UINT*) &numPixelFormats);

      if ( numPixelFormats <= 0)
      {
        LogError("Could not find a matching pixel format");
        return false;
      }

      if (! SetPixelFormat(gameWindow.dc, pfId, &pfd))
      {
        LogError("Could not set pixel format for OpenGL context creation");
        return false;
      }

      const int contextAttribs[] =
      {
        WGL_CONTEXT_MAJOR_VERSION_ARB, major,
        WGL_CONTEXT_MINOR_VERSION_ARB, minor,
        WGL_CONTEXT_FLAGS_ARB,
#ifdef _LDK_DEBUG_
        WGL_CONTEXT_DEBUG_BIT_ARB |
#endif // _LDK_DEBUG_
          WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
      };

      gameWindow.rc = wglCreateContextAttribsARB(
          gameWindow.dc,
          0,
          contextAttribs);

      if (!gameWindow.rc)
      {
        LogError("Could not create a core profile OpenGL context");
        return false;
      }

      if (!ldk_win32_makeContextCurrent(&gameWindow))
        return false;

      return true;
    }

    void ldk_win32_updateGamePad()
    {
      // get gamepad input
      for(int32 gamepadIndex = 0; gamepadIndex < LDK_MAX_JOYSTICKS; gamepadIndex++)
      {
        ldk::platform::XINPUT_STATE gamepadState;
        JoystickState& gamepad = _platform.gamepadState[gamepadIndex];

        // ignore unconnected controllers
        if ( platform::XInputGetState(gamepadIndex, &gamepadState) == ERROR_DEVICE_NOT_CONNECTED )
        {
          if ( gamepad.connected)
          {
            gamepad = {};					
          }
          gamepad.connected = 0;
          continue;
        }

        // digital buttons
        WORD buttons = gamepadState.Gamepad.wButtons;
        uint8 isDown=0;
        uint8 wasDown=0;

#define GET_GAMEPAD_BUTTON(btn) do {\
  isDown = (buttons & XINPUT_GAMEPAD_##btn) > 0;\
  wasDown = gamepad.button[ldk::input::LDK_JOYSTICK_##btn] & LDK_KEYSTATE_PRESSED;\
  gamepad.button[ldk::input::LDK_JOYSTICK_##btn] = ((isDown != wasDown) << 0x01) | isDown;\
} while(0)
        GET_GAMEPAD_BUTTON(DPAD_UP);			
        GET_GAMEPAD_BUTTON(DPAD_DOWN);
        GET_GAMEPAD_BUTTON(DPAD_LEFT);
        GET_GAMEPAD_BUTTON(DPAD_RIGHT);
        GET_GAMEPAD_BUTTON(START);
        GET_GAMEPAD_BUTTON(BACK);
        GET_GAMEPAD_BUTTON(LEFT_THUMB);
        GET_GAMEPAD_BUTTON(RIGHT_THUMB);
        GET_GAMEPAD_BUTTON(LEFT_SHOULDER);
        GET_GAMEPAD_BUTTON(RIGHT_SHOULDER);
        GET_GAMEPAD_BUTTON(A);
        GET_GAMEPAD_BUTTON(B);
        GET_GAMEPAD_BUTTON(X);
        GET_GAMEPAD_BUTTON(Y);
#undef SET_GAMEPAD_BUTTON

        //TODO: Make these calculations directly in assembly to make it faster
#define GAMEPAD_AXIS_VALUE(value) (value/(float)(value < 0 ? XINPUT_MIN_AXIS_VALUE * -1: XINPUT_MAX_AXIS_VALUE))
#define GAMEPAD_AXIS_IS_DEADZONE(value, deadzone) ( value > -deadzone && value < deadzone)

        // Left thumb axis
        int32 axisX = gamepadState.Gamepad.sThumbLX;
        int32 axisY = gamepadState.Gamepad.sThumbLY;
        int32 deadZone = XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE;

        gamepad.axis[ldk::input::LDK_JOYSTICK_AXIS_LX] = GAMEPAD_AXIS_IS_DEADZONE(axisX, deadZone) ? 0.0f :
        GAMEPAD_AXIS_VALUE(axisX);

        gamepad.axis[ldk::input::LDK_JOYSTICK_AXIS_LY] = GAMEPAD_AXIS_IS_DEADZONE(axisY, deadZone) ? 0.0f :	
        GAMEPAD_AXIS_VALUE(axisY);

        // Right thumb axis
        axisX = gamepadState.Gamepad.sThumbRX;
        axisY = gamepadState.Gamepad.sThumbRY;
        deadZone = XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE;

        gamepad.axis[ldk::input::LDK_JOYSTICK_AXIS_RX] = GAMEPAD_AXIS_IS_DEADZONE(axisX, deadZone) ? 0.0f :
        GAMEPAD_AXIS_VALUE(axisX);

        gamepad.axis[ldk::input::LDK_JOYSTICK_AXIS_RY] = GAMEPAD_AXIS_IS_DEADZONE(axisY, deadZone) ? 0.0f :	
        GAMEPAD_AXIS_VALUE(axisY);


        // Left trigger
        axisX = gamepadState.Gamepad.bLeftTrigger;
        axisY = gamepadState.Gamepad.bRightTrigger;
        deadZone = XINPUT_GAMEPAD_TRIGGER_THRESHOLD;

        gamepad.axis[ldk::input::LDK_JOYSTICK_AXIS_LTRIGGER] = GAMEPAD_AXIS_IS_DEADZONE(axisX, deadZone) ? 0.0f :	
        axisX/(float) XINPUT_MAX_TRIGGER_VALUE;

        gamepad.axis[ldk::input::LDK_JOYSTICK_AXIS_RTRIGGER] = GAMEPAD_AXIS_IS_DEADZONE(axisY, deadZone) ? 0.0f :	
        axisY/(float) XINPUT_MAX_TRIGGER_VALUE;

#undef GAMEPAD_AXIS_IS_DEADZONE
#undef GAMEPAD_AXIS_VALUE

        gamepad.connected = 1;
        }
}

//---------------------------------------------------------------------------
// Plays an audio buffer
// Returns the created buffer id
//---------------------------------------------------------------------------
uint32 createAudioBuffer(void* fmt, uint32 fmtSize, void* data, uint32 dataSize)
{
  BoundAudio* audio = nullptr;
  uint32 audioId = _platform.boundBufferCount;

  if (_platform.boundBufferCount < LDK_MAX_AUDIO_BUFFER)
  {
    // Get an audio buffer from the list
    audio = &(_platform.boundBufferList[audioId]);
    _platform.boundBufferCount++;
  }
  else
  {
    return -1;
  }

  // set format
  WAVEFORMATEXTENSIBLE wfx = *((WAVEFORMATEXTENSIBLE*) fmt);
  // set data
  BYTE *pDataBuffer = (BYTE*) data;

  // set XAUDIO2 instructions on what and how to play
  audio->buffer.AudioBytes = dataSize;
  audio->buffer.pAudioData = (BYTE*) data;
  audio->buffer.Flags = XAUDIO2_END_OF_STREAM;

  HRESULT hr = 0;
  //TODO: figure out how to use one single struct for both modern and legacy XAudio
  if (pXAudio2_7 != nullptr)
  {
    hr = pXAudio2_7->CreateSourceVoice(&audio->voice, (WAVEFORMATEX*)&wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO,nullptr, nullptr);
  }
  else
  {
    hr = pXAudio2->CreateSourceVoice(&audio->voice, (WAVEFORMATEX*)&wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO,nullptr, nullptr);
  }
  if (FAILED(hr))
  {
    LogError("Error creating source voice");
  }
  return audioId; 
}

//---------------------------------------------------------------------------
// Plays an audio buffer
//---------------------------------------------------------------------------
void playAudioBuffer(uint32 audioBufferId)
{
  if (_platform.boundBufferCount >= LDK_MAX_AUDIO_BUFFER || _platform.boundBufferCount <= 0)
    return;

  BoundAudio* audio = &(_platform.boundBufferList[audioBufferId]);
  HRESULT hr = audio->voice->SubmitSourceBuffer(&audio->buffer);

  if (FAILED(hr))
  {
    LogError("Error %x submitting audio buffer", hr);
  }

  hr = audio->voice->Start(0);
  if (FAILED(hr))
  {
    LogError("Error %x playing audio", hr);
  }
}

// Initialize the platform layer
uint32 initialize()
{
  CoInitialize(NULL);

  _appInstance = GetModuleHandle(NULL);

  // Set working directory
  char path[MAX_PATH];
  DWORD pathLen = GetModuleFileName(_appInstance, path, MAX_PATH);
  char*c = path + pathLen;
  do{
    --c;
  }while (*c != '\\' && c > path);
  *++c=0;

  // initialize timer data
  QueryPerformanceFrequency(&_platform.ticksPerSecond);
  QueryPerformanceCounter(&_platform.ticksSinceEngineStartup);
  SetCurrentDirectory(path);
  LogInfo("LDK Running from '%s'", path);
  ldk_win32_initXInput();
  ldk_win32_initXAudio();
  return ldk_win32_registerWindowClass(_appInstance);
}

// terminates the platform layer
void terminate()
{
  LogInfo("LDK terminating...");
}

void setEventCallback(LDKWindow* window, LDKPlatformEventHandleFunc eventCallback)
{
  window->eventCallback = eventCallback;
}

// Creates a window
LDKWindow* createWindow(uint32* attributes, const char* title, LDKWindow* share)
{
  uint32* pAttribute = attributes;	
  uint32 width = 800;
  uint32 height = 600;
  uint32 visible = 1;
  uint32 colorBits = 32;
  uint32 depthBits = 24;
  uint32 glVersionMajor = 3;
  uint32 glVersionMinor = 3;
  bool success = true;

  while ( pAttribute != 0 && *pAttribute != 0 )
  {
    ldk::platform::WindowHint windowHint = (ldk::platform::WindowHint) *pAttribute;

    switch (windowHint)
    {
      case ldk::platform::WindowHint::WIDTH:
        width = *++pAttribute;
        break;
      case ldk::platform::WindowHint::HEIGHT:
        height = *++pAttribute;
        break;
      case ldk::platform::WindowHint::VISIBLE:
        visible = *++pAttribute;
        break;
      case ldk::platform::WindowHint::GL_CONTEXT_VERSION_MAJOR:
        glVersionMajor = *++pAttribute;
        break;
      case ldk::platform::WindowHint::GL_CONTEXT_VERSION_MINOR:
        glVersionMinor = *++pAttribute;
        break;
      case ldk::platform::WindowHint::COLOR_BUFFER_BITS:
        colorBits = *++pAttribute;
        break;
      case ldk::platform::WindowHint::DEPTH_BUFFER_BITS:
        depthBits = *++pAttribute;
        break;
      default:
        LogError("Ignoring unkown window hint");
        break;
    }
    ++pAttribute;
  }

  ldk::platform::LDKWindow* window = (platform::LDKWindow*) ldkEngine::memory_alloc(sizeof(LDKWindow));
  *window = {};

  window->eventCallback = dummyEventHandler;

  // Calculate total window size
  RECT clientArea = {(LONG)0,(LONG)0, (LONG)width, (LONG)height};
  if (!AdjustWindowRect(&clientArea, WS_OVERLAPPEDWINDOW, FALSE))
  {
    LogError("Could not calculate window size");
  }

  uint32 windowWidth = clientArea.right - clientArea.left;
  uint32 windowHeight = clientArea.bottom - clientArea.top;

  if (!ldk_win32_createWindow(window, LDK_WINDOW_STYLE, windowWidth, windowHeight, _appInstance, (TCHAR*) title))
  {

    LogError("Could not create window");
    return nullptr;
  }

  ldk_win32_calculateClientRect(window);

  /* create a new context or share an existing one ? */
  if (share)
  {
    //FIXME: Context sharing is not working!
    window->rc = share->rc;
    wglMakeCurrent(window->dc, window->rc);
  }
  else
  {
    if (!ldk_win32_initOpenGL(*window, _appInstance, glVersionMajor, glVersionMinor, colorBits, depthBits))
    {
      success = false;
    }
  }

  if (visible)
  {
    ldk::platform::showWindow(window);
  }

  if (!success)
  {
    ldkEngine::memory_free(window);
    return nullptr;
  }

  //LogInfo("Initialized OpenGL %s\n\t%s\n\t%s", 
  LogInfo("Initialized OpenGL\tVERSION: %s\tVENDOR: %s\tRENDERER: %s", 
      glGetString(GL_VERSION),
      glGetString(GL_VENDOR),
      glGetString(GL_RENDERER));

  //_platform.windowList.insert(std::make_pair(window->hwnd, window));
  _platform.windowList[window->hwnd] = window;
  return window;
}

// Toggles the window fullscreen/windowed
void toggleFullScreen(LDKWindow* window, bool fullScreen)
{
  if (fullScreen == window->fullscreenFlag)
    return;

  LONG newStyle = 0;
  RECT newRect;

  if (fullScreen)
  {
    // save current rect and style
    GetWindowRect(window->hwnd, &window->defaultRect);
    window->defaultStyle = GetWindowLong(window->hwnd, GWL_STYLE);

    LogInfo("SAVING WINDOW SIZE = %dx%d %dx%d",
        window->defaultRect.top,
        window->defaultRect.left,
        window->defaultRect.right,
        window->defaultRect.bottom);
    newStyle = WS_POPUP;
    GetWindowRect(GetDesktopWindow(), &newRect);
  }
  else
  {
    newStyle = window->defaultStyle;
    newRect = window->defaultRect;
  }

  window->fullscreenFlag = fullScreen;
  SetWindowLong(window->hwnd, GWL_STYLE, newStyle);
  SetWindowPos(window->hwnd, HWND_TOP, 
      0, 0,
      newRect.right - newRect.left, 
      newRect.bottom - newRect.top, 
      SWP_SHOWWINDOW);
}

bool isFullScreen(LDKWindow* window)
{
  return window->fullscreenFlag;
}

// Destroys a window
void destroyWindow(LDKWindow* window)
{
  auto it = _platform.windowList.find(window->hwnd);
  _platform.windowList.erase(it);
  DestroyWindow(window->hwnd);
}

// returns the value of the close flag of the specified window
bool windowShouldClose(LDKWindow* window)
{
  return window->closeFlag;
}

void setWindowCloseFlag(LDKWindow* window, bool flag)
{
  window->closeFlag = flag;	
  window->event.type = ldk::EventType::QUIT_EVENT;

  if (window->eventCallback)
    window->eventCallback(window, &window->event);
}

// Update the window framebuffer
void swapWindowBuffer(LDKWindow* window)
{
  //TODO: Enable this when implementing gl context sharing
  //ldk_win32_makeContextCurrent(window);
  if (window->closeFlag)
    return;

  if (!SwapBuffers(window->dc))
  {
    LogInfo("SwapBuffer error %x", GetLastError());
    return;
  }
}

void showWindow(LDKWindow* window)
{
  ShowWindow(window->hwnd, SW_SHOW);
}

void showCursor(LDKWindow* window, bool show)
{
  ShowCursor(show);
}

// Get the state of mouse
const ldk::platform::MouseState* getMouseState()
{
  return &_platform.mouseState;
}

// Get the state of keyboard
const ldk::platform::KeyboardState*	getKeyboardState()
{
  return &_platform.keyboardState;
}

// Get the state of a gamepad.
const ldk::platform::JoystickState* getJoystickState(uint32 gamepadId)
{
  LDK_ASSERT(( gamepadId >=0 && gamepadId < LDK_MAX_JOYSTICKS),"Gamepad id is out of range");
  return &_platform.gamepadState[gamepadId];
}

// Updates all windows and OS dependent events
void pollEvents()
{
  // clear 'changed' bit from keyboard key state
  for(int i=0; i < LDK_MAX_KBD_KEYS ; i++)
  {
    _platform.keyboardState.key[i] &= ~LDK_KEYSTATE_CHANGED;
  }

  // clear 'changed' bit from gamepads buttons state
  for (uint32 id=0; id < LDK_MAX_JOYSTICKS; id++)
  {
    // clear 'changed' bit from input key state
    for(uint32 i=0; i < LDK_JOYSTICK_MAX_DIGITAL_BUTTONS ; i++)
    {
      _platform.gamepadState[id].button[i] &= ~LDK_KEYSTATE_CHANGED;
    }
  }

  // clear 'changed' bit from mouse buttons
  for (int i=0; i < LDK_MAX_MOUSE_BUTTONS; i++)
  {
    _platform.mouseState.button[i] &= ~LDK_KEYSTATE_CHANGED;
  }

  ldk_win32_updateGamePad();

  MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
    LDKWindow* window = findWindowByHandle(msg.hwnd);
    bool postKeyboardEvent = false;

    switch(msg.message)
    {
      LDK_ASSERT(window != nullptr, "Could not found a matching window for the current event hwnd");
      case WM_CHAR:
        postKeyboardEvent = true;
      case WM_KEYDOWN:
      case WM_KEYUP:
        {
          int8 isDown = (msg.lParam & (1 << 31)) == 0; //  0 means pressed, 1 means released
          int8 wasDown = (msg.lParam & (1 << 30)) != 0;
          int8 isShiftDown = (msg.lParam & (1 << 29)) != 0;
          int8 repeat = LOWORD(msg.lParam);
          int16 vkCode = msg.wParam;
          _platform.keyboardState.key[vkCode] = ((isDown != wasDown) << 1) | isDown;
          int8 isAltDown = _platform.keyboardState.key[ldk::input::LDK_KEY_ALT];
          int8 isControlDown = _platform.keyboardState.key[ldk::input::LDK_KEY_CONTROL];

          // we only want to post event in case of a WM_CHAR
          if(postKeyboardEvent)
          {
            window->event.type = ldk::EventType::KEYBOARD_EVENT;
            window->event.keyboardEvent.type = isDown 
              ? ldk::KeyboardEvent::KEYBOARD_KEY_DOWN
              : ldk::KeyboardEvent::KEYBOARD_KEY_UP;
            window->event.keyboardEvent.key = vkCode;
            window->event.keyboardEvent.text = msg.wParam;
            window->event.keyboardEvent.repeat = repeat;
            window->event.keyboardEvent.isControlDown = isControlDown;
            window->event.keyboardEvent.isAltDown = isAltDown;
            window->event.keyboardEvent.isShiftDown = isShiftDown;
            window->eventCallback(window, (const ldk::Event*)&window->event);
            postKeyboardEvent = false;
          }
          continue;
        }
        break;

      case WM_LBUTTONDOWN:
      case WM_LBUTTONUP:
      case WM_MBUTTONDOWN:
      case WM_MBUTTONUP:
      case WM_RBUTTONDOWN:
      case WM_RBUTTONUP:
      case WM_MOUSEMOVE:

        uint32 x = GET_X_LPARAM(msg.lParam);
        uint32 y = GET_Y_LPARAM(msg.lParam);
        _platform.mouseState.cursor = {(float)x ,(float)y};

        int8 isDown = (msg.wParam & MK_LBUTTON) == MK_LBUTTON;
        _platform.mouseState.button[ldk::input::LDK_MOUSE_LEFT] =
          ((_platform.mouseState.button[ldk::input::LDK_MOUSE_LEFT] != isDown) << 1) | isDown;

        isDown = (msg.wParam & MK_MBUTTON) == MK_MBUTTON;
        _platform.mouseState.button[ldk::input::LDK_MOUSE_MIDDLE] =
          ((_platform.mouseState.button[ldk::input::LDK_MOUSE_MIDDLE] != isDown) << 1) | isDown;

        isDown = (msg.wParam & MK_RBUTTON) == MK_RBUTTON;
        _platform.mouseState.button[ldk::input::LDK_MOUSE_RIGHT] =
          ((_platform.mouseState.button[ldk::input::LDK_MOUSE_RIGHT] != isDown) << 1) | isDown;

        break;
    }
  }
}

ldk::platform::SharedLib* loadSharedLib(char* sharedLibName)
{
  LogInfo("Loading Module:\t'%s'", sharedLibName);
  HMODULE hmodule = LoadLibrary(sharedLibName);

  if (!hmodule)
  {
    LogError("Could not load shared lib:\t'%s'", sharedLibName);
    return nullptr;
  }

  //TODO: Use custom allocation here
  SharedLib* sharedLib = new SharedLib;
  sharedLib->handle = hmodule;
  return sharedLib;
}

bool unloadSharedLib(ldk::platform::SharedLib* sharedLib)
{

  if(sharedLib->handle)
  {
    const uint32 buffSize = 255;
    char8 moduleName[buffSize];
    uint32 nameLen = GetModuleFileNameA(sharedLib->handle, (LPSTR)&moduleName, buffSize);
    if(nameLen < buffSize)
      moduleName[nameLen] = 0;
    LogInfo("Unloading Module:\t'%s'", moduleName);

    if (FreeLibrary(sharedLib->handle))
    {

      sharedLib->handle = NULL;
      delete sharedLib;
      return true;
    }

  }


  LogError("Could not unload unknown sahred lib");
  return false;
}

const void* getFunctionFromSharedLib(const ldk::platform::SharedLib* sharedLib, const char* function)
{
  return GetProcAddress(sharedLib->handle, function);
}

void* memoryAlloc(size_t size)
{
  return ldkEngine::memory_alloc(size, ldkEngine::Allocation::Tag::GENERAL);
}

void memoryFree(void* memory)
{
  return ldkEngine::memory_free(memory);
}

void* memoryRealloc(void* memory, size_t size)
{
  return ldkEngine::memory_realloc(memory, size);
}

void* loadFileToBufferOffset(const char* fileName, size_t* fileSize, size_t additionalSize, size_t offset)
{
  HANDLE hFile = CreateFile((LPCSTR)fileName,
      GENERIC_READ,
      FILE_SHARE_READ,
      0,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      0);

  DWORD err = GetLastError();

  if (hFile == INVALID_HANDLE_VALUE) 
  {
    LogError("Could not open file '%s'", fileName);
    return nullptr;
  }

  int32 fileSizeLocal = GetFileSize(hFile, 0);
  size_t totalSize = fileSizeLocal + additionalSize;


  char* buffer = (char*) memoryAlloc(totalSize);
  uint32 bytesRead = 0;

  int32 readSuccess = ReadFile(hFile, (LPVOID)(buffer + offset), (int32)fileSizeLocal, (LPDWORD)&bytesRead, 0);
  if (!buffer || !readSuccess)
  {
    err = GetLastError();
    CloseHandle(hFile);
    LogError("%d Could not read file '%s'", err, fileName);
    return nullptr;
  }

  CloseHandle(hFile);

  if (fileSize != nullptr) { *fileSize = bytesRead; }
  return buffer; 
}

void* loadFileToBuffer(const char* fileName, size_t* fileSize)
{
  return loadFileToBufferOffset(fileName, fileSize, 0, 0);
}

int64 getFileWriteTime(const char* fileName)
{
  FILETIME writeTime;
  HANDLE handle = CreateFileA(fileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  GetFileTime(handle, 0, 0, &writeTime);
  CloseHandle(handle);
  return ((((int64) writeTime.dwHighDateTime) << 32) + writeTime.dwLowDateTime);
}

bool copyFile(const char* sourceFileName, const char* destFileName)
{
  return CopyFileA(sourceFileName, destFileName, false);
}

bool moveFile(const char* sourceFileName, const char* destFileName)
{
  return MoveFile(sourceFileName, destFileName);
}

bool deleteFile(const char* sourceFileName)
{
  return DeleteFile(sourceFileName);
}

uint64 getTicks()
{
  LARGE_INTEGER value;
  QueryPerformanceCounter(&value);
  return value.QuadPart;
}

float getTimeBetweenTicks(uint64 start, uint64 end)
{
  end -= _platform.ticksSinceEngineStartup.QuadPart;
  start -= _platform.ticksSinceEngineStartup.QuadPart;

  float deltaTime = (( end - start))/ (float)_platform.ticksPerSecond.QuadPart;
#ifdef _LDK_DEBUG_ 
  // if we stopped on a breakpoint, make things behave mor natural
  if ( deltaTime > 0.05f)
    deltaTime = 0.016f;
#endif
  return deltaTime;
}

} // namespace platform
} // namespace ldk
