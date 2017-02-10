/*

Copyright (c) 2016, Libertus Code
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that
the following conditions are met:
  * Redistributions of source code must retain the above copyright notice, this list of conditions and
    the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
    the following disclaimer in the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*/

#pragma once

#if defined(_WIN32)
  #define CINDER_MSW
  #if ! defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
  #endif
  #include <Windows.h>
  #include <comutil.h>
  #pragma comment(lib, "comsuppw.lib")
#elif defined(__linux__)
  #define CINDER_LINUX
#endif

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace cinder {
  namespace app {
    class App;
	  class Window;
	  class Renderer;

    namespace detail {
      using CreateRendererFn = std::function<cinder::app::Renderer*(cinder::app::Window*)>;
	    struct AppParams {
        std::string       name;
#if defined(CINDER_MSW)
		    HINSTANCE			    hInstance = nullptr;
        int               nCmdShow = 0;
#endif
		    CreateRendererFn  createRendererFn;
	    };
	  } // namespace detail

    class KeyEvent {
    };

    class MouseEvent {
    };

	  class Renderer {
	  public:
      Renderer(Window* window);
      virtual ~Renderer();
#if defined(CINDER_MSW)
      virtual void  setup(HWND wnd, HDC dc, std::shared_ptr<Renderer> sharedRenderer) = 0;
#endif
    protected:
      Window*   mWindow = nullptr;
	  };

	  //! \class WindowImpl
	  //!
	  //!
	  class WindowImpl {
	  public:
      WindowImpl(Window* window);
      virtual ~WindowImpl();
      virtual void  redraw() = 0;
    protected:
      virtual void  setupRenderer() = 0;
	  protected:
	    Window*					mWindow;
	    std::unique_ptr<Renderer>	mRenderer;
    private:
      void createRenderer(detail::CreateRendererFn createRendererFn);      
      friend class Window;
	  };
    
	  //! \class Window
	  //!
	  //!
	  class Window {
	  public:
      Window(App* app);
      virtual ~Window();
      App*          getApp() const {return mApp;}
      virtual void  close();
      virtual void  redraw();
	  private:
      App*                        mApp = nullptr;
	    std::unique_ptr<WindowImpl>	mImpl;
    public:
      //! Do not call this method direct, the behavior is undefined.
      void private_draw();
	  };

    //! \class AppImpl
    //!
    //!
    class AppImpl {
    public:
      AppImpl(App* app);
      virtual ~AppImpl();
    protected:
      virtual void  run() = 0;
      virtual void  closeWindow(Window* window) = 0;
      friend class App;
    protected:
      App*						                      mApp = nullptr;
      bool                                  mShouldQuit = false;
	    std::vector<std::unique_ptr<Window>>  mWindows; 
    private:
      void private_redraw();
    };
    
    //! \class App
    //!
    //!
    class App {
    public:
      class Settings {
      };

      App();
      virtual ~App();

      const detail::AppParams&  getAppParams() const {return mAppParams;}

      // Do not call this method directly, behavior is undefined.
      void          private_run(); 

      virtual void  setup() {}
      virtual void	mouseDrag(MouseEvent event) {}
      virtual void	keyDown(KeyEvent event) {}
      virtual void	update() {}
      virtual void	draw() {}
    private:
      detail::AppParams         mAppParams;
      std::unique_ptr<AppImpl>	mImpl;
      Window*                   mCurrentWindow = nullptr;
    
    private:
      void private_setup();
      void private_update();
      void private_draw(Window* window);
      void private_redraw();
      void private_close_window(Window* window);
#if defined(CINDER_MSW)
      friend class AppImplMsw;
#endif
      friend class Window;

    public:
      static void configureApp(const detail::AppParams& appParams, std::function<void(App::Settings*)> prepareSettingsFn = nullptr);
    };

#if defined(CINDER_MSW)
    //! \class AppImplMsw
    //!
    //!
    class AppImplMsw : public AppImpl {
	  public:
      AppImplMsw(App* app);
      virtual ~AppImplMsw();
    protected:
      virtual void  run() override;
      virtual void  closeWindow(Window* window) override;
	  private:
    };

	  //! \class WindowImplMsw
	  //!
	  //!
	  class WindowImplMsw : public WindowImpl {	
	  public:
        WindowImplMsw(Window* window);
        virtual ~WindowImplMsw();
        virtual void  redraw() override;
      protected:
        virtual void  setupRenderer() override;
	  private:
        HWND                  mWnd = nullptr;
        HDC                   mDc = nullptr;
        std::vector<wchar_t>  mWindowClassName;
	  };
#elif defined(CINDER_LINUX)
	  //! \class App
	  //!
	  //!
    class AppImplLinux : public AppImpl {
    };

	  //! \class App
	  //!
	  //!
	  class WindowImplLinux : public WindowImpl {
	  };
#endif

    class RendererGl : public Renderer {
    public:
      RendererGl(Window* window);
      virtual ~RendererGl();
      virtual void  setup(HWND wnd, HDC dc, std::shared_ptr<Renderer> sharedRenderer) override;
    };
  } // namespace app
} // namespace cinder

