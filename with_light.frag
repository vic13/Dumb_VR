#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform float ns;
uniform vec3 lightColor;

in vec3 N;
in vec3 L;
in vec3 V;

void main() {
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
    vec3 color0 = texture(texture_diffuse1, uv).xyz;
    
    /* Lighting */
    // Ambient
    float ambient_strength = 1;
    
    // Diffuse
    float cosTheta = dot(N, L);
    float diffuse_strength = clamp(cosTheta, 0, 1);
    
    // Specular
    float cosPhi = 0;
    if (dot(L, N)>0) {   //if bounced on outside surface
        vec3 R = normalize(2*dot(N, L)*N - L);
        cosPhi = dot(R, V);
    }
    float specular_strength = pow(clamp(cosPhi, 0, 1), ns);

    // Total
    float total_strength = ka*ambient_strength + kd*diffuse_strength + ks*specular_strength;
    color = vec4(total_strength * color0 * lightColor, 1);
}
