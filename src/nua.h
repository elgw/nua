#ifndef __nua_h__
#define __nua_h__

#define sampler_test 1

#include <assert.h>
#include <getopt.h>
#include <signal.h> // for gdb/raise(SIGINT)
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL.h>
#include <SDL_vulkan.h>
#include <vulkan/vulkan.h>
#include <kazmath/kazmath.h>

#include "vshape.h"
#include "cmm_io.h"
#include "nua_object.h"
#include "nua_util.h"
#include "nua_vkutil.h"
#include "nua_version.h"

struct nua_obj;
typedef struct nua_obj nua_obj_t;

typedef struct {
    uint32_t graphics; // vkQueueFamilyIndex;
    uint32_t present;
} queue_family_indexes_t;

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR surfaceFormat;
    VkPresentModeKHR presentMode;
    VkExtent2D extent; // width x height in pixels

    VkImage * swapChainImages;
    VkImageView * swapChainImageViews;

    VkFormat swapChainImageFormat;

} swap_chain_conf_t;

typedef struct  {
    float model[16];
    float view[16];
    float proj[16];
} UniformBufferObject;

/* Todo: factor into vulkan, objects and general program settings */
typedef struct nua {
    /*
     * General settings typically parsed from command line
     * or changed by user interactions
     */

    // default 1. 0 should be quiet. 2 and above for debugging
    int verbose;
    // set to 1 to quit after first frame, for debugging
    int oneframe;
    // set to 0 to disable any validation layers
    int validation_layers;
    float zoom;
    int fullscreen;
    int quit; /* set to 1 to indicate that it is time to quit */
    int proj_ortho;    /* Projection type: TODO: enum */
    int show_domain;
    int show_links;
    int show_beads;
    /* Visual quality */
    int msaa_want;
    int spherediv;
    int linkdiv;
    int pause; // not used

    /* Book keeping */
    struct timespec time_start;
    size_t nframes; /* Number of frames that has been drawn */

    size_t n_allocate_memory;
    size_t n_free_memory;
    size_t n_create_buffer;
    size_t n_destroy_buffer;
    size_t n_image_create;
    size_t n_image_destroy;
    size_t n_image_view_create;
    size_t n_image_view_destroy;

    /*
     * User interactions and events
     */
    /* Handler to custom event callback function */
    void (*user_handle) (SDL_Event , void *);
    void * user_data;
    /* Mouse position at beginning of mouse press and drag */
    int mousex;
    int mousey;
    float rotx0;
    float roty0;
    float rotx;
    float roty;

    volatile int data_changed; // set to 1 to let nua update the data
    /* A SDL event number for changes in data that should trigger a
     * coordinate transfer to the GPU */
    uint32_t data_changed_event;


    /*
     * Graphical objects
     */
    int nbeads; /* Number of instances */
    float * bead_data; /* Instancing data */

    int nlinks; /* Number of instances */
    float * link_data; /* Instancing data */

    nua_obj_t * balls;
    nua_obj_t * links;
    nua_obj_t * domain;

    /*
     * Vulkan context, Shared between all graphical objects
     */
    SDL_Window * window;
    VkPhysicalDevice vkPDevice; // Destroyed with the vkInstance
    VkDevice vkDevice; // logical device
    VkBuffer * uniformBuffers;
    VkDeviceMemory * uniformBuffersMemory;
    uint32_t current_frame;
    int frames_in_flight;
    int framebuffer_resized;
    int vkEnableValidationLayers;
    VkInstance vkInstance;
    queue_family_indexes_t queue_fami;
    VkSurfaceKHR vkSurface;
    VkQueue vkGraphicsQueue;
    VkQueue vkPresentQueue;
    swap_chain_conf_t swap_conf;
    VkSwapchainKHR vkSwapChain;
    VkImageView * vkSwapChainImages;
    int nSwapChainFramebuffers;
    VkFramebuffer * swapChainFramebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer * commandBuffers; // One per frame in flight
    /* Synchronization */
    VkSemaphore * imageAvailableSemaphores; // One per frame in flight
    VkSemaphore * renderFinishedSemaphores; // One per frame in flight
    VkFence * inFlightFences; // One per frame in flight
    /* For Depth buffer */
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
    /* For MSAA  */
    VkSampleCountFlagBits msaaSamples;
    VkImage colorImage;
    VkDeviceMemory colorImageMemory;
    VkImageView colorImageView;
    VkRenderPass renderPass;
} nua_t;


