#version 330 core

layout(location=0) in vec4 coord;
layout(location=1) in vec3 normal;

struct PointLight {
    vec3 position;
};
#define NR_POINT_LIGHTS 10

uniform vec3 L_directional;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform vec3 flashlightPosition;
uniform vec3 flashlightDirection;
uniform vec3 cameraPosition;
uniform mat4 mvp;
uniform mat4 m;

out VS_OUT {
    vec2 uv;
    vec3 L_pointlights[NR_POINT_LIGHTS];
    vec3 L_flashlight;
    vec3 L_directional;
    vec3 flashlightDirection;
    vec3 N;
    vec3 V;
	vec4 coord;
} vs_out;

void main(void) {
	vs_out.coord = coord;
	gl_Position = mvp * vec4(coord.xyz, 1);

	if(coord.w < 0){
		vs_out.uv = vec2(0.0625 * (fract(coord.x) + coord.w), coord.z);
	}
	else{	 	
		vs_out.uv = vec2(0.0625 * (coord.w + fract(coord.x + coord.z)), - coord.y); 
	}

	//vs_out.uv = uv; 
    vec3 position_worldCoord = (m * vec4(coord.xyz , 1)).xyz;
    
    // The following vectors are normalized in the fragment shader (useless to normalize before interpolation)
    
    // L for directional light
    vs_out.L_directional = L_directional;
    // L for point lights
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        vs_out.L_pointlights[i] = pointLights[i].position-position_worldCoord; // vector vertex -> point light
    }
    // L for flashlight
    vs_out.L_flashlight = flashlightPosition-position_worldCoord;  // vector vertex -> flashlight
    // Flashlight direction
    vs_out.flashlightDirection = flashlightDirection;
    // V
    vs_out.V = cameraPosition-position_worldCoord;                 // vector vertex -> camera
    // N
    vs_out.N = (vec4(normal,0)).xyz;;                            // takes into account rotation of the model
        
}
