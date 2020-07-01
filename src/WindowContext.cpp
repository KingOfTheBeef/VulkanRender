//
// Created by jonat on 23/05/2020.
//

#include <iostream>
#include <SDL_syswm.h>
#include "WindowContext.h"

void WindowContext::initialise(const char *title, int width, int height) {
  this->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_HIDDEN);

  if (this->window == nullptr) {
    std::cout << "Window-Error: Failed to create SDL window" << std::endl;
  }

  if(!SDL_GetWindowWMInfo(this->window, &this->info)) {
    std::cout << "Window-Error: Failed to acquire SDL window info" << std::endl;
  }
}

HINSTANCE WindowContext::getWindowInstance() {
  return this->info.info.win.hinstance;
}

HWND WindowContext::getWindowHWND() {
  return this->info.info.win.window;
}

void WindowContext::clean() {
  SDL_DestroyWindow(this->window);
  SDL_Quit();
}
