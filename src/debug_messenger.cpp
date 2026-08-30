#include "debug_messenger.hpp"

namespace my_vk_app {

  DebugMessenger::DebugMessenger(vk::raii::Instance& instance) {
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
      vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
    );

    vk::DebugUtilsMessageTypeFlagsEXT messageTypeFlags(
      vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
      vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
      vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation
    );

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{
      .messageSeverity = severityFlags,
      .messageType     = messageTypeFlags,
      .pfnUserCallback = &DebugMessenger::debugCallback
    };

    messenger = instance.createDebugUtilsMessengerEXT(createInfo);
  }

  VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugMessenger::debugCallback(
    vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
    vk::DebugUtilsMessageTypeFlagsEXT type,
    const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
  {
    std::cerr << ((severity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) ? "\033[31m[ERROR]" : "\033[33m[WARNING]")
              << " Validation Layer (" << to_string(type) << "):\n\t"
              << pCallbackData->pMessage << "\033[0m\n\n";
    return vk::False;
  }

} // namespace my_vk_app