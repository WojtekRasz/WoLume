#ifndef DEBUG_MESSENGER_HPP
#define DEBUG_MESSENGER_HPP

#include <vulkan/vulkan_raii.hpp>
#include <iostream>

namespace my_vk_app {

  class DebugMessenger {
  public:
    DebugMessenger() = default;
    DebugMessenger(vk::raii::Instance& instance);

    static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
      vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
      vk::DebugUtilsMessageTypeFlagsEXT type,
      const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData);

  private:
    vk::raii::DebugUtilsMessengerEXT messenger = nullptr;
  };

} // namespace my_vk_app

#endif