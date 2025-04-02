#include "EmscriptenCompat.hpp"

// Modify the includes to be conditional
#ifndef __EMSCRIPTEN__
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>
// Other Vulkan includes
#endif
