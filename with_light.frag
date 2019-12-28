#version 330 core



out vec4 color;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_flashlight;
uniform float ns;
uniform vec3 pointlightColor;

struct Flashlight {
    vec3 color;
    float cosAngle;
    bool on;
};
uniform Flashlight flashlight;
uniform bool bump_mapping;

uniform vec3 right;

in VS_OUT {
    vec2 uv;
    vec3 L_pointlight;
    vec3 L_flashlight;
    vec3 flashlightDirection;
    vec3 N;
    vec3 V;
} fs_in;

vec3 L_pointlight_norm = normalize(fs_in.L_pointlight);
vec3 L_flashlight_norm = normalize(fs_in.L_flashlight);
vec3 flashlightDirection_norm = normalize(fs_in.flashlightDirection);
vec3 N_norm = normalize(fs_in.N);
vec3 V_norm = normalize(fs_in.V);
float flashlightDistance = length(fs_in.L_flashlight);

vec3 getLightColor(vec3 L, vec3 lightColor, float lightDistance);
vec3 getFlashlightColor();
vec2 getFlashlightUV(float r, float phi);

void main() {
    if (bump_mapping) {
        vec3 normal = texture(texture_normal1, fs_in.uv).rgb;
        N_norm = normalize(normal * 2.0 - 1.0);
    }
    
    
    vec3 color0 = texture(texture_diffuse1, fs_in.uv).xyz;
    
    /* Lighting */
    vec3 lightColor = vec3(0.0);
    // Point light
    lightColor += getLightColor(L_pointlight_norm, pointlightColor, 0.0);
    // Flashlight
    if (flashlight.on) {
        lightColor += getFlashlightColor();
    }
    
    color = vec4(color0 * lightColor, 1);
}


vec3 getFlashlightColor() {
    float cosTheta = dot(normalize(-flashlightDirection_norm), L_flashlight_norm);
    
    if (cosTheta > flashlight.cosAngle) {
        float theta = acos(cosTheta);
        float maxAngle = acos(flashlight.cosAngle);
        float r = 0.5*theta/maxAngle;

        vec3 L_proj = L_flashlight_norm - dot(L_flashlight_norm, flashlightDirection_norm)*flashlightDirection_norm;
        vec3 L_proj_norm = -1*normalize(L_proj);
        float cosPhi = dot(normalize(right), L_proj_norm);
        float phi = acos(cosPhi);
//        if (-L_flashlight_norm.y > flashlightDirection_norm.y) {
//            phi *= -1;
//        }
        vec2 flashlight_uv = getFlashlightUV(r, phi);
        vec3 colorFlashlightTex = texture(texture_flashlight, flashlight_uv).xyz;
        return getLightColor(L_flashlight_norm, flashlight.color*colorFlashlightTex, flashlightDistance);
        return flashlight.color;
    } else {
        return vec3(0.0);
    }
}

vec2 getFlashlightUV(float r, float phi) {
    float u = 0.5+r*cos(phi);
    float v = 0.5+r*sin(phi);
    return vec2(u,v);
}

vec3 getLightColor(vec3 L_norm, vec3 lightColor, float lightDistance) {
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
    // Ambient
    float ambient_strength = 1;
    
    // Diffuse
    float cosTheta = dot(N_norm, L_norm);
    float diffuse_strength = clamp(cosTheta, 0, 1);
    
    // Specular
    float cosPhi = 0;
    if (cosTheta>0) {   //if bounced on outside surface
        vec3 R = 2*cosTheta*N_norm - L_norm;
        cosPhi = dot(normalize(R), V_norm);
    }
    float specular_strength = pow(clamp(cosPhi, 0, 1), ns);

    // Total
    float total_strength = ka*ambient_strength + kd*diffuse_strength + ks*specular_strength;
    float attenuation = 1.0/(1.0+lightDistance);
    
    return total_strength * lightColor * attenuation;
    
}
