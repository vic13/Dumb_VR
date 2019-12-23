#version 330 core

in vec2 uv;

out vec4 color;

uniform float intensity;
uniform sampler2D texSampler;
uniform float ns;

in vec3 n;
in vec3 l;
in vec3 v;

void main() {
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
    //vec3 lightColor = vec3(0.8,1,0.8); // Green light
    
    vec3 color0 = texture(texSampler, uv).xyz;
    
    /* Lighting */
    // Ambient
    float ambient = 1;
    
    // Diffuse
    float cosTheta = dot(n, l);
    float diffuse = clamp(cosTheta, 0, 1);
    
    // Specular
//    vec3 h = normalize(l+v);
//    float cosPsi = dot(h, v);
//    float specular = pow(clamp(cosPsi, 0, 1),300);
    
    
    float cosPhi = 0;
    if (dot(l, n)>0) {   //if bounced on outside surface
        vec3 r = normalize(2*dot(n,l)*n-l);
        cosPhi = dot(r, v);
    }
    float specular = pow(clamp(cosPhi, 0, 1), ns);
    
    // Total
    float total = ka*ambient + kd*diffuse + ks*specular;
//    total = specular;
    
//    float a = clamp(cosPsi, 0, 1);
//    color = vec4(a, a, a, 1);
    color = vec4(color0*total, 1);
//    color = vec4((h+1)/2,1); //debug : show normals
}
