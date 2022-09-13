#ifndef __nua_object_h__
#define __nua_object_h__

/* An object consists of vertices, triangles, shaders, instancing data
 * and possibly a texture map.
 *
 * Currently only pre-defined meshes can be used.
 *
 */

#include "nua.h"

#include <assert.h>
#include <getopt.h>
#include <signal.h> // for gdb/raise(SIGINT)
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vulkan/vulkan.h>
#include <SDL.h>
#include <SDL_vulkan.h>

/* The types of object that are understood */
enum nua_obj_type_{
    NUA_OBJECT_BALL,
    NUA_OBJECT_CONNECT,
    NUA_OBJECT_DOMAIN
};

typedef enum nua_obj_type_ nua_obj_type;

struct nua;
typedef struct nua nua_t;

typedef struct nua_obj {
    nua_obj_type otype;

    /* Vertices */
    size_t npoints; /* Number of vertices */
    float * data; /* vertices */

    /* Triangles */
    size_t nindices; /* 3x number of triangles */
    uint32_t * indices; /* triangles */

    /* Instancing data: Color, Positions, etc.
     * Interpretation depends on otype */
    size_t nbeads; /* TODO nbeads and nconnect is really the same as ninstances */
    size_t nconnect;
    size_t ninstances;
    float * instance_data;

    /* Affecting quality of objects */
    int spherediv; /* How many subdivisions to use for the balls */
    int linkdiv; /* How many sides should the links have */
    VkPolygonMode polygon_mode;

    /* shader files in SPV format */
    char * frag_shader_file;
    char * vert_shader_file;
    /* loaded shaders */
    VkShaderModule vertShader;
    VkShaderModule fragShader;

    /* GPU memory ... */
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer instanceBuffer;
    VkDeviceMemory instanceBufferMemory;

    /* To describe a descriptor set for a VkBuffer */
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet * descriptorSets;

    /* For one texture */
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;

    int nBindingDescriptions;
    int  nAttribDescriptions;
    VkVertexInputBindingDescription * bindingDescriptions;
    VkVertexInputAttributeDescription * attribDescriptions;

    /* Add vkDevice? -- would solve most references to p*/
    /* verbose  */

} nua_obj_t;

/* Create a new object */
nua_obj_t * nua_obj_new(nua_t * p, /* They nua environment */
                        nua_obj_type otype, /* What type */
                        int ninstances, /* number of instances */
                        float * instancing_data);

void nua_obj_free(nua_obj_t * v, nua_t * p);

/*
 * internal stuff ...
 */

/* Called by nua to update the instancing data @GPU  */
void nua_obj_update_pos(nua_t * p, nua_obj_t * v);

void nua_obj_record_command_buffer(nua_t * p, nua_obj_t * v);

void nua_obj_create_bindings(nua_obj_t * v);
/* Freed in nua_obj_free */

void nua_obj_create_graphics_pipeline(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_graphics_pipeline(nua_t * p, nua_obj_t * v);

void nua_obj_create_vertex_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_vertex_buffer(nua_t * p, nua_obj_t * v);

void nua_obj_create_index_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_index_buffer(nua_t * p, nua_obj_t * v);

void nua_obj_create_vertex_instance_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_vertex_instance_buffer(nua_t * p, nua_obj_t * v);

void nua_obj_create_index_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_create_descriptor_set_layout(nua_t * p, nua_obj_t * v);

void nua_obj_create_descriptor_sets(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_descriptor_sets(nua_t * p, nua_obj_t * v);

void nua_obj_create_texture_image_view(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_texture_image_view(nua_t * p, nua_obj_t * v);

void nua_obj_create_texture_image(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_texture_image(nua_t * p, nua_obj_t * v);

void nua_obj_create_texture_sampler(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_texture_sampler(nua_t * p, nua_obj_t * v);

void nua_obj_create_descriptor_pool(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_descriptor_pool(nua_t * p, nua_obj_t * v);

#endif
