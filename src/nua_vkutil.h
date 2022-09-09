/* Purpose of this file:
 * move away Vulkan noise from other files */

#ifndef __nua_vkutil_h__
#define __nua_vkutil_h__

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <vulkan/vulkan.h>

void print_VkResult(VkResult r);

#define require_VK_SUCCESS(x) if(x != VK_SUCCESS) {\
        print_VkResult(x);\
        fprintf(stderr, "Failure in %s %s line %d\n", __FILE__, __FUNCTION__, __LINE__); \
        exit(EXIT_FAILURE); \
    }


void print_VkPresentModeKHR(VkPresentModeKHR mode);

#endif
