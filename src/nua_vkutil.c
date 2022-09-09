#include "nua_vkutil.h"

void print_VkResult(VkResult r)
{
    // generated by gen_print_swich.py
    // data from https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkResult.html
    switch(r)
    {
#ifdef VK_ERROR_COMPRESSION_EXHAUSTED_EXT
    case VK_ERROR_COMPRESSION_EXHAUSTED_EXT:
        printf("VK_ERROR_COMPRESSION_EXHAUSTED_EXT\n");
        break;
#endif
#ifdef VK_ERROR_DEVICE_LOST
    case VK_ERROR_DEVICE_LOST:
        printf("VK_ERROR_DEVICE_LOST\n");
        break;
#endif
#ifdef VK_ERROR_EXTENSION_NOT_PRESENT
    case VK_ERROR_EXTENSION_NOT_PRESENT:
        printf("VK_ERROR_EXTENSION_NOT_PRESENT\n");
        break;
#endif
#ifdef VK_ERROR_FEATURE_NOT_PRESENT
    case VK_ERROR_FEATURE_NOT_PRESENT:
        printf("VK_ERROR_FEATURE_NOT_PRESENT\n");
        break;
#endif
#ifdef VK_ERROR_FORMAT_NOT_SUPPORTED
    case VK_ERROR_FORMAT_NOT_SUPPORTED:
        printf("VK_ERROR_FORMAT_NOT_SUPPORTED\n");
        break;
#endif
#ifdef VK_ERROR_FRAGMENTATION
    case VK_ERROR_FRAGMENTATION:
        printf("VK_ERROR_FRAGMENTATION\n");
        break;
#endif
#ifdef VK_ERROR_FRAGMENTATION_EXT
    case VK_ERROR_FRAGMENTATION_EXT:
        printf("VK_ERROR_FRAGMENTATION_EXT\n");
        break;
#endif
#ifdef VK_ERROR_FRAGMENTED_POOL
    case VK_ERROR_FRAGMENTED_POOL:
        printf("VK_ERROR_FRAGMENTED_POOL\n");
        break;
#endif
#ifdef VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT
    case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
        printf("VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT\n");
        break;
#endif
#ifdef VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR
    case VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR:
        printf("VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR\n");
        break;
#endif
#ifdef VK_ERROR_INCOMPATIBLE_DISPLAY_KHR
    case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
        printf("VK_ERROR_INCOMPATIBLE_DISPLAY_KHR\n");
        break;
#endif
#ifdef VK_ERROR_INCOMPATIBLE_DRIVER
    case VK_ERROR_INCOMPATIBLE_DRIVER:
        printf("VK_ERROR_INCOMPATIBLE_DRIVER\n");
        break;
#endif
#ifdef VK_ERROR_INITIALIZATION_FAILED
    case VK_ERROR_INITIALIZATION_FAILED:
        printf("VK_ERROR_INITIALIZATION_FAILED\n");
        break;
#endif
#ifdef VK_ERROR_INVALID_DEVICE_ADDRESS_EXT
    case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
        printf("VK_ERROR_INVALID_DEVICE_ADDRESS_EXT\n");
        break;
#endif
#ifdef VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT
    case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
        printf("VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT\n");
        break;
#endif
#ifdef VK_ERROR_INVALID_EXTERNAL_HANDLE
    case VK_ERROR_INVALID_EXTERNAL_HANDLE:
        printf("VK_ERROR_INVALID_EXTERNAL_HANDLE\n");
        break;
#endif
#ifdef VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR
    case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:
        printf("VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR\n");
        break;
#endif
#ifdef VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
        printf("VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS\n");
        break;
#endif
#ifdef VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR
    case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR:
        printf("VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR\n");
        break;
#endif
#ifdef VK_ERROR_INVALID_SHADER_NV
    case VK_ERROR_INVALID_SHADER_NV:
        printf("VK_ERROR_INVALID_SHADER_NV\n");
        break;
#endif
#ifdef VK_ERROR_LAYER_NOT_PRESENT
    case VK_ERROR_LAYER_NOT_PRESENT:
        printf("VK_ERROR_LAYER_NOT_PRESENT\n");
        break;
#endif
#ifdef VK_ERROR_MEMORY_MAP_FAILED
    case VK_ERROR_MEMORY_MAP_FAILED:
        printf("VK_ERROR_MEMORY_MAP_FAILED\n");
        break;
#endif
#ifdef VK_ERROR_NATIVE_WINDOW_IN_USE_KHR
    case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
        printf("VK_ERROR_NATIVE_WINDOW_IN_USE_KHR\n");
        break;
#endif
#ifdef VK_ERROR_NOT_PERMITTED_EXT
    case VK_ERROR_NOT_PERMITTED_EXT:
        printf("VK_ERROR_NOT_PERMITTED_EXT\n");
        break;
#endif
#ifdef VK_ERROR_NOT_PERMITTED_KHR
    case VK_ERROR_NOT_PERMITTED_KHR:
        printf("VK_ERROR_NOT_PERMITTED_KHR\n");
        break;
#endif
#ifdef VK_ERROR_OUT_OF_DATE_KHR
    case VK_ERROR_OUT_OF_DATE_KHR:
        printf("VK_ERROR_OUT_OF_DATE_KHR\n");
        break;
#endif
#ifdef VK_ERROR_OUT_OF_DEVICE_MEMORY
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
        printf("VK_ERROR_OUT_OF_DEVICE_MEMORY\n");
        break;
#endif
#ifdef VK_ERROR_OUT_OF_HOST_MEMORY
    case VK_ERROR_OUT_OF_HOST_MEMORY:
        printf("VK_ERROR_OUT_OF_HOST_MEMORY\n");
        break;
#endif
#ifdef VK_ERROR_OUT_OF_POOL_MEMORY
    case VK_ERROR_OUT_OF_POOL_MEMORY:
        printf("VK_ERROR_OUT_OF_POOL_MEMORY\n");
        break;
#endif
#ifdef VK_ERROR_OUT_OF_POOL_MEMORY_KHR
    case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:
        printf("VK_ERROR_OUT_OF_POOL_MEMORY_KHR\n");
        break;
#endif
#ifdef VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT
    case VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT:
        printf("VK_ERROR_PIPELINE_COMPILE_REQUIRED_EXT\n");
        break;
#endif
#ifdef VK_ERROR_SURFACE_LOST_KHR
    case VK_ERROR_SURFACE_LOST_KHR:
        printf("VK_ERROR_SURFACE_LOST_KHR\n");
        break;
#endif
#ifdef VK_ERROR_TOO_MANY_OBJECTS
    case VK_ERROR_TOO_MANY_OBJECTS:
        printf("VK_ERROR_TOO_MANY_OBJECTS\n");
        break;
#endif
#ifdef VK_ERROR_UNKNOWN
    case VK_ERROR_UNKNOWN:
        printf("VK_ERROR_UNKNOWN\n");
        break;
#endif
#ifdef VK_ERROR_VALIDATION_FAILED_EXT
    case VK_ERROR_VALIDATION_FAILED_EXT:
        printf("VK_ERROR_VALIDATION_FAILED_EXT\n");
        break;
#endif
#ifdef VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR
    case VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR:
        printf("VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR\n");
        break;
#endif
#ifdef VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR
    case VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR:
        printf("VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR\n");
        break;
#endif
#ifdef VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR
    case VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR:
        printf("VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR\n");
        break;
#endif
#ifdef VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR
    case VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR:
        printf("VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR\n");
        break;
#endif
#ifdef VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR
    case VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR:
        printf("VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR\n");
        break;
#endif
#ifdef VK_EVENT_RESET
    case VK_EVENT_RESET:
        printf("VK_EVENT_RESET\n");
        break;
#endif
#ifdef VK_EVENT_SET
    case VK_EVENT_SET:
        printf("VK_EVENT_SET\n");
        break;
#endif
#ifdef VK_INCOMPLETE
    case VK_INCOMPLETE:
        printf("VK_INCOMPLETE\n");
        break;
#endif
#ifdef VK_NOT_READY
    case VK_NOT_READY:
        printf("VK_NOT_READY\n");
        break;
#endif
#ifdef VK_OPERATION_DEFERRED_KHR
    case VK_OPERATION_DEFERRED_KHR:
        printf("VK_OPERATION_DEFERRED_KHR\n");
        break;
#endif
#ifdef VK_OPERATION_NOT_DEFERRED_KHR
    case VK_OPERATION_NOT_DEFERRED_KHR:
        printf("VK_OPERATION_NOT_DEFERRED_KHR\n");
        break;
#endif
#ifdef VK_PIPELINE_COMPILE_REQUIRED
    case VK_PIPELINE_COMPILE_REQUIRED:
        printf("VK_PIPELINE_COMPILE_REQUIRED\n");
        break;
#endif
#ifdef VK_PIPELINE_COMPILE_REQUIRED_EXT
    case VK_PIPELINE_COMPILE_REQUIRED_EXT:
        printf("VK_PIPELINE_COMPILE_REQUIRED_EXT\n");
        break;
#endif
#ifdef VK_SUBOPTIMAL_KHR
    case VK_SUBOPTIMAL_KHR:
        printf("VK_SUBOPTIMAL_KHR\n");
        break;
#endif
#ifdef VK_THREAD_DONE_KHR
    case VK_THREAD_DONE_KHR:
        printf("VK_THREAD_DONE_KHR\n");
        break;
#endif
#ifdef VK_THREAD_IDLE_KHR
    case VK_THREAD_IDLE_KHR:
        printf("VK_THREAD_IDLE_KHR\n");
        break;
#endif
#ifdef VK_TIMEOUT
    case VK_TIMEOUT:
        printf("VK_TIMEOUT\n");
        break;
#endif
#ifdef VK_SUCCESS
    case VK_SUCCESS:
        printf("VK_SUCCESS\n");
        break;
#endif
    default:
        printf("Error code %d not described by print_VK_result\n", r);
    }
    return;
}


void print_VkPresentModeKHR(VkPresentModeKHR mode)
{
    switch(mode)
    {
    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        printf("VK_PRESENT_MODE_IMMEDIATE_KHR\n");
        break;
    case VK_PRESENT_MODE_MAILBOX_KHR:
        printf("VK_PRESENT_MODE_MAILBOX_KHR\n");
        break;
    case VK_PRESENT_MODE_FIFO_KHR:
        printf("VK_PRESENT_MODE_FIFO_KHR\n");
        break;
    case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
        printf("VK_PRESENT_MODE_FIFO_RELAXED_KHR\n");
        break;
    default:
        printf("Other present mode\n");
    }
    return;
}
