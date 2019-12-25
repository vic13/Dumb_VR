#version 330 core

in vec2 uv;

out vec4 color;

uniform sampler2D texSampler;
uniform float ns;
uniform vec3 lightColor;

in vec3 n;
in vec3 l;
in vec3 v;

void main() {
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
    vec3 color0 = texture(texSampler, uv).xyz;
    
    /* Lighting */
    // Ambient
    float ambient_strength = 1;
	vec3 ambient = normalize(ambient_strength * lightColor);
    
    // Diffuse
    float cosTheta = dot(n, l);
    float diffuse_strength = clamp(cosTheta, 0, 1);
	vec3 diffuse = diffuse_strength * lightColor;
    
    // Specular
    float cosPhi = 0;
    if (dot(l, n)>0) {   //if bounced on outside surface
        vec3 r = normalize(2*dot(n,l)*n-l);
        cosPhi = dot(r, v);
    }
    float specular_strength = pow(clamp(cosPhi, 0, 1), ns);
	vec3 specular = specular_strength * lightColor;

    // Total
    vec3 total = ka*ambient + kd*diffuse + ks*specular;
    color = vec4(color0 * total, 1);
//    color = vec4((h+1)/2,1); //debug : show normals
}