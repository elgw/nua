/* Purpose of this file:
 * move away Vulkan noise from other files.
 * Put Vulkan functions not requiring more than a few
 * objects here */

#ifndef __nua_vkutil_h__
#define __nua_vkutil_h__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>
// https://github.com/KhronosGroup/glslang
// #include "glslang/Include/glslang_c_interface.h"

void print_VkResult(VkResult r);

#define require_VK_SUCCESS(x) if(x != VK_SUCCESS) {\
        print_VkResult(x);\
        fprintf(stderr, "Failure in %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__); \
        exit(EXIT_FAILURE); \
    }


void print_VkPresentModeKHR(VkPresentModeKHR mode);
void print_VkSampleCountFlagBits(VkSampleCountFlagBits bits);

/* Return the highest VK_SAMPLE_COUNT_X_BIT supported by the device */
VkSampleCountFlagBits get_max_usable_sample_count(VkPhysicalDevice);

void create_buffer(VkPhysicalDevice pdev,
                   VkDevice dev,
                   VkDeviceSize size,
                   VkBufferUsageFlags usage,
                   VkMemoryPropertyFlags properties,
                   VkBuffer * buffer,
                   VkDeviceMemory * bufferMemory);

uint32_t find_memory_type(VkPhysicalDevice dev,
                          uint32_t typeFilter,
                          VkMemoryPropertyFlags properties);

/* Load shader from buffer */
VkShaderModule shader_from_buffer(VkDevice dev,
                                  const uint32_t * buffer,
                                  size_t buffer_size,
                                  int verbose);

/* Load shader from file name */
VkShaderModule load_shader(VkDevice dev,
                           const char * file,
                           int verbose);
#endif
