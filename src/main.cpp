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

  core.draw();
  std::this_thread::sleep_for(std::chrono::milliseconds(5000));

  // context->clean();
  return 0;
}
