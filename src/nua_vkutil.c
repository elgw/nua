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

VkSampleCountFlagBits get_max_usable_sample_count(VkPhysicalDevice pdev)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(pdev, &physicalDeviceProperties);

    VkSampleCountFlags counts =
        physicalDeviceProperties.limits.framebufferColorSampleCounts
        & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

void print_VkSampleCountFlagBits(VkSampleCountFlagBits b)
{
switch(b){
case VK_SAMPLE_COUNT_16_BIT:
    printf("VK_SAMPLE_COUNT_16_BIT\n");
    break;
case VK_SAMPLE_COUNT_1_BIT:
    printf("VK_SAMPLE_COUNT_1_BIT\n");
    break;
case VK_SAMPLE_COUNT_2_BIT:
    printf("VK_SAMPLE_COUNT_2_BIT\n");
    break;
case VK_SAMPLE_COUNT_32_BIT:
    printf("VK_SAMPLE_COUNT_32_BIT\n");
    break;
case VK_SAMPLE_COUNT_4_BIT:
    printf("VK_SAMPLE_COUNT_4_BIT\n");
    break;
case VK_SAMPLE_COUNT_64_BIT:
    printf("VK_SAMPLE_COUNT_64_BIT\n");
    break;
case VK_SAMPLE_COUNT_8_BIT:
    printf("VK_SAMPLE_COUNT_8_BIT\n");
    break;
case VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM:
    printf("VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM\n");
    break;
}
return;
}


void create_buffer(VkPhysicalDevice pdev,
                   VkDevice dev,
                   VkDeviceSize size,
                   VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkBuffer * buffer,
                   VkDeviceMemory * bufferMemory)
{

    VkBufferCreateInfo bufferInfo = {};

    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


    if (vkCreateBuffer(dev, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        fprintf(stderr, "failed to create buffer on line %d", __LINE__);
    }
    //    printf("buffer at %p\n", (void *) buffer);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(dev, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(pdev,
                                               memRequirements.memoryTypeBits,
                                               properties);


    if (vkAllocateMemory(dev,
                         &allocInfo,
                         NULL,
                         bufferMemory) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate buffer memory on line %d",
                __LINE__);
    }

    vkBindBufferMemory(dev, *buffer, *bufferMemory, 0);
    return;
}

uint32_t find_memory_type(VkPhysicalDevice dev,
                        uint32_t typeFilter,
                        VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(dev,
                                        &memProperties);
    // What is this? TODO understand
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    fprintf(stderr, "failed to find a suitable memory type\n");
    exit(EXIT_FAILURE);
    return 0;
}

VkShaderModule load_shader(VkDevice dev, const char * file, int verbose)
{
    if(verbose > 1)
    {
        printf("load_shader: trying to open %s\n", file);
    }

    FILE * fid = fopen(file, "r");
    if(fid == NULL)
    {
        fprintf(stderr, "! Error\nFailed to open the shader file %s. "
                "The shader should be in spv format.\n",
                file);
        exit(EXIT_FAILURE);
    }

    fseek(fid, 0, SEEK_END);
    long fsize = ftell(fid);
    fseek(fid, 0, SEEK_SET);

    size_t buff_size = fsize +1;
    uint32_t * buffer = calloc(buff_size, 1);
    assert(fsize % 4 == 0);
    size_t codesize = fread(buffer, 4, fsize/4, fid);
    if(verbose > 2)
    {
        printf("Read %zu bytes\n", codesize*4);
    }
    assert(codesize < buff_size);
    fclose(fid);

    VkShaderModule shader = shader_from_buffer(dev,
                                               buffer,
                                               codesize,
                                               verbose);
    free(buffer);
    return shader;
}

VkShaderModule shader_from_buffer(VkDevice dev,
                                      const uint32_t * buffer,
                                      size_t buffer_size,
                                      int verbose)
 {
     if(verbose > 2)
     {
         printf("shader_from_buffer\n");
     }

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer_size*4;
    createInfo.pCode = buffer;
    VkShaderModule shaderModule;
    int status = vkCreateShaderModule(dev,
                                      &createInfo,
                                      NULL,
                                      &shaderModule);
    require_VK_SUCCESS(status);
    return shaderModule;
}
