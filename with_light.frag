#version 330 core

#define NR_POINT_LIGHTS 10

struct Flashlight {
    vec3 color;
    float cosAngle;
    bool on;
};
struct PointLight {
    vec3 L;
    vec3 color;
};
struct Material {
    float ka;
    float kd;
    float ks;
    float ns;
};

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_flashlight;
uniform Material material;
uniform vec3 pointlightColors[NR_POINT_LIGHTS];
uniform bool bump_mapping;
uniform bool chunk; // True if we are dealing with a Chunk
uniform Flashlight flashlight;
uniform vec3 directional_color;
uniform vec3 right;
uniform bool validSelectedBlock;
uniform vec4 blockSelected;


in VS_OUT {
    vec2 uv;
    vec3 L_pointlights[NR_POINT_LIGHTS];
    vec3 L_flashlight;
    vec3 L_directional;
    vec3 flashlightDirection;
    vec3 N;
    vec3 V;
	vec4 coord; // Only used to compute the UV for the Chunk
} fs_in;

out vec4 color;


vec3 L_directional_norm = normalize(fs_in.L_directional);
vec3 L_flashlight_norm = normalize(fs_in.L_flashlight);
float flashlightDistance = length(fs_in.L_flashlight);
vec3 flashlightDirection_norm = normalize(fs_in.flashlightDirection);
vec3 N_norm = normalize(fs_in.N);
vec3 V_norm = normalize(fs_in.V);


vec3 getLightColor(vec3 L, vec3 lightColor, float lightDistance);
vec3 getFlashlightColor();
vec2 getFlashlightUV(float r, float phi);
vec3 getColorBeforeLight();

void main() {

	vec3 colorBeforeLight = getColorBeforeLight();
    
    if (bump_mapping) {
        vec3 normal = texture(texture_normal1, fs_in.uv).rgb;
        N_norm = normalize(normal * 2.0 - 1.0);
    }
    
    
    /* Lighting */
    vec3 lightColor = vec3(0.0);
    // Point light
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        lightColor += getLightColor(normalize(fs_in.L_pointlights[i]), pointlightColors[i], length(fs_in.L_pointlights[i]));
    }
    // Flashlight
    if (flashlight.on) {
        lightColor += getFlashlightColor();
    }

    float eps = 0.0001;
    if (validSelectedBlock && fs_in.coord.x + eps >= blockSelected.x && fs_in.coord.x - eps < blockSelected.x + 1) {
        if (fs_in.coord.y + eps >= blockSelected.y && fs_in.coord.y - eps < blockSelected.y + 1) {
            if (fs_in.coord.z + eps >= blockSelected.z && fs_in.coord.z - eps < blockSelected.z + 1) {
                lightColor += vec3(0.3f, 0.3f, 0.3f);
            }
        }
    }
    
    // Directional
    lightColor += getLightColor(L_directional_norm, directional_color, 0.0);
    
    color = vec4(colorBeforeLight * lightColor, 1);
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
        return 2.0*getLightColor(L_flashlight_norm, flashlight.color*colorFlashlightTex, flashlightDistance);
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
    float specular_strength = pow(clamp(cosPhi, 0, 1), material.ns);

    // Total
    float total_strength = material.ka*ambient_strength + material.kd*diffuse_strength + material.ks*specular_strength;
    float attenuation = 1.0/(1.0+0.3*lightDistance);
    
    return total_strength * lightColor * attenuation;
    
}

vec3 getColorBeforeLight() {
    if(chunk){
        vec2 chunk_uv;	
        if(fs_in.coord.w < 0) {
            chunk_uv = vec2(0.0625 * (fract(fs_in.coord.x) + fs_in.coord.w), fs_in.coord.z);
        }
		
        else {	
            chunk_uv = vec2( 0.0625*(fs_in.coord.w + fract(fs_in.coord.x + fs_in.coord.z)), - fs_in.coord.y);
        }

        return texture(texture_diffuse1, chunk_uv).xyz;
	}

    else{
        return texture(texture_diffuse1, fs_in.uv).xyz;
    }
}

