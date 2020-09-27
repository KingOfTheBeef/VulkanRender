#include <iostream>
#include <chrono>
#include <thread>

#include "Core.h"
#include "WindowContext.h"
#include "FileReader.h"
#include "tempVertexData.h"

int main(int argc, char** argv) {

  #ifdef PLATFORM
  std::cout << "Platform detected: " << PLATFORM << std::endl;
  #endif

  Core core;
  core.init();
  // WindowContext *context = new WindowContext();
  // context->initialise("Vulkan Dynamic Link", 500, 500);

  bool running = true;
  while(running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_KEYDOWN:
          core.update();
          break;
        case SDL_QUIT:
          running = false;
          break;
        default:
          break;
      }
    }
    core.draw();
  }

  // context->clean();
  core.clean();
  return 0;
}
