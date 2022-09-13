#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "nua.h"

#define UNUSED __attribute__((unused))


void nua_setup_graphics(nua_t * p)
{
    unit_tests();
    /* Setup the window  */
    setup_SDL(p);
    /* Setup the vulkan context */
    init_vulkan(p);
}

void nua_run(nua_t * p)
{
    if(p->msaaSamples == VK_SAMPLE_COUNT_1_BIT)
    {
        nua_fixme;
        printf("Warning: need to have at least 2-but MSAA\n");
        p->msaaSamples = VK_SAMPLE_COUNT_2_BIT;
    }

    nua_setup_graphics(p);
    nua_show_usage(p);

    /* Add Spheres */
    p->balls = NULL;
    if(p->bead_data != NULL && p->nbeads > 0)
    {
        p->balls = nua_obj_new(p, NUA_OBJECT_BALL, p->nbeads, p->bead_data);
    }
    /* Add Links */
    p->links = NULL;
    if(p->link_data != NULL && p->nlinks > 0)
    {
        p->links = nua_obj_new(p, NUA_OBJECT_CONNECT, p->nlinks, p->link_data);
    }
    /* Add geometry */
    p->domain = NULL;
    p->domain = nua_obj_new(p, NUA_OBJECT_DOMAIN, 0, NULL);

    nua_main_loop(p);
    return;
}

void nua_destroy(nua_t * p)
{
    /* Clean up spheres*/
    if(p->balls != NULL)
    {
        nua_obj_free(p->balls, p);
    }
    if(p->links != NULL)
    {
        nua_obj_free(p->links, p);
    }
    if(p->domain != NULL)
    {
        nua_obj_free(p->domain, p);
    }

    /* Cleanup graphics  */
    nua_destroy_graphics(p);
}

nua_t * nua_new()
{
    nua_t * p = calloc(1, sizeof(nua_t));
    /* General program settings */
    p->oneframe = 0;
    p->validation_layers = 1;
    p->zoom = 1.0;
    p->fullscreen = 0;
    p->verbose = 1;
    p->quit = 0;
    p->msaa_want = 8;
    p->pause = 0;
    p->spherediv = 2;
    p->linkdiv = 5;

    p->proj_ortho = 1;

    p->show_domain = 1;
    p->show_beads = 1;
    p->show_links = 1;

    /* Vulkan settings */
    p->msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    p->vkEnableValidationLayers = 1;
    p->window = NULL;
    p->vkPDevice = VK_NULL_HANDLE;
    /* Each frame has its own command buffer, set of semaphores, and fence. */
    p->frames_in_flight = 2;

    /* Graphics state variables */
    p->current_frame = 0;
    p->framebuffer_resized = 0;

    clock_gettime(CLOCK_REALTIME, &p->time_start);

    p->n_allocate_memory = 0;
    p->n_free_memory = 0;
    p->n_create_buffer = 0;
    p->n_destroy_buffer = 0;
    p->n_image_create = 0;;
    p->n_image_destroy = 0;
    p->n_image_view_create = 0;
    p->n_image_view_destroy = 0;

    return p;
}

void nua_show_usage(UNUSED nua_t * p)
{
    printf(
           " User interface:\n"
           "      <q>  quit                       <F11>  toggle fullscreen    \n"
           "      <t>  zoom in                     <g>  zoom out              \n"
           "      <1>  show/hide domain            <2>  show/hide links       \n"
           "      <3>  show/hide beads                                        \n"
           "      <0>  toggle projection type                                 \n");
    return;
}


void setup_VK_physical_device(nua_t * p)
{
    uint32_t deviceCount = 0;

    int status = vkEnumeratePhysicalDevices(p->vkInstance, &deviceCount, NULL);
    require_VK_SUCCESS(status);

    if(p->verbose > 2)
    {
        printf("Found %u devices\n", deviceCount);
    }

    VkPhysicalDevice * devices = calloc(deviceCount, sizeof(VkPhysicalDevice));

    status = vkEnumeratePhysicalDevices(p->vkInstance, &deviceCount, devices);
    assert(status == VK_SUCCESS);

    int device_selected = -1;
    for(int kk = 0; kk< (int) deviceCount; kk++)
    {
        VkPhysicalDeviceProperties * props = calloc(1, sizeof(VkPhysicalDeviceProperties));
        vkGetPhysicalDeviceProperties(devices[kk], props);
        if(p->verbose > 2)
        {
            printf("%d %s\n", kk, props->deviceName);
        }
        if(props->deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            device_selected = kk;
        }
        free(props);
    }
    assert(device_selected > -1);
    if(p->verbose > 2)
    {
        printf("device %d selected\n", device_selected);
    }
    fflush(stdout);
    p->vkPDevice = devices[device_selected];
    //p->msaaSamples = get_max_usable_sample_count(p->vkPDevice);
    free(devices);
    return;
}

