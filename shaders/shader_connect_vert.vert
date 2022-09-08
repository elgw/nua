#version 450

layout(binding = 0) uniform UniformBufferObject {
mat4 model;
mat4 view;
mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;

layout(location = 1) in vec3 instance_from;
layout(location = 2) in vec3 instance_to;
layout(location = 3) in float instance_radius;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec2 texCoord;

// use
// gl_InstanceIndex
// if vkCmdDrawIndexed has an instance count > 1
// in that case, supply the center of the models in the ubo ... 

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}


void main() {

vec3 pos = inPosition;

// Normal is the same as the position, although
// the Z-position should be reset
fragNormal = pos;
fragNormal.z = 0.0;
fragNormal = normalize(fragNormal);


// vshape_tube is elongated along z, from 0 to 1, so we scale z to get the correct length
pos.z = pos.z * length(instance_to - instance_from);

// We scale in the (x,y) plane to get the desired radius
float r = length(vec2(pos.x, pos.y)); 
pos.x = pos.x * instance_radius / r;
pos.y = pos.y * instance_radius / r;

// Next we need to rotate the cube along the two supplied points
vec3 d1 =  (instance_to - instance_from);
d1 = normalize(d1);
vec3 d2 = vec3(0, 0, 1);
vec3 rot_axis = cross(d1, d2);

float theta = 0;

mat4 rot = mat4(1.0, 0.0, 0.0, 0.0,  // 1. column
                  0.0, 1.0, 0.0, 0.0,  // 2. column
                  0.0, 0.0, 1.0, 0.0,  // 3. column
                  0.0, 0.0, 0.0, 1.0); // 4. column

if(abs(length(rot_axis)) > 0.000001)
{        
        theta = acos(dot(d1,d2));
        rot = rotationMatrix(rot_axis, theta);
} 


// instance position

// New position for the vertex before MVP

vec4 npos = (rot * vec4(pos, 1.0) + vec4(instance_from, 1.0));
npos.w = 1;

vec4 fragNormal4 = (rot*vec4(fragNormal, 1.0));
fragNormal4.w = 1;

gl_Position = ubo.proj * ubo.view * ubo.model * npos;
fragNormal4 = - ubo.model * fragNormal4;
fragNormal = vec3(fragNormal4);

// Swap back to OpenGl way
    gl_Position.y = -gl_Position.y;

//fragNormal = -vec3(ubo.model * vec4(fragNormal, 1.0));    

fragColor = vec3(0.9, 0.9, 0);

}

