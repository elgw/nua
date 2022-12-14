#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec2 texCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 2) uniform sampler2D texSampler;

void main() {     

    float ambientStrength = 0.4; 
    float diffuseStrength = 0.6;
    float specularStrength = 4.2;

    vec3 _fragNormal = normalize(fragNormal);

    vec3 ambient = ambientStrength * fragColor;

    vec3 view = vec3(1, -1 ,-3);
    
    view = normalize(view);

    vec3 diffuse = max(dot(_fragNormal, view), 0) * diffuseStrength * fragColor;

    vec3 specular = pow(max(dot(_fragNormal, view),0),32)*specularStrength * fragColor;

    outColor = vec4(ambient + diffuse + specular, 1.0);
    //    vec4 texColor = texture(texSampler, (texCoord+1.0)/2.0);
    //    outColor = outColor + texColor;
}
