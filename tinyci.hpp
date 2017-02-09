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
#elif defined(__linux__)
  #define CINDER_LINUX
#endif

#include <functional>
#include <memory>

namespace cinder {
  namespace app {
    class App;
	  class Window;
	  class Renderer;

    namespace detail {
      using CreateRendererFn = std::function<cinder::app::Renderer*(cinder::app::Window*)>;
	    struct AppParams {
#if defined(CINDER_MSW)
		    HINSTANCE			    hInstance = nullptr;
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
    protected:
      virtual   void setupRenderer() = 0;
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
        Window(App* app, detail::CreateRendererFn createRendererFn);
        virtual ~Window();
	  private:
      App*                        mApp = nullptr;
	    std::unique_ptr<WindowImpl>	mImpl;
	  };

    //! \class AppImpl
    //!
    //!
    class AppImpl {
    public:
      AppImpl(App* app, const detail::AppParams& appParams);
      virtual ~AppImpl();
    protected:
      virtual void  run() = 0;
      friend class App;
    protected:
      App*						        mApp = nullptr;
	    detail::AppParams			  mAppParams = {};
	    std::unique_ptr<Window>	mMainWindow;
    };
    
    //! \class App
    //!
    //!
    class App {
    public:
      App();
      virtual ~App();

      void          run();

      virtual void	mouseDrag(MouseEvent event) {}
      virtual void	keyDown(KeyEvent event) {}
      virtual void	update() {}
      virtual void	draw() {}
    private:
      detail::AppParams         mAppParams;
      std::unique_ptr<AppImpl>	mImpl;
    };

#if defined(CINDER_MSW)
    //! \class AppImplMsw
    //!
    //!
    class AppImplMsw : public AppImpl {
	  public:
      AppImplMsw(App* app, const detail::AppParams& appParams);
      virtual ~AppImplMsw();
    protected:
      virtual void  run() override;
	  private:
    };

	  //! \class WindowImplMsw
	  //!
	  //!
	  class WindowImplMsw : public WindowImpl {	
	  public:
        WindowImplMsw(Window* window);
        virtual ~WindowImplMsw();
      protected:
        virtual void  setupRenderer() override;
	  private:
        HWND  mWnd = nullptr;
        HDC   mDc = nullptr;
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

namespace cinder {
  namespace app {
    namespace detail {
	    static AppParams sAppParams = {};
	  } // namespace detail

	  // ------------------------------------------------------------------------------------------------
	  // App
	  // ------------------------------------------------------------------------------------------------
    App::App() {
      mAppParams = detail::sAppParams;
      detail::sAppParams = {};
#if defined(CINDER_MSW)
	    mImpl = std::make_unique<AppImplMsw>(this, mAppParams);
#endif
	  }

    App::~App() {
	  }

    void App::run() {
      mImpl->run();
    }

	  // ------------------------------------------------------------------------------------------------
	  // AppImpl
	  // ------------------------------------------------------------------------------------------------
    AppImpl::AppImpl(App* app, const detail::AppParams& appParams)
	  	: mApp(app), mAppParams(appParams)
	  {
	  }
	  
	  AppImpl::~AppImpl() {
	  }

	  // ------------------------------------------------------------------------------------------------
	  // Window
	  // ------------------------------------------------------------------------------------------------
    Window::Window(App* app, detail::CreateRendererFn createRendererFn)
      : mApp(app)
    {
#if defined(CINDER_MSW)
	    mImpl = std::make_unique<WindowImplMsw>(this);
#endif
      mImpl->createRenderer(createRendererFn);
      mImpl->setupRenderer();
	  }

    Window::~Window() {
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
    AppImplMsw::AppImplMsw(App* app, const detail::AppParams& appParams)
      : AppImpl(app, appParams) 
	  {
        mMainWindow = std::make_unique<Window>(app, appParams.createRendererFn);
	  }

    AppImplMsw::~AppImplMsw() {
	  }

    void AppImplMsw::run() {
      MSG msg = {};
      while (true) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
          if (msg.message == WM_QUIT) {
            break;
          }

          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
        else {
        }
      }
    }

	  // ------------------------------------------------------------------------------------------------
	  // WindowImplMsw
	  // ------------------------------------------------------------------------------------------------
    static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam);

    WindowImplMsw::WindowImplMsw(Window* window)
      : WindowImpl(window)
    {
      WNDCLASSEX wc = {};
	    wc.cbSize         = sizeof(WNDCLASSEX);
	    wc.style          = CS_HREDRAW | CS_VREDRAW;
	    wc.lpfnWndProc    = WndProc;
	    wc.cbClsExtra     = NULL;
	    wc.cbWndExtra     = NULL;
	    wc.hInstance      = mWindow->getApp()->
	    wc.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
	    wc.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 2);
	    wc.lpszMenuName   = NULL;
	    wc.lpszClassName  = L"MyTestWindow";
	    wc.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);
    }

    WindowImplMsw::~WindowImplMsw() {
    }

    void WindowImplMsw::setupRenderer() {
      mRenderer->setup(mWnd, mDc, nullptr);
    }

    static LRESULT CALLBACK WndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam) {
      switch (msg) {
        case WM_KEYDOWN: {
        } break; 
        
        case WM_DESTROY: {
          PostQuitMessage(0);
          return 0;
        } break;
      }
      return DefWindowProc(wnd, msg, wParam, lParam);
    }
  } // namespace app
} // namespace cinder
#endif

#if defined(CINDER_MSW)
  #define CINDER_APP( APP, RENDERER, ... ) \
    int __stdcall WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) \
    {                                                                                                    \
      cinder::app::detail::sAppParams.hInstance = hInstance;                                             \
      cinder::app::detail::sAppParams.createRendererFn                                                   \
		    = [](cinder::app::Window* window) -> cinder::app::Renderer* {                                    \
			      return new RENDERER(window); };                                                              \
	    std::unique_ptr<ci::app::App> app = std::make_unique<APP>();                                       \
      app->run();                                                                                        \
      return 0;                                                                                          \
    }
#elif defined(CINDER_LINUX)
#endif

#endif // defined(TINYCI_IMPLEMENTATION)