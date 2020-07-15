//
// Created by jonat on 23/05/2020.
//

#ifndef DYNAMICLINK_WINDOWCONTEXT_H
#define DYNAMICLINK_WINDOWCONTEXT_H

#include "SDL.h"
#include "SDL_syswm.h"

class WindowContext {
private:
  SDL_Window *window;
  SDL_SysWMinfo info;

public:
    void initialise(const char* title, int width, int height);
    void idleWhileMinimised();
    bool isMinimised() {return SDL_GetWindowFlags(this->window) & SDL_WINDOW_MINIMIZED;} ;
    void clean();
#ifdef VK_USE_PLATFORM_WIN32_KHR
    HINSTANCE getWindowInstance();
    HWND getWindowHWND();
#endif
};

#endif //DYNAMICLINK_WINDOWCONTEXT_H
