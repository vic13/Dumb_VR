#version 330 core

in vec3 L_tangent;
in vec3 V_tangent;
in vec2 uv;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform float ns;
uniform vec3 lightColor;

out vec4 color;

void main() {
    vec3 normal = texture(texture_normal1, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    
    // Calc lighting with new normals
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
    vec3 color0 = texture(texture_diffuse1, uv).xyz;
    
    /* Lighting */
    // Ambient
    float ambient_strength = 1;
    
    // Diffuse
    float cosTheta = dot(normal, L_tangent);
    float diffuse_strength = clamp(cosTheta, 0, 1);
    
    // Specular
    float cosPhi = 0;
    if (dot(L_tangent, normal)>0) {   //if bounced on outside surface
        vec3 R = normalize(2*dot(normal, L_tangent)*normal-L_tangent);
        cosPhi = dot(R, V_tangent);
    }
    float specular_strength = pow(clamp(cosPhi, 0, 1), ns);

    // Total
    float total_strength = ka*ambient_strength + kd*diffuse_strength + ks*specular_strength;
    color = vec4(total_strength * color0 * lightColor, 1);
    
}
