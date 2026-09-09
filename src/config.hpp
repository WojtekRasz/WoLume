#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <string_view>
#include <cstdint>

#ifdef NDEBUG
    #define ENGINE_LOG_INFO_ENABLED    0
    #define ENGINE_LOG_WARNING_ENABLED 0
    #define ENGINE_LOG_ERROR_ENABLED   1
    #define ENGINE_LOG_VULKAN_ENABLED  0
    #define ENGINE_LOG_RENDER_ENABLED  0
#else
    #define ENGINE_LOG_INFO_ENABLED    1
    #define ENGINE_LOG_WARNING_ENABLED 1
    #define ENGINE_LOG_ERROR_ENABLED   1
    #define ENGINE_LOG_VULKAN_ENABLED  1
    #define ENGINE_LOG_RENDER_ENABLED  0
#endif

namespace config {

    inline constexpr uint32_t width  = 1280;
    inline constexpr uint32_t height = 720;
    inline constexpr const char* appName    = "Vulkan Renderer";
    inline constexpr const char* engineName = "Custom Engine";

    constexpr int MAX_FRAMES_IN_FLIGHT = 2;

#ifdef NDEBUG
    inline constexpr bool enableValidationLayers = false;
#else
    inline constexpr bool enableValidationLayers = true;
#endif

    inline const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    inline const std::vector<const char*> requiredDeviceExtension = {
        vk::KHRSwapchainExtensionName
    };

    inline vk::ApplicationInfo getAppInfo() {
        return vk::ApplicationInfo{}
            .setPApplicationName(appName)
            .setApplicationVersion(VK_MAKE_VERSION(1, 0, 0))
            .setPEngineName(engineName)
            .setEngineVersion(VK_MAKE_VERSION(1, 0, 0))
            .setApiVersion(VK_API_VERSION_1_3);
    }
} // namespace Config