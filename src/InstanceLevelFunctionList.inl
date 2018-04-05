#ifndef VK_INSTANCE_LEVEL_FUNCTION
#define VK_INSTANCE_LEVEL_FUNCTION( fun )
#endif // !VK_INSTANCE_LEVEL_FUNCTION

VK_INSTANCE_LEVEL_FUNCTION(vkDestroyInstance)

VK_INSTANCE_LEVEL_FUNCTION(vkEnumeratePhysicalDevices)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceProperties)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceFeatures)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceFormatProperties)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceMemoryProperties)

VK_INSTANCE_LEVEL_FUNCTION(vkCreateDevice)

VK_INSTANCE_LEVEL_FUNCTION(vkEnumerateDeviceExtensionProperties)

VK_INSTANCE_LEVEL_FUNCTION(vkEnumerateDeviceLayerProperties)

VK_INSTANCE_LEVEL_FUNCTION(vkGetDeviceProcAddr)

/* Debug report callback extension */

VK_INSTANCE_LEVEL_FUNCTION(vkCreateDebugReportCallbackEXT)

VK_INSTANCE_LEVEL_FUNCTION(vkDestroyDebugReportCallbackEXT)

/* Surface extensions */

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR)

VK_INSTANCE_LEVEL_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR)

VK_INSTANCE_LEVEL_FUNCTION(vkDestroySurfaceKHR)

#if defined(VK_USE_PLATFORM_WIN32_KHR)
VK_INSTANCE_LEVEL_FUNCTION(vkCreateWin32SurfaceKHR)
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
VK_INSTANCE_LEVEL_FUNCTION(vkCreateWaylandSurfaceKHR)
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
VK_INSTANCE_LEVEL_FUNCTION(vkCreateXlibSurfaceKHR)
#endif


#undef VK_INSTANCE_LEVEL_FUNCTION