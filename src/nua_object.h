#ifndef __nua_object_h__
#define __nua_object_h__

#include "nua.h"

/* For storing all vertices
 * Vertices: (3+3) x n  x,y,z,R,G,B
 * Indices:  3 x m (three per triangle)
 */

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



/* The three types of object that can be created */
enum nua_obj_type_{
    NUA_OBJECT_BALL,
    NUA_OBJECT_CONNECT,
    NUA_OBJECT_DOMAIN
};

typedef enum nua_obj_type_ nua_obj_type;

struct prog;
typedef struct prog nua_t;

typedef struct nua_obj {
    nua_obj_type otype;
    float * data;
    uint32_t * indices;
    size_t nindices;
    size_t npoints;
    size_t nbeads;
    size_t nconnect;
    /* Affecting quality of objects */
    int spherediv;
    int linkdiv;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    VkBuffer instanceBuffer;
    VkDeviceMemory instanceBufferMemory;
    char * frag_shader_file;
    char * vert_shader_file;
    VkShaderModule vertShader;
    VkShaderModule fragShader;
    VkPolygonMode polygon_mode;

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

    size_t ninstances;
    float * instance_data;

    /* Add vkDevice? -- would solve most references to p*/
    /* verbose  */

} nua_obj_t;

nua_obj_t * nua_obj_new(nua_t * p,
                      nua_obj_type otype,
                      int nobjects,
                      float * object_data);

void nua_obj_free(nua_obj_t * v, nua_t * p);

void nua_obj_update_pos(nua_t * p, nua_obj_t * v);
void nua_obj_create_bindings(nua_obj_t * v);

void nua_obj_create_graphics_pipeline(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_graphics_pipeline(nua_t * p, nua_obj_t * v);

void nua_obj_destroy_vertex_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_index_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_vertex_instance_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_record_command_buffer(nua_t * p, nua_obj_t * v);

void nua_obj_create_descriptor_sets(nua_t * p, nua_obj_t * v);
void nua_obj_create_vertex_instance_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_create_vertex_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_create_index_buffer(nua_t * p, nua_obj_t * v);
void nua_obj_create_descriptor_set_layout(nua_t * p, nua_obj_t * v);
void nua_obj_create_descriptor_set_layout(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_descriptor_sets(nua_t * p, nua_obj_t * v);


void nua_obj_create_texture_image(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_texture_image_view(nua_t * p, nua_obj_t * v);
void nua_obj_create_texture_image_view(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_texture_image(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_texture_sampler(nua_t * p, nua_obj_t * v);
void nua_obj_create_texture_sampler(nua_t * p, nua_obj_t * v);

void nua_obj_create_descriptor_pool(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_descriptor_pool(nua_t * p, nua_obj_t * v);
void nua_obj_destroy_command_pool(nua_t * p, nua_obj_t * v);

#endif