/*
 * High level interface
 */

/* Create a new nua object with default settings and
 * some fields to fill out  */
nua_t * nua_new();

/* Do some validation on the object and then start the thing */
void nua_run(nua_t * p);

/* Free up. Todo: close window if called from thread. */
void nua_destroy(nua_t * p);

/* Print brief ui-usage to stdout */
void nua_show_usage(__attribute__((unused)) nua_t * p);

/* Tell nua to draw a new frame. Usually not needed. */
void nua_redraw(nua_t * p);

/* Tell nua that the data has changed, will trigger a redraw  */
void nua_data_changed(nua_t * p);


/*
 * Internal TODO don't expose these
 */
void nua_setup_graphics(nua_t * p);

void create_command_pool(nua_t * p);
void destroy_command_pool(nua_t * p);

int create_swap_chain(nua_t * p);
void destroy_swap_chain(nua_t * p);
void recreate_swap_chain(nua_t * p);

void create_image_views(nua_t * p);
void destroy_image_views(nua_t * p);

void vertex_create_graphics_pipeline(nua_t * p, nua_obj_t * v);
void vertex_destroy_graphics_pipeline(nua_t * p, nua_obj_t * v);

void create_sync_objects(nua_t * p);
void destroy_sync_objects(nua_t * p);

void create_framebuffers(nua_t * p);
void destroy_framebuffers(nua_t * p);

void create_vertex_buffer(nua_t * p);
void destroy_vertex_buffer(nua_t * p);
void copy_buffer(nua_t * p, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
void record_command_buffer(nua_t * p, uint32_t imageIndex);
void create_descriptor_set_layout(nua_t * p);

void destroy_index_buffer(nua_t * p);

void create_uniform_buffers(nua_t * p);
void destroy_uniform_buffers(nua_t * p);

void create_descriptor_pool(nua_t * p);
void destroy_descriptor_pool(nua_t * p);

void create_descriptor_sets(nua_t * p);
void destroy_descriptor_sets(nua_t * p);

void update_uniform_buffer(nua_t * p);

void create_color_resource(nua_t * p);
void destroy_color_resource(nua_t * p);

void create_depth_resources(nua_t * p);
void destroy_depth_resource(nua_t * p);

void create_render_pass(nua_t * p);
void destroy_render_pass(nua_t * p);

void create_image(nua_t * p,
                  uint32_t width,
                  uint32_t height,
                  VkFormat format,
                  VkSampleCountFlagBits numSamples,
                  VkImageTiling tiling,
                  VkImageUsageFlags usage,
                  VkMemoryPropertyFlags properties,
                  VkImage* image,
                  VkDeviceMemory* imageMemory);

VkImageView create_image_view(nua_t*,
                              VkImage,
                              VkFormat,
                              VkImageAspectFlags);

VkCommandBuffer begin_single_time_commands(nua_t * p);

void end_single_time_commands(nua_t * p,
                         VkCommandBuffer commandBuffer);

void transition_image_layout(nua_t * p,
                             VkImage image,
                           VkFormat format,
                           VkImageLayout oldLayout,
                           VkImageLayout newLayout);

void copy_buffer_to_image(nua_t * p, VkBuffer buffer,
                          VkImage image,
                          uint32_t width, uint32_t height);

void unit_tests(void);
void setup_SDL(nua_t * p);
void init_vulkan(nua_t * p);

void nua_main_loop(nua_t * p);
void nua_destroy_graphics(nua_t * p);

#endif