namespace ci = cinder;

#if defined(TINYCI_IMPLEMENTATION)
#include <unordered_map>

namespace cinder {
  namespace app {
    namespace detail {
	    static AppParams      sAppParams;
      static App::Settings  sAppSettings;
	  } // namespace detail

	  // ------------------------------------------------------------------------------------------------
	  // App
	  // ------------------------------------------------------------------------------------------------

    App::App() {
      mAppParams = detail::sAppParams;
      detail::sAppParams = {};
#if defined(CINDER_MSW)
	    mImpl = std::make_unique<AppImplMsw>(this);
#endif
	  }

    App::~App() {
	  }

    void App::private_setup() {
      setup();
    }

    void App::private_update() {
      update();
    }

    void App::private_draw(Window* window)
    {
      mCurrentWindow = window;
      draw();
    }

    void App::private_redraw() {
      mImpl->private_redraw();
    }

    void App::private_run() {
      mImpl->run();
    }

    void App::private_close_window(Window* window)
    {
      mImpl->closeWindow(window);
    }

    void App::configureApp(const detail::AppParams& appParams, std::function<void(App::Settings*)> prepareSettingsFn)
    {
      detail::sAppParams = appParams;
      if (prepareSettingsFn) {
        prepareSettingsFn(&(detail::sAppSettings));
      }
    }

	  // ------------------------------------------------------------------------------------------------
	  // AppImpl
	  // ------------------------------------------------------------------------------------------------
    AppImpl::AppImpl(App* app)
	  	: mApp(app)
	  {
	  }
	  
	  AppImpl::~AppImpl() {
	  }

    void AppImpl::private_redraw()
    {
      for (auto& window : mWindows) {
        window->redraw();
      }
    }

    // ------------------------------------------------------------------------------------------------
	  // Window
	  // ------------------------------------------------------------------------------------------------
    Window::Window(App* app)
      : mApp(app)
    {
#if defined(CINDER_MSW)
	    mImpl = std::make_unique<WindowImplMsw>(this);
#endif
      auto createRendererFn = app->getAppParams().createRendererFn;
      mImpl->createRenderer(createRendererFn);
      mImpl->setupRenderer();
	  }

    Window::~Window() {
	  }

    void Window::close()
    {
      mApp->private_close_window(this);
    }

    void Window::redraw()
    {
      mImpl->redraw();
    }

    void Window::private_draw() {
      mApp->private_draw(this);
    }

	  // ------------------------------------------------------------------------------------------------
	  // WindowImpl
	  // ------------------------------------------------------------------------------------------------
    WindowImpl::WindowImpl(Window* window)
      : mWindow(window)
    {
    }
    
    WindowImpl::~WindowImpl() {
	  }

    void WindowImpl::createRenderer(detail::CreateRendererFn createRendererFn) {
      Renderer* renderer = createRendererFn(mWindow);
      mRenderer = std::unique_ptr<Renderer>(renderer);
    }

	  // ------------------------------------------------------------------------------------------------
	  // Renderer
	  // ------------------------------------------------------------------------------------------------
    Renderer::Renderer(Window* window)
      : mWindow(window)
    {
    }

    Renderer::~Renderer() {
    }

	  // ------------------------------------------------------------------------------------------------
	  // RendererGl
	  // ------------------------------------------------------------------------------------------------
    RendererGl::RendererGl(Window* window)
      : Renderer(window)
    {
    }

    RendererGl::~RendererGl() {
    }

    void RendererGl::setup(HWND wnd, HDC dc, std::shared_ptr<Renderer> sharedRenderer) {
      int stopMe = 1;
    }
  } // namespace app
} // namespace cinder

#if defined(CINDER_MSW)
namespace cinder {
  namespace app {
	  // ------------------------------------------------------------------------------------------------
	  // AppImplMsw
	  // ------------------------------------------------------------------------------------------------
    AppImplMsw::AppImplMsw(App* app)
      : AppImpl(app) 
	  {
        auto window = std::make_unique<Window>(app);
        mWindows.push_back(std::move(window));
	  }

    AppImplMsw::~AppImplMsw() {
	  }

    void AppImplMsw::run() {
      mApp->private_setup();

      MSG msg = {};
      while (! mShouldQuit) {
        // Regulate frame rate
        mApp->private_update();
        if (! mShouldQuit ) {
          mApp->private_redraw();
        }

        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
          if (msg.message == WM_QUIT) {
            break;
          }

          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
      }
    }

