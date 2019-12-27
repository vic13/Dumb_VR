#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D texture_diffuse1;
uniform float ns;
uniform vec3 pointlightColor;

struct Flashlight {
    vec3 color;
    vec3 direction;
    float angle;
    bool on;
};
uniform Flashlight flashlight;

in vec3 L_pointlight;
in vec3 L_flashlight;
in vec3 N;
in vec3 V;

vec3 getLightColor(vec3 L, vec3 lightColor);
vec3 getFlashlightColor();

void main() {
    vec3 color0 = texture(texture_diffuse1, uv).xyz;
    
    /* Lighting */
    vec3 lightColor = vec3(0.0);
    // Point light
    lightColor += getLightColor(L_pointlight, pointlightColor);
    // Flashlight
    if (flashlight.on) {
        lightColor += getFlashlightColor();
    }
    
    color = vec4(color0 * lightColor, 1);
}


vec3 getFlashlightColor() {
    if (dot(-flashlight.direction, L_flashlight) > flashlight.angle) {
        return getLightColor(L_flashlight, flashlight.color);
    } else {
        return vec3(0.0);
    }
}

vec3 getLightColor(vec3 L, vec3 lightColor) {
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
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
    
    return total_strength * lightColor;
    
}