void setup_VK_logical_device(nua_t * p)
{
    if(p->verbose > 2)
    {
        printf("-> Setting up VK logical device\n");
    }
    /* 1. Set up queues to be created */
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = p->queue_fami.graphics;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.pEnabledFeatures = &deviceFeatures;

    uint32_t extensioncount = 0;
    const char ** extensions = vec_char_add(NULL,
                                            &extensioncount,
                                            VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    if(p->verbose > 2)
    {
        printf("Extensions to be used\n");
        for(int kk = 0; kk< (int) extensioncount; kk++)
        {
            printf("%d: %s\n", kk, extensions[kk]);
        }
        fflush(stdout);
    }
    createInfo.enabledExtensionCount = extensioncount;
    createInfo.ppEnabledExtensionNames = extensions;


    if (vkCreateDevice(p->vkPDevice, &createInfo, NULL, &p->vkDevice)
        != VK_SUCCESS) {
        fprintf(stderr, "failed to create logical device!");
        exit(EXIT_FAILURE);
    }
    free(extensions);
    return;
}

static int resizingEventWatcher(void* data, SDL_Event* event) {
    nua_t * p = (nua_t *) data;
    if (event->type == SDL_WINDOWEVENT &&
        event->window.event == SDL_WINDOWEVENT_RESIZED) {
        if(p->verbose > 2)
        {
            printf("resizing.....\n");
        }
        p->framebuffer_resized = 1;
    }
    return 0;
}

void setup_SDL(nua_t * p)
{
    SDL_Init(SDL_INIT_VIDEO);
    p->data_changed_event = SDL_RegisterEvents(1);

    p->window = SDL_CreateWindow("nua",
                                 SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                 512, 512,
                                 SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
    SDL_AddEventWatch(resizingEventWatcher, p);

    assert(p->window != NULL);
    if(p->verbose > 2)
    {
        printf("SDL Window created\n");
    }
}


void setup_VK_queue(nua_t * p)
{
    // Look for VK_QUEUE_GRAPHICS_BIT
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(p->vkPDevice,
                                             &queueFamilyCount, NULL);

    VkQueueFamilyProperties*
        queueFamilies = calloc(queueFamilyCount,
                               sizeof(VkQueueFamilyProperties));
    vkGetPhysicalDeviceQueueFamilyProperties(p->vkPDevice,
                                             &queueFamilyCount,
                                             queueFamilies);
    if(p->verbose > 2)
    {
        printf("Found %u queue families\n", queueFamilyCount);
    }
    for(int kk = 0; kk<(int) queueFamilyCount; kk++)
    {
        if(p->verbose > 2)
        {
            printf("%d %u ", kk, queueFamilies[kk].queueFlags);
        }
        if(queueFamilies[kk].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            if(p->verbose > 2)
            {
                printf("has VK_QUEUE_GRAPHICS_BIT");
            }
            p->queue_fami.graphics = kk;

        }
        if(p->verbose > 2)
        {
            printf("\n");
        }
    }
    free(queueFamilies);
}

void setup_VK_validation_layers(nua_t * p)
{
    /* We will only check for
       VK_LAYER_KHRONOS_validation */
    assert(p!=NULL);

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);
    if(p->verbose > 2)
    {
        printf("Found %u validation layers\n", layerCount);
    }

    VkLayerProperties * availableLayers =
        calloc(layerCount,
               sizeof(VkLayerProperties));

    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    int foundit = 0;
    for(int kk = 0; kk < (int) layerCount; kk++)
    {
        char * name = availableLayers[kk].layerName;
        if(p->verbose > 2)
        {
            printf("%d : %s\n", kk, name);
        }
        if(strcmp(name, "VK_LAYER_KHRONOS_validation") == 0)
        {
            foundit = 1;
        }
    }
    if(foundit == 0)
    {
        printf("Could not find VK_LAYER_KHRONOS_validation\n");
    }
    if(p->verbose > 1)
    {
        printf("VK_LAYER_KHRONOS_validation is present\n");
    }
    fflush(stdout);

    free(availableLayers);
    return;
}

void setup_VK_instance(nua_t * p)
{

    // Check that "VK_LAYER_KHRONOS_validation" is available
    setup_VK_validation_layers(p);

    uint32_t extensioncount;
    if (!SDL_Vulkan_GetInstanceExtensions(p->window, &extensioncount, NULL))
    {
        fprintf(stderr, "!SDL_Vulkan_GetInstanceExtensions\n");
        exit(EXIT_FAILURE);
    }
    if(p->verbose > 2)
    {
        printf("SDL requires %d extensions\n", extensioncount);
    }
    const char ** extensions_sdl = calloc(extensioncount,
                                          sizeof(char*));
    int status = SDL_Vulkan_GetInstanceExtensions(p->window,
                                                  &extensioncount,
                                                  extensions_sdl);
    if(status != SDL_TRUE){
        fprintf(stderr, "!SDL_Vulkan_GetInstanceExtensions (2nd)\n");
        exit(EXIT_FAILURE);
    }
    if(p->verbose > 2)
    {
        for(int kk = 0; kk< (int) extensioncount; kk++)
        {
            printf("%d: %s\n", kk, extensions_sdl[kk]);
        }
    }
    fflush(stdout);

    const char ** extensions = extensions_sdl;


    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;


    int layercount = 1;
    if(p->validation_layers)
    {
        if(p->verbose > 1)
        {
            printf("Enabling validation layer(s)\n");
        }
        createInfo.enabledLayerCount = layercount;
    } else {
        if(p->verbose > 1)
        {
            printf("Validation layer(s) disabled\n");
        }
        createInfo.enabledLayerCount = 0;
    }
    char ** layernames = calloc(layercount,
                                sizeof(char*));
    char * l1 = calloc(1024, 1);
    sprintf(l1, "VK_LAYER_KHRONOS_validation");
    //sprintf(l1, "VK_LAYER_LUNARG_monitor");
    layernames[0] = l1;
    if(layercount > 1)
    {
        char * l2 = calloc(1024, 1);
        sprintf(l2, "VK_LAYER_LUNARG_api_dump");
        layernames[1] = l2;
    }

    createInfo.ppEnabledLayerNames = (const char * const*) layernames;

    createInfo.enabledExtensionCount = extensioncount;
    createInfo.ppEnabledExtensionNames = extensions;

    status = vkCreateInstance(&createInfo, NULL, &p->vkInstance);
    require_VK_SUCCESS(status);

    if(p->verbose > 2)
    {
        printf("VKInstance created\n");
    }
    free(l1);
    free(layernames);

    free(extensions_sdl);
    return;
}

void setup_VK_surface(nua_t * p)
{
    SDL_Vulkan_LoadLibrary(NULL);
    SDL_bool got_surface = SDL_Vulkan_CreateSurface(p->window,
                                                    p->vkInstance,
                                                    &p->vkSurface);
    if(got_surface != SDL_TRUE)
    {
        fprintf(stderr, "SDL_Vulkan_CreateSurface failed on line %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }
}

void query_VK_SwapChainSupport(nua_t * p)
{
    if(p->verbose > 2)
    {
        printf("-> Querying swap chain support\n");
    }
    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p->vkPDevice,
                                              p->vkSurface,
                                              &capabilities);
    p->swap_conf.capabilities = capabilities;
    if(p->verbose > 2)
    {
        printf("minImageCount: %u, maxImageCount: %u\n",
               capabilities.minImageCount,
               capabilities.maxImageCount);
    }
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(p->vkPDevice,
                                         p->vkSurface,
                                         &formatCount,
                                         NULL);
    if(p->verbose > 2)
    {
        printf("-> %u surface formats supported\n", formatCount);
    }

    VkSurfaceFormatKHR * formats = calloc(formatCount,
                                          sizeof(VkSurfaceFormatKHR));
    vkGetPhysicalDeviceSurfaceFormatsKHR(p->vkPDevice, p->vkSurface,
                                         &formatCount, formats);
    if(p->verbose > 2)
    {
        for(uint32_t kk = 0; kk<formatCount; kk++)
        {
            printf("%d: VkFormat: %d\n", kk, formats[kk].format);
        }
    }

    p->swap_conf.surfaceFormat = formats[0];
    p->swap_conf.swapChainImageFormat = formats[0].format;
    free(formats);
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(p->vkPDevice, p->vkSurface,
                                              &presentModeCount, NULL);

    assert(presentModeCount > 0);

    VkPresentModeKHR*
        modes = calloc(presentModeCount,
                       sizeof(VkPresentModeKHR));
    vkGetPhysicalDeviceSurfacePresentModesKHR(p->vkPDevice, p->vkSurface,
                                              &presentModeCount,
                                              modes);
    if(p->verbose > 2)
    {
        printf("-> %u surface present modes supported:\n", presentModeCount);
        for(uint32_t kk = 0; kk < presentModeCount; kk++)
        {
            print_VkPresentModeKHR(modes[kk]);
        }
    }
    free(modes);
    /* Locked to vsync */
    p->swap_conf.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    /* As fast as possible, 4500 FPS ... */
    //p->swap_conf.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    {
        int w = 0;
        int h = 0;
        //SDL_GetWindowSize(p->window, &w, &h);
        SDL_Vulkan_GetDrawableSize(p->window, &w, &h);
        VkExtent2D ext = {};
        ext.width = w;
        ext.height = h;
        p->swap_conf.extent = ext;
    }
}

void setup_VK_present_queue(nua_t * p)
{
    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(p->vkPDevice,
                                         p->queue_fami.graphics,
                                         p->vkSurface,
                                         &presentSupport);
    if(presentSupport == true)
    {
        if(p->verbose > 2)
        {
            printf("Good, device supports presentation as well\n");
        }
    } else {
        fprintf(stderr, "error no vkGetPhysicalDeviceSurfaceSupportKHR\n");
        exit(EXIT_FAILURE);
    }
    p->queue_fami.present = p->queue_fami.graphics;

    vkGetDeviceQueue(p->vkDevice, p->queue_fami.present, 0,
                     &p->vkPresentQueue);
    // Todo: create presentQueue
    // https://vulkan-tutorial.com/en/Drawing_a_triangle/Presentation/Window_surface
}

/* Check what size the window has */
int get_window_size(nua_t * p, int * w, int * h)
{
    VkSurfaceCapabilitiesKHR capabilities = {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(p->vkPDevice,
                                              p->vkSurface,
                                              &capabilities);
    int w_VK = capabilities.currentExtent.width;
    int h_VK = capabilities.currentExtent.height;

    /*
    // SDL can't be trusted. We use what we get from Vulkan
    int w_SDL = 0;
    int h_SDL = 0;
    SDL_Vulkan_GetDrawableSize(p->window, &w_SDL, &h_SDL);
    */
    *w = w_VK;
    *h = h_VK;

    return EXIT_SUCCESS;
}

int create_swap_chain(nua_t * p)
{

    int w = 0;
    int h = 0;
    get_window_size(p, &w, &h);

    p->swap_conf.extent.width = w;
    p->swap_conf.extent.height = h;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = p->vkSurface;
    createInfo.minImageCount = p->frames_in_flight;
    createInfo.imageFormat = p->swap_conf.surfaceFormat.format;
    createInfo.imageColorSpace = p->swap_conf.surfaceFormat.colorSpace;
    createInfo.imageExtent = p->swap_conf.extent;
    if(p->verbose > 2)
    {
        printf("Creating a swap chain for %u x %u\n",
               p->swap_conf.extent.width,
               p->swap_conf.extent.height);
    }

    createInfo.imageArrayLayers = 1; // > 1 for stereo
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // simplest, only one queue family
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0; // Optional
    createInfo.pQueueFamilyIndices = NULL; // Optional

    createInfo.preTransform = p->swap_conf.capabilities.currentTransform;
    createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;

    // Blending with other windows in the windowing system
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = p->swap_conf.presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    // vkGetPhysicalDeviceSurfaceCapabilitiesKHR

    int status = vkCreateSwapchainKHR(p->vkDevice,
                                      &createInfo,
                                      NULL,
                                      &p->vkSwapChain);

    if(status != VK_SUCCESS)
    {
        /* This is expected sometimes and not fatal */
        if(status == VK_ERROR_INITIALIZATION_FAILED)
        {
            print_VkResult(status);
            return EXIT_FAILURE;
        }
        exit(EXIT_FAILURE);
    }

    uint32_t imageCount = 0;
    vkGetSwapchainImagesKHR(p->vkDevice,
                            p->vkSwapChain,
                            &imageCount,
                            NULL);

    if(p->frames_in_flight < (int) imageCount)
    {
        fprintf(stderr, "Error: the swap chain has fewer images than wanted\n");
        exit(EXIT_FAILURE);
    }
    p->swap_conf.swapChainImages = calloc(imageCount*sizeof(VkImage*), 1);

    vkGetSwapchainImagesKHR(p->vkDevice,
                            p->vkSwapChain,
                            &imageCount,
                            p->swap_conf.swapChainImages);

    create_image_views(p);
    create_color_resource(p);
    create_depth_resources(p);
    create_framebuffers(p);
    return EXIT_SUCCESS;
}


void destroy_swap_chain(nua_t * p)
{

    free(p->swap_conf.swapChainImages);
    vkDestroySwapchainKHR(p->vkDevice, p->vkSwapChain, NULL);
    destroy_image_views(p);
    destroy_color_resource(p);
    destroy_depth_resource(p);
    destroy_framebuffers(p);
}

void nua_destroy_graphics(nua_t * p)
{

    SDL_DestroyWindow(p->window);
    destroy_sync_objects(p);
    destroy_uniform_buffers(p);
    destroy_render_pass(p);
    destroy_swap_chain(p);
    destroy_command_pool(p);
    free(p->commandBuffers);
    vkDeviceWaitIdle(p->vkDevice);
    if(p->verbose > 2)
    {
        printf("p->vkDevice: %p\n", (void *) p->vkDevice);
    }

    vkDestroyDevice(p->vkDevice, NULL);
    vkDestroySurfaceKHR(p->vkInstance, p->vkSurface, NULL);
    vkDestroyInstance(p->vkInstance, NULL);
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();

    if(p->verbose > 1)
    {
        printf("Allocation summary:\n");
        printf("n_allocate_memory: %zu, n_free_memory: %zu\n",
               p->n_allocate_memory,
               p->n_free_memory);
        printf("n_create_buffer: %zu, n_destroy_buffer: %zu\n",
               p->n_create_buffer,
               p->n_destroy_buffer);
        printf("images, created: %zu, destroyed: %zu\n",
               p->n_image_create,
               p->n_image_destroy);
        printf("image views, created: %zu, destroyed: %zu\n",
               p->n_image_view_create,
               p->n_image_view_destroy);
    }
    free(p);
}

void create_image_views(nua_t * p)
{
    p->swap_conf.swapChainImageViews =
        calloc(p->frames_in_flight,
               sizeof(VkImageView*));

    for(int kk = 0; kk < p->frames_in_flight; kk++)
    {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = p->swap_conf.swapChainImages[kk];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = p->swap_conf.swapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        int status = vkCreateImageView(p->vkDevice,
                                       &createInfo,
                                       NULL,
                                       &p->swap_conf.swapChainImageViews[kk]);
        require_VK_SUCCESS(status);

        p->n_image_view_create++;
    }
}

void destroy_image_views(nua_t * p)
{
    for(int kk = 0 ; kk < p->frames_in_flight; kk++)
    {
        vkDestroyImageView(p->vkDevice,
                           p->swap_conf.swapChainImageViews[kk],
                           NULL);
        p->n_image_view_destroy++;
    }
    free(p->swap_conf.swapChainImageViews);
}

void get_VK_graphics_queue(nua_t * p)
{
    vkGetDeviceQueue(p->vkDevice,
                     p->queue_fami.graphics,
                     0,
                     &p->vkGraphicsQueue);
    return;
}






void create_render_pass(nua_t * p)
{
    if(p->verbose > 1)
    {
        printf("Creating a render pass. msaaSamples: ");
        print_VkSampleCountFlagBits(p->msaaSamples);
    }

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT; // TODO dynamic
    depthAttachment.samples = p->msaaSamples; //  VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.samples = p->msaaSamples;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = p->swap_conf.swapChainImageFormat;
    colorAttachment.samples = p->msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; not for MSAA
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachmentResolve = {};
    colorAttachmentResolve.format = p->swap_conf.surfaceFormat.format; // VK_FORMAT_R32G32B32_SFLOAT;  // swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT; // KEEP!
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentResolveRef = {};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

        subpass.pDepthStencilAttachment = &depthAttachmentRef; // For depth buffer
    subpass.pResolveAttachments = &colorAttachmentResolveRef; // For MSAA

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkAttachmentDescription * attachments = calloc(4,
                                                   sizeof(VkAttachmentDescription));
    attachments[0] = colorAttachment;
    attachments[1] = depthAttachment;
    attachments[2] = colorAttachmentResolve;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = (uint32_t) 3;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    int status = vkCreateRenderPass(p->vkDevice,
                                    &renderPassInfo,
                                    NULL,
                                    &p->renderPass);
    require_VK_SUCCESS(status);

    free(attachments);
    return;
}

void destroy_render_pass(nua_t * p)
{
    vkDestroyRenderPass(p->vkDevice, p->renderPass, NULL);
    return;
}

void create_command_pool(nua_t * p)
{

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = p->queue_fami.graphics;

    int status = vkCreateCommandPool(p->vkDevice,
                                     &poolInfo,
                                     NULL,
                                     &p->commandPool);
    require_VK_SUCCESS(status);
}

void destroy_command_pool(nua_t * p)
{
    vkDestroyCommandPool(p->vkDevice, p->commandPool, NULL);
}


void create_framebuffers(nua_t * p)
{
    if(p->verbose > 2)
    {
        printf("-> create_framebuffers\n");
    }
    p->nSwapChainFramebuffers = p->frames_in_flight;
    int n = p->nSwapChainFramebuffers;
    if(p->verbose > 2)
    {
        printf("Creating %d framebuffers\n", n);
    }
    p->swapChainFramebuffers = calloc(n,
                                      sizeof(VkFramebuffer));

    for(int kk = 0; kk < p->frames_in_flight; kk++)
    {
        assert(p->depthImageView != NULL);
        uint32_t nAttach = 3;
        VkImageView * attachments = calloc(nAttach,
                                           sizeof(VkImageView));
        attachments[0] = p->colorImageView;
        attachments[1] = p->depthImageView;
        attachments[2] = p->swap_conf.swapChainImageViews[kk];

        assert(attachments[0] != NULL);
        assert(attachments[1] != NULL);
        assert(attachments[2] != NULL);

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = p->renderPass;
        framebufferInfo.attachmentCount = nAttach;
        framebufferInfo.pAttachments = attachments;

        framebufferInfo.width = p->swap_conf.extent.width;
        framebufferInfo.height = p->swap_conf.extent.height;
        framebufferInfo.layers = 1;

        int status = vkCreateFramebuffer(p->vkDevice,
                                         &framebufferInfo,
                                         NULL,
                                         &p->swapChainFramebuffers[kk]);
        require_VK_SUCCESS(status);

        free(attachments);
    }
    if(p->verbose > 2)
    {
        printf("// create_framebuffers\n");
    }
    return;
}

void destroy_framebuffers(nua_t * p)
{
    for(int kk = 0; kk < p->frames_in_flight ; kk++)
    {
        vkDestroyFramebuffer(p->vkDevice,
                             p->swapChainFramebuffers[kk],
                             NULL);
    }
    free(p->swapChainFramebuffers);
}

void create_command_buffer(nua_t * p)
{
    p->commandBuffers = calloc(p->frames_in_flight, sizeof(VkCommandBuffer));
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = p->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = p->frames_in_flight;

    int status =
        vkAllocateCommandBuffers(p->vkDevice,
                                 &allocInfo,
                                 p->commandBuffers);
    require_VK_SUCCESS(status);

    if(p->verbose > 2)
    {
        printf("p->commandBuffers at %p\n", &p->commandBuffers[0]);
    }
    return;
}

void record_command_buffer(nua_t * p, uint32_t imageIndex)
{
    if(p->verbose > 1)
    {
        printf("-> record_command_buffer(%u)\n", imageIndex);
    }
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 1; // Optional
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = NULL; // Optional

    int status = vkBeginCommandBuffer(p->commandBuffers[p->current_frame], &beginInfo);
    require_VK_SUCCESS(status);

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = p->renderPass;
    renderPassInfo.framebuffer = p->swapChainFramebuffers[imageIndex];

    renderPassInfo.renderArea.offset.x = 0;
    renderPassInfo.renderArea.offset.y = 0;

    renderPassInfo.renderArea.extent = p->swap_conf.extent;


    /* One clear value for the color and one for the depth buffer
     * #background #color
     */
    VkClearValue * clear_values = calloc(2, sizeof(VkClearValue));
    clear_values[0].color.float32[0] = 0.0; // R
    clear_values[0].color.float32[1] = 0.0; // G
    clear_values[0].color.float32[2] = 0.0; // B
    clear_values[0].color.float32[3] = 0.0; // For depth
    clear_values[1].depthStencil.depth = 1.0;
    clear_values[1].depthStencil.stencil = 0;

    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clear_values;
    vkCmdBeginRenderPass(p->commandBuffers[p->current_frame],
                         &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);
    free(clear_values);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = p->swap_conf.extent.width;
    viewport.height = p->swap_conf.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(p->commandBuffers[p->current_frame],
                     0, 1,
                     &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = p->swap_conf.extent;
    vkCmdSetScissor(p->commandBuffers[p->current_frame], 0, 1, &scissor);


    if(p->show_domain)
    {
        nua_obj_record_command_buffer(p, p->domain);
    }
    if(p->show_beads)
    {
        nua_obj_record_command_buffer(p, p->balls);
    }
    if(p->show_links)
    {
        nua_obj_record_command_buffer(p, p->links);
    }

    vkCmdEndRenderPass(p->commandBuffers[p->current_frame]);

    status = vkEndCommandBuffer(p->commandBuffers[p->current_frame]);
    require_VK_SUCCESS(status);

    return;
}



void update_uniform_buffer(nua_t * p)
{

    int current_frame = p->current_frame;
    UniformBufferObject ubo = {};

    float width = (float) p->swap_conf.extent.width;
    float height = (float) p->swap_conf.extent.height;
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    UNUSED float t = timespec_diff(&now, &p->time_start);

    if(p->pause)
    {
        t = 0.0;
    }

    /* Model */
    kmMat4 id, t1, t2, model;

    kmMat4 rotX;
    kmMat4RotationX(&rotX, (float) p->roty / 120.0);
    kmMat4 rotY;
    kmMat4RotationY(&rotY, (float) p->rotx / 120.0);
    kmMat4 rotZ;
    kmMat4RotationZ(&rotZ, 0);

    kmMat4Identity(&id);
    kmMat4Multiply(&t1, &id, &rotX);
    kmMat4Multiply(&t2, &t1, &rotY);
    kmMat4Multiply(&model, &t2, &rotZ);
    memcpy(ubo.model, model.mat, 16*sizeof(float));

    /* View */
    kmVec3 eye, center, up;
    kmMat4 scale, view;
    if(p->proj_ortho)
    {
        kmVec3Fill(&eye, 0.0, 0.0, 1.0);
    } else {
        kmVec3Fill(&eye, 0.0, 0.0, 3.0/p->zoom);
    }

    kmVec3Fill(&center, 0.0, 0.0, 0.0);
    kmVec3Fill(&up, 0.0, 1.0, 0.0);
    // same as glm::lookAt
    kmMat4LookAt(&t1, &eye, &center, &up);

    kmMat4Identity(&scale);
    float s = 1;
    if(width / height < 1)
    {
        s *= width/height;
    }
    scale.mat[0] = s;
    scale.mat[5] = s;
    scale.mat[10] = s;
    scale.mat[15] = 1;
    kmMat4Multiply(&view, &t1, &scale);
    memcpy(ubo.view, view.mat, 16*sizeof(float));

    /* Projection */
    float width_over_height = width / height;
    if(p->proj_ortho)
    {
        // like glOrtho
        float k = 2.0/p->zoom;
        kmMat4OrthographicProjection((kmMat4*) &ubo.proj,
                                     -k*width_over_height, //kmScalar  	left,
                                     k*width_over_height, //kmScalar  	right,
                                     -k, //kmScalar  	bottom,
                                     k, //kmScalar  	top,
                                     -100000, //kmScalar  	nearVal,
                                     100000);//kmScalar  	farVal
    } else {
        kmMat4PerspectiveProjection((kmMat4*) &ubo.proj,
                                    //60.0/p->zoom, // fov i degrees like gluPerspective
                                    60.0,
                                    width_over_height, // aspect
                                    0.1, // zNear
                                    10.0); // zFar
    }



    //showMat4("Model", ubo.model);
    //showMat4("View", ubo.view);
    //showMat4("Projection", ubo.proj);

    void* data;
    vkMapMemory(p->vkDevice,
                p->uniformBuffersMemory[current_frame],
                0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(p->vkDevice, p->uniformBuffersMemory[current_frame]);
    return;
}

void draw_frame(nua_t * p)
{
    if( p->framebuffer_resized )
    {
        recreate_swap_chain(p);
        // We skip a frame
        return;
    }

    if(p->verbose > 1)
    {
        printf(" -> draw_frame\n");
        printf(" - WaitForFences\n");
    }

    update_uniform_buffer(p);

    vkWaitForFences(p->vkDevice,
                    1,
                    &p->inFlightFences[p->current_frame],
                    VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result =
        vkAcquireNextImageKHR(p->vkDevice,
                              p->vkSwapChain,
                              UINT64_MAX,
                              p->imageAvailableSemaphores[p->current_frame],
                              VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        if(p->verbose > 1)
        {
            // Not an actual error, is it?
            fprintf(stderr, "VK_ERROR_OUT_OF_DATE_KHR at line %d\n", __LINE__);
        }
        p->framebuffer_resized = 1;
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        fprintf(stderr,
                "failed to acquire swap chain image! at line %d\n",
                __LINE__);
        p->framebuffer_resized = 1;
    }

    vkResetFences(p->vkDevice, 1, &p->inFlightFences[p->current_frame]);

    if(p->verbose > 1)
    {
        printf("Available imageIndex: %u\n", imageIndex);
    }

    // if(p->nframes < 2) { ...
    // Possible re-use, but does not save much time

    vkResetCommandBuffer(p->commandBuffers[p->current_frame], 0);

    record_command_buffer(p, imageIndex);


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] =
        {p->imageAvailableSemaphores[p->current_frame]};
    VkPipelineStageFlags waitStages[] =
        {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &p->commandBuffers[p->current_frame];
    VkSemaphore signalSemaphores[] =
        {p->renderFinishedSemaphores[p->current_frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    if(p->verbose > 1)
    {
        printf("vkQueueSubmit\n");
    }
    int status =
        vkQueueSubmit(p->vkGraphicsQueue,
                      1,
                      &submitInfo,
                      p->inFlightFences[p->current_frame]);

    if(status != VK_SUCCESS) {
        print_VkResult(status);
        fprintf(stderr, "failed to submit draw command buffer!");
        goto fail;

        //exit(EXIT_FAILURE);
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &p->vkSwapChain;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(p->vkPresentQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR
        || result == VK_SUBOPTIMAL_KHR
        || p->framebuffer_resized)
    {
        p->framebuffer_resized = 1;
        return;
    }
    if (result != VK_SUCCESS) {
        fprintf(stderr, "failed to present swap chain image!");
    }


 fail: ;
    p->current_frame = (p->current_frame + 1) % p->frames_in_flight;
}

void handle_SDL_event(nua_t * p,  SDL_Event e)
{
    int win_height; int win_width;
    SDL_GetWindowSize(p->window, &win_height, &win_width);
    float winscale = win_height;
    win_height > win_width ? winscale = win_width : 0;
    if( e.type == SDL_QUIT )
    {
        p->quit = 1;
    }
    if(e.type == SDL_MOUSEWHEEL)
    {
        if(e.wheel.y > 0)
        {
            p->zoom *= 1.1;
        }
        else if(e.wheel.y < 0)
        {
            p->zoom /= 1.1;
        }
    }

    if(e.type == SDL_MOUSEBUTTONDOWN)
    {
        if(e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            p->mousex = e.motion.x;
            p->mousey = e.motion.y;
            p->rotx0 = p->rotx;
            p->roty0 = p->roty;
        }
    }

    if(e.type == SDL_MOUSEBUTTONUP)
    {
        if(e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            //printf("Drag ended\n");

        }
    }

    if (e.type == SDL_MOUSEMOTION)
    {
        if(e.button.button == SDL_BUTTON(SDL_BUTTON_LEFT))
        {
            //printf("e.motion.x = %d, e.motion.y = %d\n",
            //e.motion.x, e.motion.y);
            p->rotx = p->rotx0 + (float) (e.motion.x - p->mousex)/winscale*200.0;
            p->roty = p->roty0 + (float) (e.motion.y - p->mousey)/winscale*200.0;
        }
    }
    if( e.type == SDL_KEYDOWN )
    {
        // https://wiki.libsdl.org/SDL_Scancode
        char keyDown = e.key.keysym.scancode;
        if(keyDown == SDL_SCANCODE_1)
        {
            p->show_domain = (p->show_domain + 1) % 2;
        }
        if(keyDown == SDL_SCANCODE_2)
        {
            p->show_links = (p->show_links + 1) % 2;
        }
        if(keyDown == SDL_SCANCODE_3)
        {
            p->show_beads = (p->show_beads + 1) % 2;
        }
        if(keyDown == SDL_SCANCODE_0)
        {
            p->proj_ortho = (p->proj_ortho + 1) % 2;
        }
        if(keyDown == SDL_SCANCODE_SPACE)
        {
            p->pause = (p->pause + 1) % 2;
        }
        if(keyDown == SDL_SCANCODE_Q || keyDown == SDL_SCANCODE_ESCAPE)
        {
            p->quit = 1;
        }
        if(keyDown == SDL_SCANCODE_F11)
        {
            p->fullscreen = (p->fullscreen + 1) % 2;
            if(p->fullscreen == 1)
            {
                SDL_SetWindowFullscreen(p->window,
                                        SDL_WINDOW_FULLSCREEN_DESKTOP);
            } else {
                SDL_SetWindowFullscreen(p->window, 0);
            }
        }

        if(keyDown == SDL_SCANCODE_T)
        {
            p->zoom *= 1.1;
        }
        if(keyDown == SDL_SCANCODE_G)
        {

            p->zoom *= 0.9;
            if(p->proj_ortho == 0)
                /* Zooming by changing FOV, need to be restricted */
                if(60.0/p->zoom > 160.0)
                {
                    p->zoom = 60.0/160.0;
                }
        }
    }
    if(p->user_handle != NULL)
    {
        p->user_handle(e, p->user_data);
    }
    return;
}

void handle_user_interaction(nua_t * p)
{
    SDL_Event e;


    /* A computational thread should
     * make a call to SDL_PushEvent when there is something to update. */

    { /* Un-comment this for instant drawing which is a CPU/GPU waste */
        SDL_WaitEvent( &e );
        handle_SDL_event(p, e);
    }
    while( SDL_PollEvent( &e ) != 0 )
    {
        handle_SDL_event(p, e);
    }

    return;
}


void nua_main_loop(nua_t * p)
{
    struct timespec tstart, tend;
    clock_gettime(CLOCK_REALTIME, &tstart);

    if(p->verbose > 1)
    {
        printf("Starting main loop\n");
    }

    draw_frame(p);
    p->nframes = 0;
    while(!p->quit)
    {
        handle_user_interaction(p);
        if(p->data_changed)
        {
            nua_obj_update_pos(p, p->balls);
            nua_obj_update_pos(p, p->links);
            nua_redraw(p);
            p->data_changed = 0;
        }

        draw_frame(p);
        p->nframes++;
        if(p->oneframe)
        {
            p->quit = 1;
        }
    }
    clock_gettime(CLOCK_REALTIME, &tend);
    double time = timespec_diff(&tend, &tstart);

    if(p->verbose > 0)
    {
        printf("%zu frames in %.1f s. %.1f FPS\n",
               p->nframes, time, (float) p->nframes / time);
    }
    vkDeviceWaitIdle(p->vkDevice);
}


void create_sync_objects(nua_t * p)
{
    if(p->verbose > 2)
    {
        printf("-> create_sync_objects\n");
    }
    p->imageAvailableSemaphores = calloc(p->frames_in_flight,
                                         sizeof(VkSemaphore));
    p->renderFinishedSemaphores = calloc(p->frames_in_flight,
                                         sizeof(VkSemaphore));
    p->inFlightFences = calloc(p->frames_in_flight, sizeof(VkFence));

    for(int kk = 0; kk < p->frames_in_flight; kk++)
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        int status = vkCreateSemaphore(p->vkDevice,
                                       &semaphoreInfo,
                                       NULL,
                                       &p->imageAvailableSemaphores[kk]);
        require_VK_SUCCESS(status);


        status = vkCreateSemaphore(p->vkDevice,
                                   &semaphoreInfo,
                                   NULL,
                                   &p->renderFinishedSemaphores[kk]);
        require_VK_SUCCESS(status);


        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        status = vkCreateFence(p->vkDevice,
                               &fenceInfo, NULL,
                               &p->inFlightFences[kk]);
        require_VK_SUCCESS(status);

    }
    return;
}


void destroy_sync_objects(nua_t * p)
{
    for(int kk = 0; kk<p->frames_in_flight; kk++)
    {
        vkDestroySemaphore(p->vkDevice, p->imageAvailableSemaphores[kk], NULL);
        vkDestroySemaphore(p->vkDevice, p->renderFinishedSemaphores[kk], NULL);
        vkDestroyFence(p->vkDevice, p->inFlightFences[kk], NULL);
    }
    free(p->imageAvailableSemaphores);
    free(p->renderFinishedSemaphores);
    free(p->inFlightFences);
}


void recreate_swap_chain(nua_t * p)
{
    destroy_swap_chain(p);
    while(create_swap_chain(p) == EXIT_FAILURE)
    {
        printf("Can't create a swap chain, trying again\n");
    }
    p->framebuffer_resized = 0;
    return;
}



VkCommandBuffer begin_single_time_commands(nua_t * p)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = p->commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(p->vkDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void end_single_time_commands(nua_t * p, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(p->vkGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(p->vkGraphicsQueue);
    vkFreeCommandBuffers(p->vkDevice, p->commandPool, 1, &commandBuffer);

    return;
}

void copy_buffer(nua_t * p,
                VkBuffer srcBuffer,
                VkBuffer dstBuffer,
                VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(p);

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    end_single_time_commands(p, commandBuffer);
    return;
}

void transition_image_layout(nua_t * p,
                             VkImage image,
                             __attribute__((unused)) VkFormat format,
                             VkImageLayout oldLayout,
                             VkImageLayout newLayout)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(p);
    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;

    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        fprintf(stderr, "unsupported layout transition on line %d\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    vkCmdPipelineBarrier(
                         commandBuffer,
                         sourceStage, destinationStage,
                         0,
                         0, NULL,
                         0, NULL,
                         1, &barrier
                         );

    end_single_time_commands(p, commandBuffer);
}

void copy_buffer_to_image(nua_t * p,
                          VkBuffer buffer,
                          VkImage image,
                          uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = begin_single_time_commands(p);

    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset.x = 0;
    region.imageOffset.y = 0;
    region.imageOffset.z = 0;

    region.imageExtent.width = width;
    region.imageExtent.height = height;
    region.imageExtent.depth = 1;


    vkCmdCopyBufferToImage(
                           commandBuffer,
                           buffer,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region
                           );

    end_single_time_commands(p, commandBuffer);
}








void create_uniform_buffers(nua_t * p)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    p->uniformBuffers = calloc(p->frames_in_flight,
                               sizeof(VkBuffer));
    p->uniformBuffersMemory = calloc(p->frames_in_flight,
                                     sizeof(VkDeviceMemory));

    for (int kk = 0; kk < p->frames_in_flight; kk++) {
        create_buffer(p->vkPDevice, p->vkDevice,
                     bufferSize,
                     VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     &p->uniformBuffers[kk], &p->uniformBuffersMemory[kk]);
        p->n_create_buffer++;
    }
    return;
}

void destroy_uniform_buffers(nua_t * p)
{
    for(int kk = 0; kk < p->frames_in_flight; kk++)
    {
        p->n_destroy_buffer++;
        vkDestroyBuffer(p->vkDevice, p->uniformBuffers[kk], NULL);
        p->n_free_memory++;
        vkFreeMemory(p->vkDevice, p->uniformBuffersMemory[kk], NULL);
    }
    free(p->uniformBuffers);
    free(p->uniformBuffersMemory);
    return;
}


VkImageView create_image_view(nua_t * p,
                              VkImage image,
                              VkFormat format,
                              VkImageAspectFlags aspectFlags) {
    if(p->verbose > 2)
    {
        printf("-> create_image_view\n");
    }
    assert(image != NULL);
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    //viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.aspectMask = aspectFlags,
        viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(p->vkDevice,
                          &viewInfo,
                          NULL,
                          &imageView) != VK_SUCCESS) {
        fprintf(stderr, "failed to create texture image view!");
        exit(EXIT_FAILURE);
    }
    p->n_image_view_create++;
    if(p->verbose > 2)
    {
        printf("Image view at %p\n", &imageView);
    }
    return imageView;
}


void create_depth_resources(nua_t * p)
{
    if(p->verbose > 2)
    {
        printf("-> create_depth_resources\n");
    }
    /* This might not be supported, in that case we need to take the
     * route via vkGetPhysicalDeviceFormatProperties */
    VkFormat depthFormat = VK_FORMAT_D32_SFLOAT;
    create_image(p,
                 p->swap_conf.extent.width,
                 p->swap_conf.extent.height,
                 depthFormat,
                 p->msaaSamples,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &p->depthImage,
                 &p->depthImageMemory);

    assert(p->depthImage != NULL);
    p->depthImageView = create_image_view(p,
                                          p->depthImage,
                                          depthFormat,
                                          VK_IMAGE_ASPECT_DEPTH_BIT);
    assert(p->depthImageView != NULL);

    return;
}

void destroy_depth_resource(nua_t * p)
{
    /* depth buffer resources */
    vkDestroyImageView(p->vkDevice, p->depthImageView, NULL);
    p->n_image_view_destroy++;
    vkDestroyImage(p->vkDevice, p->depthImage, NULL);
    p->n_image_destroy++;
    vkFreeMemory(p->vkDevice, p->depthImageMemory, NULL);
    p->n_free_memory++;
    return;
}


void create_image(nua_t * p,
                  uint32_t width,
                  uint32_t height,
                  VkFormat format,
                  VkSampleCountFlagBits numSamples,
                  VkImageTiling tiling,
                  VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties,
                  VkImage *image,
                  VkDeviceMemory *imageMemory)
{
    if(p->verbose > 2)
    {
        printf("-> create_image\n");
    }
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(p->vkDevice, &imageInfo, NULL, image) != VK_SUCCESS) {
        fprintf(stderr, "failed to create image!");
        exit(EXIT_FAILURE);
    }
    p->n_image_create++;
    assert(*image != NULL);
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(p->vkDevice, *image, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(p->vkPDevice,
                                               memRequirements.memoryTypeBits,
                                               properties);

    p->n_allocate_memory++;
    if (vkAllocateMemory(p->vkDevice, &allocInfo, NULL, imageMemory) != VK_SUCCESS) {
        fprintf(stderr, "failed to allocate image memory!");
        exit(EXIT_FAILURE);
    }

    vkBindImageMemory(p->vkDevice, *image, *imageMemory, 0);
    if(p->verbose > 2)
    {
        printf("imageMemory: %p\n", (void*) *imageMemory);
        assert(*image != NULL);
        printf("<- create_image\n");
    }
    return;
}

void create_color_resource(nua_t * p)
{

    create_image(p,
                 p->swap_conf.extent.width,
                 p->swap_conf.extent.height,
                 p->swap_conf.swapChainImageFormat,
                 p->msaaSamples,
                 VK_IMAGE_TILING_OPTIMAL,
                 VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                 &p->colorImage,
                 &p->colorImageMemory);
    assert(p->colorImage != NULL);

    p->colorImageView = create_image_view(p,
                                          p->colorImage,
                                          p->swap_conf.swapChainImageFormat,
                                          VK_IMAGE_ASPECT_COLOR_BIT);
    if(p->verbose > 2)
    {
        printf("p->colorImage: %p\n", p->colorImage);
    }
    return;
}


void destroy_color_resource(nua_t * p)
{
    /* msaa resources */
    vkDestroyImageView(p->vkDevice, p->colorImageView, NULL);
    p->n_image_view_destroy++;
    vkDestroyImage(p->vkDevice, p->colorImage, NULL);
    p->n_image_destroy++;
    vkFreeMemory(p->vkDevice, p->colorImageMemory, NULL);
    p->n_free_memory++;
    return;
}


void init_vulkan(nua_t * p)
{
    // p->vkInstance
    setup_VK_instance(p);

    // p->vkPDevice
    setup_VK_physical_device(p);

    // p->queue_fami.graphics
    setup_VK_queue(p);

    // p->vkDevice
    setup_VK_logical_device(p);

    // p->vkGraphicsQueue
    get_VK_graphics_queue(p);

    // p->vkSurface via SDL_Vulkan_CreateSurface
    setup_VK_surface(p);

    // p->queue_fami.present
    // &p->vkPresentQueue
    setup_VK_present_queue(p);

    /* p->swap_conf.surfaceFormat = formats[0];
       p->swap_conf.swapChainImageFormat = formats[0].format;
       p->swap_conf.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    */
    query_VK_SwapChainSupport(p);

    /*
     * Creates:
     * - p->renderPass
     * Requires:
     * - p->msaaSamples (from prog_new)
     * - p->swap_conf.swapChainImageFormat (query_VK_SwapChainSupport)
     */
    create_render_pass(p);

    /*
      p->vkSwapChain
      p->swap_conf.swapChainImages
    */
    create_swap_chain(p);
    create_command_pool(p);
    create_uniform_buffers(p);
    create_command_buffer(p);
    create_sync_objects(p);
    return;
}


void unit_tests(void)
{
    assert(sizeof(kmScalar) == sizeof(float));
}

void nua_data_changed(nua_t * nua)
{
    nua->data_changed = 1;
    nua_redraw(nua);
}

/* Call to draw a new frame */
void nua_redraw(nua_t * p)
{
    /* Not to confuse SDL it would be better to use SDL_RegisterEvents
     * earlier on */
    SDL_Event sdlevent;
    //sdlevent.type = SDL_WINDOWEVENT;
    ///sdlevent.window.event = SDL_WINDOWEVENT_EXPOSED;
    sdlevent.type = p->data_changed_event;
    SDL_PushEvent(&sdlevent);
}
