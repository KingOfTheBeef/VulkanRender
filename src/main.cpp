#include <iostream>
#include <chrono>
#include <thread>

#include "Core.h"
#include "WindowContext.h"
#include "FileReader.h"

int main(int argc, char** argv) {

  #ifdef PLATFORM
  std::cout << "Platform detected: " << PLATFORM << std::endl;
  #endif

  Core core;
  core.init();
  // WindowContext *context = new WindowContext();
  // context->initialise("Vulkan Dynamic Link", 500, 500);

  for (int i = 0; i < 500; i++) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_WINDOWEVENT_RESIZED:
          std::cout << "WHATT" << std::endl;
          core.windowResize();
          break;
        default:
          break;
      }
    }
    core.draw();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // context->clean();
  core.clean();
  return 0;
}
