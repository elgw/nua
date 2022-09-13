#include "nua_object.h"

/* Note: should be included from the source, not the header file. */
// #define STB_IMAGE_IMPLEMENTATION
// #include <stb_image.h>

#ifndef __shaders_h__
#define __shaders_h__
#include "../shaders/shader_ball_frag.spv.h"
#include "../shaders/shader_ball_vert.spv.h"
#include "../shaders/shader_connect_frag.spv.h"
#include "../shaders/shader_connect_vert.spv.h"
#include "../shaders/shader_domain_frag.spv.h"
#include "../shaders/shader_domain_vert.spv.h"
#endif



vertex_t * vertex_new(nua_t * p, vertex_type vtype,
                      int nobjects, float * object_data)
{
    if(p->verbose > 2)
    {
        printf(" -> vertex_new, %d objects\n", nobjects);
    }

    vertex_t * v = calloc(1, sizeof(vertex_t));
    v->vtype = vtype;
    v->spherediv = p->spherediv;
    v->linkdiv = p->linkdiv;
    v->ninstances = nobjects;
    v->frag_shader_file = calloc(128,1);
    v->vert_shader_file = calloc(128,1);


/* 1. Create primitive object */
    vshape_t * shape = NULL;
    if(vtype == VERTEX_BALL)
    {
        shape = vshape_sphere(v->spherediv);
        if(p->verbose > 1)
        {
            printf("Created a sphere with %u triangles and %u vertices\n",
                   shape->ntri,
                   shape->nvert);
        }
        sprintf(v->frag_shader_file, "shaders/shader_ball_frag.spv");
        sprintf(v->vert_shader_file, "shaders/shader_ball_vert.spv");
    }

    if(vtype == VERTEX_DOMAIN)
    {
        v->ninstances = 1;
        shape = vshape_sphere(4);
        if(p->verbose > 1)
        {
            printf("Created a sphere with %u triangles and %u vertices\n",
                   shape->ntri,
                   shape->nvert);
        }
        sprintf(v->frag_shader_file, "shaders/shader_domain_frag.spv");
        sprintf(v->vert_shader_file, "shaders/shader_domain_vert.spv");
    }

    if(vtype == VERTEX_CONNECT)
    {
        shape = vshape_tube(1, 1, v->linkdiv);

        if(p->verbose > 1)
        {
            printf("Created a tube with %u triangles and %u vertices\n",
                   shape->ntri,
                   shape->nvert);
        }
        sprintf(v->frag_shader_file, "shaders/shader_connect_frag.spv");
        sprintf(v->vert_shader_file, "shaders/shader_connect_vert.spv");
    }

    if(shape == NULL)
    {
        fprintf(stderr, "Unsupported vertex type supplied\n");
        exit(EXIT_FAILURE);
    }

    v->npoints = shape->nvert;
    v->data = shape->vert;
    v->nindices = shape->ntri*3;
    v->indices = shape->tri;
    free(shape);

    /* 2. Create instance data */
    if(vtype == VERTEX_CONNECT)
    {
        v->instance_data = object_data;
#if 0
        v->instance_data = calloc(v->ninstances*7, sizeof(float));
        for(uint32_t kk = 0; kk<v->ninstances; kk++)
        {
            float * p = v->instance_data + kk*7;

            sphererand(p);
            sphererand(p+3);

            p[6] = 0.0002; // r


            if(kk == 0)
            {
                p[0] = 0; p[1] = 0; p[2] = 0; // from
                p[3] = 0.5; p[4] = 0; p[5] = 0; // to
                p[6] = 0.01; // radius?
            }
            if(kk == 1)
            {
                p[0] = 0; p[1] = 0; p[2] = 0; // from
                p[3] = 0; p[4] = 1; p[5] = 0; // to
                p[6] = 0.02; // radius?
            }
            if(kk == 2)
            {
                p[0] = 0; p[1] = 0; p[2] = 0; // from
                p[3] = 0; p[4] = 0; p[5] = 1.0; // to
                p[6] = 0.04; // radius?
            }
            if(kk == 3)
            {
                p[0] = 0; p[1] = 0; p[2] = 0; // centre to
                p[3] = 1.0/sqrt(3); p[4] = 1.0/sqrt(3); p[5] = 1.0/sqrt(3); // pos x
                p[6] = 0.005; // radius?
            }
            if(kk == 4)
            {
                p[0] = 1; p[1] = 0; p[2] = 0; // centre to
                p[3] = 0; p[4] = 1.0; p[5] = 0; // pos x
                p[6] = 0.005; // radius?
            }
        }
#endif
    }

    if(vtype == VERTEX_BALL)
    {
        v->instance_data = object_data;
#if 0
        for(uint32_t kk = 0; kk<v->ninstances; kk++)
        {
            float * p = v->instance_data + kk*7;
            sphererand(p);
            p[3] = 0.1 + 0.9*urand(); // r
            p[4] = 0.1 + 0.9*urand(); // g
            p[5] = 0.1 + 0.9*urand(); // b
            p[6] = 0.01*(0.5 + 0.5*urand()); // r

            if(kk == 0)
            {
                p[0] = 0; p[1] = 0; p[2] = 0; // centered
                p[3] = 0; p[4] = 1.0; p[5] = 0; // green
                p[6] = 0.03;
            }
            if(kk == 1)
            {
                p[0] = 1.0/sqrt(3); p[1] = 1.0/sqrt(3); p[2] = 1.0/sqrt(3); // centered
                p[3] = 0; p[4] = 1.0; p[5] = 0; // green
                p[6] = 0.03;
            }

        }
#endif
    }

    if(vtype == VERTEX_DOMAIN)
    {
        v->instance_data = calloc(7, sizeof(float));
        float * p = v->instance_data;
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p[3] = .45;
        p[4] = .3;
        p[5] = .3;
        p[6] = 1;
    }

#ifndef NDEBUG
    float sum = 0;
    for(size_t kk = 0; kk< v->ninstances*7; kk++)
    {
        sum+=v->instance_data[kk];
    }
    printf("Sum of instance data: %f\n", sum);
#endif

    vertex_create_vertex_buffer(p, v);
    vertex_create_index_buffer(p, v);
    vertex_create_vertex_instance_buffer(p, v);
    vertex_create_texture_image(p, v); // after command pool
    vertex_create_texture_image_view(p, v);
    vertex_create_texture_sampler(p, v);
    vertex_create_bindings(v);
    vertex_create_descriptor_set_layout(p, v);
    vertex_create_descriptor_pool(p, v);
    vertex_create_descriptor_sets(p, v);
    vertex_create_graphics_pipeline(p, v);
    return v;
}


