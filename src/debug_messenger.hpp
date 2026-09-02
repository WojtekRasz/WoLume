#ifndef DEBUG_MESSENGER_HPP
#define DEBUG_MESSENGER_HPP

#include <vulkan/vulkan_raii.hpp>

namespace wo_lum {

  VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
      vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
      vk::DebugUtilsMessageTypeFlagsEXT type,
      const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
      void* pUserData);

  [[nodiscard]] vk::raii::DebugUtilsMessengerEXT createDebugMessenger(const vk::raii::Instance& instance);

} // namespace my_vk_app

#endif