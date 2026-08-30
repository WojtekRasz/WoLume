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
    // --- TRYB DEBUG ---
    // Tutaj dowolnie przełączasz (1 = włączone, 0 = wyłączone)
    #define ENGINE_LOG_INFO_ENABLED    1
    #define ENGINE_LOG_WARNING_ENABLED 1
    #define ENGINE_LOG_ERROR_ENABLED   1
    #define ENGINE_LOG_VULKAN_ENABLED  1
    #define ENGINE_LOG_RENDER_ENABLED  0  
#endif

namespace config {

    // =========================================================================
    // 1. USTAWIENIA OKNA I APLIKACJI
    // =========================================================================
    inline constexpr uint32_t width  = 1280;
    inline constexpr uint32_t height = 720;
    inline constexpr const char* appName    = "Vulkan Renderer";
    inline constexpr const char* engineName = "Custom Engine";

    // =========================================================================
    // 2. VULKAN LAYERS & EXTENSIONS
    // =========================================================================
#ifdef NDEBUG
    inline constexpr bool enableValidationLayers = false;
#else
    inline constexpr bool enableValidationLayers = true;
#endif

    inline const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
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