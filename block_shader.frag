#version 330 core


in vec4 texcoord;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_flashlight;
uniform float ns;
uniform vec3 pointlightColor;

struct Flashlight {
    vec3 color;
    vec3 direction;
    float cosAngle;
    bool on;
};
uniform Flashlight flashlight;

uniform vec3 right;

in vec3 L_pointlight;
in vec3 L_flashlight;
in vec3 N;
in vec3 V;
in vec3 test;

vec3 getLightColor(vec3 L, vec3 lightColor, float lightDistance);
vec3 getFlashlightColor();
vec2 getFlashlightUV(float r, float phi);


out vec4 color;



void main(void) {
	vec2 uv;
	if(texcoord.w < 0){
		uv = vec2(0.0625 * fract(texcoord.x) +  0.0625 * (texcoord.w), texcoord.z);
	}
		
	else{	
		float number = fract((texcoord.x + texcoord.z));	
		uv = vec2( 0.0625*(texcoord.w) + 0.0625*number, - texcoord.y); 
	}

	vec3 color0 = texture(texture_diffuse1, uv).xyz;

	/* Lighting */
    vec3 lightColor = vec3(0.0);
    // Point light
    lightColor += getLightColor(L_pointlight, pointlightColor, 0.0);
    // Flashlight
    if (flashlight.on) {
        lightColor += getFlashlightColor();
    }
    
    color = vec4(color0 * lightColor, 1);

	//color = vec4((N + 1)/2, 1.0f);
}

vec3 getFlashlightColor() {
    float cosTheta = dot(normalize(-flashlight.direction), normalize(L_flashlight));
    
    if (cosTheta > flashlight.cosAngle) {
        float theta = acos(cosTheta);
        float maxAngle = acos(flashlight.cosAngle);
        float r = 0.5*theta/maxAngle;
        
        vec3 L_proj = L_flashlight - dot(L_flashlight, flashlight.direction)*flashlight.direction;
        L_proj *= -1;
        float cosPhi = dot(normalize(right), normalize(L_proj));
        float phi = acos(cosPhi);
        if (-L_flashlight.y > flashlight.direction.y) {
            phi *= -1;
        }
        vec2 flashlight_uv = getFlashlightUV(r, phi);
        vec3 colorFlashlightTex = texture(texture_flashlight, flashlight_uv).xyz;
        return getLightColor(L_flashlight, flashlight.color*colorFlashlightTex, length(L_flashlight));
    } else {
        return vec3(0.0);
    }
}

vec2 getFlashlightUV(float r, float phi) {
    float u = 0.5+r*cos(phi);
    float v = 0.5+r*sin(phi);
    return vec2(u,v);
}

vec3 getLightColor(vec3 L, vec3 lightColor, float lightDistance) {
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
    // Ambient
    float ambient_strength = 1;
    
    // Diffuse
    float cosTheta = dot(normalize(N), normalize(L));
    float diffuse_strength = clamp(cosTheta, 0, 1);
    
    // Specular
    float cosPhi = 0;
    if (cosTheta>0) {   //if bounced on outside surface
        vec3 R = 2*cosTheta*normalize(N) - normalize(L);
        cosPhi = dot(normalize(R), normalize(V));
    }
    float specular_strength = pow(clamp(cosPhi, 0, 1), ns);

    // Total
    float total_strength = ka*ambient_strength + kd*diffuse_strength + ks*specular_strength;
    float attenuation = 1.0/(1.0+lightDistance);
    
    return total_strength * lightColor * attenuation;
    
}
