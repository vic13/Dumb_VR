#version 330 core

in vec2 uv;

out vec4 color;

uniform float intensity;
uniform sampler2D texSampler;

in vec3 normalFrag;
in vec3 lightFrag;

void main() {
    vec3 color0 = texture(texSampler, uv).xyz;
    
    /* Lighting */
    // Ambient
    float ambient = 0.2;
    
    // Diffuse
    vec3 n = normalize(normalFrag);
    vec3 l = normalize(lightFrag);
    float cosTheta = dot(n, l);
    float diffuse = clamp(cosTheta, 0, 1);
    
    // Total
    float total = clamp(ambient+diffuse, 0, 1);
    
    color = vec4(color0*total, 1);
//    color = vec4((n+1)/2,1); //debug : show normals
}
