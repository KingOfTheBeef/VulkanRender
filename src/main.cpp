#include <iostream>

#include "Core.h"
#include "WindowContext.h"

int main(int argc, char** argv) {

  #ifdef PLATFORM
  std::cout << "Platform detected: " << PLATFORM << std::endl;
  #endif

  Core *core = new Core();
  core->init();
  WindowContext *context = new WindowContext();
  context->initialise("Vulkan Dynamic Link", 600, 600);

  context->clean();
  core->clean();

  delete(core);

  return 0;
}