void vertex_update_pos(nua_t * p, vertex_t * v)
{
    if(v == NULL)
    {
        return;
    }

    size_t data_size = v->ninstances*7*sizeof(float);
    size_t bufferSize = data_size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(p->vkPDevice, p->vkDevice, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  //VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  &stagingBuffer, &stagingBufferMemory);
    p->n_create_buffer++;
    void* data;
    vkMapMemory(p->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data,
           v->instance_data,
           data_size);
    vkUnmapMemory(p->vkDevice, stagingBufferMemory);

    copy_buffer(p, stagingBuffer, v->instanceBuffer, bufferSize);

    p->n_free_memory++;
    vkFreeMemory(p->vkDevice, stagingBufferMemory, NULL);
    p->n_destroy_buffer++;
    vkDestroyBuffer(p->vkDevice, stagingBuffer, NULL);



    return;
}

void vertex_create_bindings(vertex_t * v)
{

    v->nBindingDescriptions = 2;
    v->nAttribDescriptions = 4;

    v->bindingDescriptions = calloc(v->nBindingDescriptions,
                                    sizeof(VkVertexInputBindingDescription));

    v->bindingDescriptions[0].binding = 0;
    v->bindingDescriptions[0].stride = 3*sizeof(float);
    v->bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;


    v->bindingDescriptions[1].binding = 1;
    v->bindingDescriptions[1].stride = 7*sizeof(float);
    v->bindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;


    v->attribDescriptions = calloc(v->nAttribDescriptions,
                                   sizeof(VkVertexInputAttributeDescription));

    VkVertexInputAttributeDescription * posAttrib = &v->attribDescriptions[0];
    posAttrib->binding = 0;
    posAttrib->location = 0;
    posAttrib->format = VK_FORMAT_R32G32B32_SFLOAT;
    posAttrib->offset = 0*sizeof(float);

    /* Previously used for per vertex color
       VkVertexInputAttributeDescription * colAttrib = &v->attribDescriptions[1];
       colAttrib->binding = 0;
       colAttrib->location = 1;
       colAttrib->format = VK_FORMAT_R32G32B32_SFLOAT;
       colAttrib->offset = 3*sizeof(float);
    */

    VkVertexInputAttributeDescription * insPosAttrib = &v->attribDescriptions[1];
    insPosAttrib->binding = 1;
    insPosAttrib->location = 1;
    insPosAttrib->format = VK_FORMAT_R32G32B32_SFLOAT;
    insPosAttrib->offset = 0*sizeof(float);
    VkVertexInputAttributeDescription * insColorAttrib = &v->attribDescriptions[2];
    insColorAttrib->binding = 1;
    insColorAttrib->location = 2;
    insColorAttrib->format = VK_FORMAT_R32G32B32_SFLOAT;
    insColorAttrib->offset = 3*sizeof(float);
    VkVertexInputAttributeDescription * insRadiusAttrib = &v->attribDescriptions[3];
    insRadiusAttrib->binding = 1;
    insRadiusAttrib->location = 3;
    insRadiusAttrib->format = VK_FORMAT_R32_SFLOAT;
    insRadiusAttrib->offset = 6*sizeof(float);
    return;
}

void vertex_destroy_vertex_buffer(nua_t * p, vertex_t * v)
{
    p->n_free_memory++;
    if(p->verbose > 2)
    {
        printf("vertexBufferMemory: %p\n", (void *) v->vertexBufferMemory);
    }
    vkFreeMemory(p->vkDevice, v->vertexBufferMemory, NULL);
    p->n_destroy_buffer++;
    vkDestroyBuffer(p->vkDevice, v->vertexBuffer, NULL);

    return;
}

void vertex_free(vertex_t * v, nua_t * p)
{
    free(v->frag_shader_file);
    free(v->vert_shader_file);
    free(v->data);
    free(v->indices);
    free(v->attribDescriptions);
    vertex_destroy_graphics_pipeline(p, v);
    vertex_destroy_vertex_buffer(p, v);
    vertex_destroy_index_buffer(p, v);
    vertex_destroy_vertex_instance_buffer(p, v);
    vertex_destroy_descriptor_sets(p, v);
    vertex_destroy_texture_image(p, v);
    vertex_destroy_texture_image_view(p, v);
    vertex_destroy_texture_sampler(p, v);
    vertex_destroy_descriptor_pool(p, v);
    free(v->bindingDescriptions);
    free(v);
    return;
}

/* Per object variables:
 * - shaders
 * - binding and attribute descriptors
 */

void vertex_create_graphics_pipeline(nua_t * p, vertex_t * v)
{
    if(p->verbose > 1)
    {
        printf("-> vertex_create_graphics_pipeline(nua_t * p, vertex_t * v)\n");
    }
    if(p->verbose > 2)
    {
        printf("-> Read shaders and create modules\n");
    }

    if(nua_isfile(v->vert_shader_file))
    {
        v->vertShader = load_shader(p->vkDevice,
                                    v->vert_shader_file,
                                    p->verbose);
    } else {
        if(v->vtype == VERTEX_BALL)
        {
            v->vertShader = shader_from_buffer(p->vkDevice,
                                               (uint32_t*) __shaders_shader_ball_vert_spv,
                                               __shaders_shader_ball_vert_spv_len/4,
                                               p->verbose);
        }
        if(v->vtype == VERTEX_CONNECT)
        {
            v->vertShader = shader_from_buffer(p->vkDevice,
                                               (uint32_t*) __shaders_shader_connect_vert_spv,
                                               __shaders_shader_connect_vert_spv_len/4,
                                               p->verbose);
        }
        if(v->vtype == VERTEX_DOMAIN)
        {
            v->vertShader = shader_from_buffer(p->vkDevice,
                                               (uint32_t*) __shaders_shader_domain_vert_spv,
                                               __shaders_shader_domain_vert_spv_len/4,
                                               p->verbose);
        }
    }

    if(nua_isfile(v->frag_shader_file))
    {
        v->fragShader = load_shader(p->vkDevice,
                                    v->frag_shader_file,
                                    p->verbose);
    } else {
        if(v->vtype == VERTEX_BALL)
        {
            v->fragShader = shader_from_buffer(p->vkDevice,
                                               (uint32_t*) __shaders_shader_ball_frag_spv,
                                               __shaders_shader_ball_frag_spv_len/4,
                                               p->verbose);
        }
        if(v->vtype == VERTEX_CONNECT)
        {
            v->fragShader = shader_from_buffer(p->vkDevice,
                                               (uint32_t*) __shaders_shader_connect_frag_spv,
                                               __shaders_shader_connect_frag_spv_len/4,
                                               p->verbose);
        }
        if(v->vtype == VERTEX_DOMAIN)
        {
            v->fragShader = shader_from_buffer(p->vkDevice,
                                               (uint32_t*) __shaders_shader_domain_frag_spv,
                                               __shaders_shader_domain_frag_spv_len/4,
                                               p->verbose);
        }
    }


    // Create shader stage
    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = v->vertShader;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = v->fragShader;
    fragShaderStageInfo.pName = "main";


    uint32_t nShaderStages = 2;
    VkPipelineShaderStageCreateInfo * shaderStages = calloc(nShaderStages,
                                                            sizeof(VkPipelineShaderStageCreateInfo));
    shaderStages[0] = vertShaderStageInfo;
    shaderStages[1] = fragShaderStageInfo;

    VkDynamicState * dynamicStates = calloc(2, sizeof(VkDynamicState));
    dynamicStates[0] =  VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates[1] =  VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    /* When we want to use a vertex buffer */
    if(p->verbose > 2)
    {
        printf("%u binding descriptions, %u attribute descriptions \n",
               v->nBindingDescriptions, v->nAttribDescriptions);
    }
    vertexInputInfo.vertexBindingDescriptionCount = v->nBindingDescriptions;
    vertexInputInfo.pVertexBindingDescriptions = v->bindingDescriptions;
    vertexInputInfo.vertexAttributeDescriptionCount = v->nAttribDescriptions;
    vertexInputInfo.pVertexAttributeDescriptions = v->attribDescriptions;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    float w = (float) p->swap_conf.extent.width;
    float h = (float) p->swap_conf.extent.width;
    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = w;
    viewport.height = h;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = p->swap_conf.extent;

    /* Static, could be made dynamic */
    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;

    rasterizer.polygonMode = v->polygon_mode;
    //rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    //rasterizer.polygonMode = VK_POLYGON_MODE_LINE;

    rasterizer.lineWidth = 1.0f;


    if(v->vtype == VERTEX_DOMAIN)
    {
        rasterizer.cullMode = VK_CULL_MODE_NONE;;
    }

    if(v->vtype == VERTEX_BALL)
    {
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    }

    if(v->vtype == VERTEX_CONNECT)
    {
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    }

    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = p->msaaSamples; //  VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = NULL; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT
        | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Needed for descriptor sets
    pipelineLayoutInfo.pSetLayouts = &v->descriptorSetLayout; // Needed for descriptor sets
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

    if(p->verbose > 2)
    {
        printf("-> vkCreatePipelineLayout\n");
    }
    int status =
        vkCreatePipelineLayout(p->vkDevice,
                               &pipelineLayoutInfo,
                               NULL, &v->pipelineLayout);
    require_VK_SUCCESS(status);

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.minDepthBounds = 0.0f; // Optional
    depthStencil.maxDepthBounds = 1.0f; // Optional
    depthStencil.stencilTestEnable = VK_FALSE;
    //depthStencil.front = {}; // Optional
    //depthStencil.back = {}; // Optional

    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = nShaderStages;
    pipelineInfo.pStages = shaderStages;

    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = NULL; // Optional
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.layout = v->pipelineLayout;

    pipelineInfo.renderPass = p->renderPass;
    pipelineInfo.subpass = 0;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipelineInfo.basePipelineIndex = -1; // Optional
    pipelineInfo.pDepthStencilState = &depthStencil;

    if(p->verbose > 2)
    {
        printf("-> vkCreateGraphicsPipeline\n");
    }
    status = vkCreateGraphicsPipelines(p->vkDevice,
                                       VK_NULL_HANDLE,
                                       1,
                                       &pipelineInfo,
                                       NULL,
                                       &v->graphicsPipeline);
    require_VK_SUCCESS(status);

    if(p->verbose > 1)
    {
        printf("Graphics pipeline created\n");
    }
    assert(v->graphicsPipeline != NULL);


    free(shaderStages);
    free(dynamicStates);
    return;
}

void vertex_destroy_index_buffer(nua_t * p, vertex_t * v)
{
    p->n_destroy_buffer++;
    vkDestroyBuffer(p->vkDevice, v->indexBuffer, NULL);
    p->n_free_memory++;
    vkFreeMemory(p->vkDevice, v->indexBufferMemory, NULL);
    return;
}

void vertex_destroy_vertex_instance_buffer(nua_t * p, vertex_t * v)
{
    p->n_free_memory++;
    vkFreeMemory(p->vkDevice, v->instanceBufferMemory, NULL);
    p->n_destroy_buffer++;
    vkDestroyBuffer(p->vkDevice, v->instanceBuffer, NULL);

    return;
}

void vertex_record_command_buffer(nua_t * p, vertex_t * v)
{
    if(v == NULL)
    {
        if(p->verbose > 2)
        {
            printf("vertex_record_command_buffer: empty vertex\n");
        }
        return;
    }
    VkBuffer vertexBuffers[] = {v->vertexBuffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(p->commandBuffers[p->current_frame],
                           0, 1, vertexBuffers,
                           offsets);

    vkCmdBindVertexBuffers(p->commandBuffers[p->current_frame],
                           1, 1, &v->instanceBuffer,
                           offsets);


    vkCmdBindIndexBuffer(p->commandBuffers[p->current_frame],
                         v->indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    assert(v->graphicsPipeline != NULL);
    vkCmdBindPipeline(p->commandBuffers[p->current_frame],
                      VK_PIPELINE_BIND_POINT_GRAPHICS,
                      v->graphicsPipeline);

    vkCmdBindDescriptorSets(p->commandBuffers[p->current_frame], // command buffer
                            VK_PIPELINE_BIND_POINT_GRAPHICS, // pipelineBindPoint
                            v->pipelineLayout, // VkPipelineLayout
                            0, // firstSet
                            1, // descriptorSetCount
                            &v->descriptorSets[p->current_frame], // const VkDescriptorSet*
                            0, // dynamicOffsetCount
                            NULL); // const uint32_t* pDynamicOffsets

    vkCmdDrawIndexed(p->commandBuffers[p->current_frame], // command buffer
                     v->nindices,
                     v->ninstances, // Instance count
                     0, // firstIndex
                     0, // vertexOffset
                     0); // firstInstance
    return;
}

void vertex_create_vertex_instance_buffer(nua_t * p, vertex_t * v)
{
    if(p->verbose > 2)
    {
        printf("-> create_vertex_instance_buffer, ninstances = %zu\n", v->ninstances);
    }
    size_t data_size = v->ninstances*7*sizeof(float);
    VkDeviceSize bufferSize = data_size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(p->vkPDevice, p->vkDevice, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  //VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  &stagingBuffer, &stagingBufferMemory);
    p->n_allocate_memory++;
    void* data;
    vkMapMemory(p->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data,
           v->instance_data,
           data_size);

    vkUnmapMemory(p->vkDevice, stagingBufferMemory);
    create_buffer(p->vkPDevice, p->vkDevice, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  &v->instanceBuffer, &v->instanceBufferMemory);
    p->n_allocate_memory++;
    copy_buffer(p, stagingBuffer, v->instanceBuffer, bufferSize);

    p->n_free_memory++;
    vkFreeMemory(p->vkDevice, stagingBufferMemory, NULL);
    p->n_destroy_buffer++;
    vkDestroyBuffer(p->vkDevice, stagingBuffer, NULL);

    return;
}

void vertex_create_vertex_buffer(nua_t * p, vertex_t * v)
{
    if(p->verbose > 2)
    {
        printf("->create_vertex_buffer\n");
    }
    assert(v->npoints > 3);
    assert(v->data != NULL);

    VkDeviceSize bufferSize = v->npoints*3*sizeof(float);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    create_buffer(p->vkPDevice, p->vkDevice, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  &stagingBuffer, &stagingBufferMemory);
    p->n_allocate_memory++;
    if(p->verbose > 2)
    {
        printf("at %p %p\n", stagingBufferMemory, &stagingBufferMemory);
    }

    void* data;
    vkMapMemory(p->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data,
           v->data,
           bufferSize);

    vkUnmapMemory(p->vkDevice, stagingBufferMemory);
    create_buffer(p->vkPDevice, p->vkDevice, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  &v->vertexBuffer, &v->vertexBufferMemory);
    p->n_allocate_memory++;
    copy_buffer(p, stagingBuffer, v->vertexBuffer, bufferSize);

    p->n_free_memory++;
    vkFreeMemory(p->vkDevice, stagingBufferMemory, NULL);
    p->n_destroy_buffer++;
    vkDestroyBuffer(p->vkDevice, stagingBuffer, NULL);

    return;
}


void vertex_create_index_buffer(nua_t * p, vertex_t * v)
{
    VkDeviceSize bufferSize = sizeof(uint32_t)*v->nindices;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    create_buffer(p->vkPDevice, p->vkDevice, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                  &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(p->vkDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, v->indices, (size_t) bufferSize);
    vkUnmapMemory(p->vkDevice, stagingBufferMemory);


    create_buffer(p->vkPDevice, p->vkDevice, bufferSize,
                  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  &v->indexBuffer, &v->indexBufferMemory);

    copy_buffer(p, stagingBuffer, v->indexBuffer, bufferSize);

    p->n_destroy_buffer++;
    vkDestroyBuffer(p->vkDevice, stagingBuffer, NULL);
    p->n_free_memory++;
    vkFreeMemory(p->vkDevice, stagingBufferMemory, NULL);
}

void vertex_create_descriptor_set_layout(nua_t * p, vertex_t * v)
{
    if(p->verbose > 2)
    {
        printf("create_descriptor_set_layout\n");
    }

    uint32_t nLayouts = 2;

#if sampler_test
    nLayouts = 3;
#endif

    VkDescriptorSetLayoutBinding uboLayoutBinding = {};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutBinding indexedLayoutBinding = {};
    indexedLayoutBinding.binding = 1;
    indexedLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    indexedLayoutBinding.descriptorCount = 1;
    indexedLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    indexedLayoutBinding.pImmutableSamplers = NULL;

    VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
    samplerLayoutBinding.binding = 2;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = NULL;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding * bindings = calloc(nLayouts,
                                                     sizeof(VkDescriptorSetLayoutBinding));
    bindings[0] = uboLayoutBinding;
    if(nLayouts > 1)
    {
        bindings[1] = indexedLayoutBinding;
    }
    if(nLayouts > 2)
    {
        bindings[2] = samplerLayoutBinding;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = nLayouts;
    //layoutInfo.pBindings = &uboLayoutBinding;
    layoutInfo.pBindings = bindings;


    VkResult result = vkCreateDescriptorSetLayout(p->vkDevice,
                                                  &layoutInfo,
                                                  NULL,
                                                  &v->descriptorSetLayout);
    require_VK_SUCCESS(result);
    free(bindings);
}

void vertex_create_descriptor_sets(nua_t * p, vertex_t * v)
{
    if(p->verbose > 2)
    { printf("-> create_descriptor_sets\n"); }

    //v->layouts
    VkDescriptorSetLayout * layouts = calloc(p->frames_in_flight,
                                             sizeof(VkDescriptorSetLayout));
    for(int kk = 0; kk<p->frames_in_flight; kk++)
    {
        memcpy(&layouts[kk], &v->descriptorSetLayout,
               sizeof(VkDescriptorSetLayout));
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = v->descriptorPool;
    allocInfo.descriptorSetCount = (uint32_t) p->frames_in_flight;
    allocInfo.pSetLayouts = layouts;

    v->descriptorSets = calloc(p->frames_in_flight,
                               sizeof(VkDescriptorSet));


    if(p->verbose > 2)
    {
        printf("Allocating %d descriptorSets\n", allocInfo.descriptorSetCount);
    }
    VkResult result = vkAllocateDescriptorSets(p->vkDevice, &allocInfo, v->descriptorSets);
    require_VK_SUCCESS(result);

    if(p->verbose > 2)
    {
        printf("vkUpdateDescriptorSets\n");
    }
    for (int kk = 0; kk < p->frames_in_flight; kk++)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = p->uniformBuffers[kk];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);
#if sampler_test
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = v->textureImageView;
        imageInfo.sampler = v->textureSampler;
#endif

#if sampler_test
        int nDescriptorWrite = 3;
        VkWriteDescriptorSet * descriptorWrite = calloc(nDescriptorWrite,
                                                        sizeof(VkWriteDescriptorSet));
        descriptorWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[0].dstSet = v->descriptorSets[kk];
        descriptorWrite[0].dstBinding = 0;
        descriptorWrite[0].dstArrayElement = 0;
        descriptorWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[0].descriptorCount = 1;
        descriptorWrite[0].pBufferInfo = &bufferInfo;

        descriptorWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[1].dstSet = v->descriptorSets[kk];
        descriptorWrite[1].dstBinding = 1;
        descriptorWrite[1].dstArrayElement = 0;
        descriptorWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite[1].descriptorCount = 1;
        descriptorWrite[1].pBufferInfo = &bufferInfo;

        descriptorWrite[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite[2].dstSet = v->descriptorSets[kk];
        descriptorWrite[2].dstBinding = 2;
        descriptorWrite[2].dstArrayElement = 0;
        descriptorWrite[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite[2].descriptorCount = 1;
        descriptorWrite[2].pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(p->vkDevice, nDescriptorWrite, descriptorWrite, 0, NULL);
        free(descriptorWrite);
#else
        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = p->descriptorSets[kk];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = NULL; // Optional
        descriptorWrite.pTexelBufferView = NULL; // Optional
        vkUpdateDescriptorSets(p->vkDevice, 1, &descriptorWrite, 0, NULL);
#endif
    }
    if(p->verbose > 2)
    {
        printf("// create_descriptor_sets\n");
    }
    free(layouts);
    return;
}

void vertex_create_texture_image(nua_t * p, vertex_t * v)
{
    if(p->verbose > 1)
    {
        printf("->create_texture_image\n");
    }
    char imfile[] = "textures/texture.png";
    int texWidth, texHeight, texChannels;
#if 0
    stbi_uc* pixels = stbi_load(imfile,
                                &texWidth,
                                &texHeight,
                                &texChannels,
                                STBI_rgb_alpha);
#else
                                texWidth = 128;
                                texHeight = 128;
                                texChannels = 4;
                                uint8_t * pixels = calloc(texWidth*texHeight*texChannels,
                                                          sizeof(uint8_t));
#endif
                                VkDeviceSize imageSize = texWidth * texHeight * 4;

                                if (!pixels) {
                                    fprintf(stderr, "failed to load texture image!");
                                    fprintf(stderr, "Could not read %s\n", imfile);
                                    exit(EXIT_FAILURE);
                                }

                                VkBuffer stagingBuffer;
                                VkDeviceMemory stagingBufferMemory;

                                create_buffer(p->vkPDevice, p->vkDevice,
                                              imageSize,
                                              VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                              &stagingBuffer,
                                              &stagingBufferMemory);


                                void* data;
                                vkMapMemory(p->vkDevice,
                                            stagingBufferMemory, 0, imageSize, 0, &data);
                                memcpy(data, pixels, (size_t) imageSize);
                                vkUnmapMemory(p->vkDevice, stagingBufferMemory);

                                //stbi_image_free(pixels);
                                free(pixels);

                                create_image(p,
                                             texWidth,
                                             texHeight,
                                             VK_FORMAT_R8G8B8A8_SRGB,
                                             VK_SAMPLE_COUNT_1_BIT,
                                             VK_IMAGE_TILING_OPTIMAL,
                                             VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                             &v->textureImage,
                                             &v->textureImageMemory);

                                transition_image_layout(p, v->textureImage,
                                                        VK_FORMAT_R8G8B8A8_SRGB, // To
                                                        VK_IMAGE_LAYOUT_UNDEFINED, // From
                                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);


                                copy_buffer_to_image(p, stagingBuffer,
                                                     v->textureImage,
                                                     (uint32_t) texWidth, (uint32_t) texHeight);

                                vkDestroyBuffer(p->vkDevice, stagingBuffer, NULL);
                                vkFreeMemory(p->vkDevice, stagingBufferMemory, NULL);

                                transition_image_layout(p, v->textureImage,
                                                        VK_FORMAT_R8G8B8A8_SRGB,
                                                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                                return;
                                }

        void vertex_destroy_texture_image(nua_t * p, vertex_t * v)
    {
        vkDestroyImage(p->vkDevice, v->textureImage, NULL);
        vkFreeMemory(p->vkDevice, v->textureImageMemory, NULL);
        return;
    }

    void vertex_create_texture_image_view(nua_t * p, vertex_t * v)
    {
        v->textureImageView = create_image_view(p,
                                                v->textureImage,
                                                VK_FORMAT_R8G8B8A8_SRGB,
                                                VK_IMAGE_ASPECT_COLOR_BIT);
    }

    void vertex_destroy_texture_image_view(nua_t * p, vertex_t * v)
    {
        vkDestroyImageView(p->vkDevice, v->textureImageView, NULL);
    }

    void vertex_destroy_texture_sampler(nua_t * p, vertex_t * v)
    {
        vkDestroySampler(p->vkDevice, v->textureSampler, NULL);
    }

    void vertex_create_texture_sampler(nua_t * p, vertex_t * v)
    {
        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        /* Options:
         * VK_SAMPLER_ADDRESS_MODE_REPEAT
         * VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT
         * VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
         * VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
         * VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER
         */
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

        samplerInfo.anisotropyEnable = VK_TRUE;

        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(p->vkPDevice, &properties);
        //printf("maxSamplerAnisotropy = %f\n", properties.limits.maxSamplerAnisotropy);
        samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        VkResult result = vkCreateSampler(p->vkDevice,
                                          &samplerInfo,
                                          NULL,
                                          &v->textureSampler);
        require_VK_SUCCESS(result);
        return;
    }

    void vertex_create_descriptor_pool(nua_t * p, vertex_t * v)
    {
#if sampler_test
        VkDescriptorPoolSize * poolSize = calloc(3,
                                                 sizeof(VkDescriptorPoolSize));
        poolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize[0].descriptorCount = (uint32_t) p->frames_in_flight;
        poolSize[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize[1].descriptorCount = (uint32_t) p->frames_in_flight;
        poolSize[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        poolSize[2].descriptorCount = (uint32_t) p->frames_in_flight;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 3;
        poolInfo.pPoolSizes = poolSize;
        poolInfo.maxSets = (uint32_t) p->frames_in_flight;

        VkResult result = vkCreateDescriptorPool(p->vkDevice,
                                                 &poolInfo,
                                                 NULL,
                                                 &v->descriptorPool);
        require_VK_SUCCESS(result);
        free(poolSize);
#else
        VkDescriptorPoolSize poolSize = {};
        poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        poolSize.descriptorCount = p->frames_in_flight*2;

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = p->frames_in_flight;
        VkResult result = vkCreateDescriptorPool(p->vkDevice, &poolInfo, NULL, &v->descriptorPool);
        require_VK_SUCCESS(result);
#endif
        return;
    }

    void vertex_destroy_descriptor_pool(nua_t * p, vertex_t * v)
    {
        vkDestroyDescriptorPool(p->vkDevice, v->descriptorPool, NULL);
        return;
    }

    void vertex_destroy_graphics_pipeline(nua_t * p, vertex_t * v)
    {
        if(p->verbose > 2)
        {
            printf("void vertex_destroy_graphics_pipeline(nua_t * p, vertex_t * v)\n");
        }
        vkDestroyShaderModule(p->vkDevice, v->fragShader, NULL);
        vkDestroyShaderModule(p->vkDevice, v->vertShader, NULL);
        vkDestroyPipeline(p->vkDevice, v->graphicsPipeline, NULL);
        vkDestroyPipelineLayout(p->vkDevice, v->pipelineLayout, NULL);
        return;
    }

    void vertex_destroy_descriptor_sets(nua_t * p, vertex_t * v)
    {
        vkDestroyDescriptorSetLayout(p->vkDevice, v->descriptorSetLayout, NULL);
        free(v->descriptorSets);
        return;
    }
