#version 450

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;

layout(location = 1) in vec3 instance_pos;
layout(location = 2) in vec3 instance_color;
layout(location = 3) in float instance_radius;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 texCoord;

// use
// gl_InstanceIndex
// if vkCmdDrawIndexed has an instance count > 1
// in that case, supply the center of the models in the ubo ... 

void main() {

     vec3 pos = inPosition;
     texCoord.x = inPosition.x;
     texCoord.y = inPosition.y;
     
     fragNormal = normalize(pos);
     pos =  pos*instance_radius + instance_pos;
          
     gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);
// Swap back to OpenGl way
    gl_Position.y = -gl_Position.y;
    
    fragNormal = vec3(ubo.model * vec4(fragNormal, 1.0));    
       
    fragColor = instance_color;
    
}