    void AppImplMsw::closeWindow(Window* window)
    {
      mWindows.erase(std::remove_if(std::begin(mWindows),
                                    std::end(mWindows),
                                    [&window](const auto& elem) -> bool {
                                      return window == elem.get();
                                    }),
                     std::end(mWindows));
      if (mWindows.empty()) {
        mShouldQuit = true;
      }
    }

    // ------------------------------------------------------------------------------------------------
	  // WindowImplMsw
	  // ------------------------------------------------------------------------------------------------
    static std::unordered_map<HWND, Window*> sWndToWindow;
    static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    WindowImplMsw::WindowImplMsw(Window* window)
      : WindowImpl(window)
    {
      const auto& appParmas = mWindow->getApp()->getAppParams();
      const auto& classNameUtf8 = appParmas.name;

      mWindowClassName.resize(classNameUtf8.size() + 1);
      size_t convertedChars = 0;
      mbstowcs_s(&convertedChars, mWindowClassName.data(), mWindowClassName.size(), classNameUtf8.c_str(), _TRUNCATE);

      WNDCLASSEX wc = {};
	    wc.cbSize         = sizeof(WNDCLASSEX);
	    wc.style          = CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc    = WndProc;
	    wc.cbClsExtra     = NULL;
	    wc.cbWndExtra     = NULL;
	    wc.hInstance      = appParmas.hInstance;
	    wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 2);
	    wc.lpszMenuName   = NULL;
	    wc.lpszClassName  = mWindowClassName.data();
	    wc.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	    if (!RegisterClassEx(&wc)){
		    MessageBox(NULL, L"Error registering window class", L"Error", MB_OK | MB_ICONERROR);
        throw std::runtime_error("Error registering window class");
	    }

      DWORD dwStyle = WS_OVERLAPPEDWINDOW;
      RECT windowRect = {0, 0, 640, 480};
      AdjustWindowRect(&windowRect, dwStyle, FALSE);
      int windowWidth = windowRect.right - windowRect.left;
      int windowHeight = windowRect.bottom - windowRect.top;

	    mWnd = CreateWindowEx(NULL, mWindowClassName.data(), mWindowClassName.data(),
		    dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
		    windowWidth, windowHeight, NULL, NULL, appParmas.hInstance, NULL);

	    if (! mWnd) {
		    MessageBox(NULL, L"Error creating window", L"Error", MB_OK | MB_ICONERROR);
		    throw std::runtime_error("Error creating window");
	    }

      sWndToWindow[mWnd] = mWindow;

      ShowWindow(mWnd, appParmas.nCmdShow);
      UpdateWindow(mWnd);
    }

    WindowImplMsw::~WindowImplMsw() {
      const auto& appParmas = mWindow->getApp()->getAppParams();
      UnregisterClass(mWindowClassName.data(), appParmas.hInstance);
    }

    void WindowImplMsw::redraw()
    {
	    ::RedrawWindow( mWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW );
    }

    void WindowImplMsw::setupRenderer() {
      mRenderer->setup(mWnd, mDc, nullptr);
    }

    static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
      Window* window = nullptr;
      if (sWndToWindow.find(wnd) != sWndToWindow.end()) {
        window = sWndToWindow[wnd];
      }
    
      switch (msg) {
        case WM_CREATE: {
          // NOTE: sWndWindow will not yet contain an entry for wnd since
          //       this message will be sent before CreateWindowEx finishes.
        } break;

        case WM_KEYDOWN: {
          assert(window != nullptr);
        } break; 

        case WM_PAINT: {
          assert(window != nullptr);
          window->private_draw();
        } break; 

        
        case WM_DESTROY: {
          PostQuitMessage(0);
          sWndToWindow.erase(wnd); 
          window->close();
          return 0;
        } break;
      }
      return DefWindowProc(wnd, msg, wParam, lParam);
    }
  } // namespace app
} // namespace cinder
#endif

#endif // defined(TINYCI_IMPLEMENTATION)

#if defined(CINDER_MSW)
  #define CINDER_APP( APP, RENDERER, ... )                                                               \
    int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) \
    {                                                                                                    \
      cinder::app::detail::AppParams appParams;                                                          \
      appParams.name = #APP;                                                                             \
      appParams.hInstance = hInstance;                                                                   \
      appParams.nCmdShow = nCmdShow;                                                                     \
      appParams.createRendererFn                                                                         \
		    = [](cinder::app::Window* window) -> cinder::app::Renderer* {                                    \
			      return new RENDERER(window); };                                                              \
      cinder::app::App::configureApp(appParams, ##__VA_ARGS__);                                          \
	    std::unique_ptr<ci::app::App> app = std::make_unique<APP>();                                       \
      app->private_run();                                                                                \
      return 0;                                                                                          \
    }
#elif defined(CINDER_LINUX)
#endif

