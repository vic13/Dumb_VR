#version 330 core

in vec3 l;
in vec3 v;
in mat3 TBN;

in vec2 uv;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;
uniform float ns;
uniform vec3 lightColor;

out vec4 color;

void main() {
    //vec3 colorTangent = (T+1.0f)/2.0f;
    //color = vec4(colorTangent, 1.0);
    //vec4 color0 = texture(texSampler, uv);
    //color = texture(texture_diffuse1, uv);
    vec3 normal = texture(texture_normal1, uv).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(TBN * normal);
    
    // Calc lighting with new normals
    float ka = 0.3;
    float kd = 0.4;
    float ks = 0.3;
    
    vec3 color0 = texture(texture_diffuse1, uv).xyz;
    
    /* Lighting */
    // Ambient
    float ambient_strength = 1;
    vec3 ambient = normalize(ambient_strength * lightColor);
    
    // Diffuse
    float cosTheta = dot(normal, l);
    float diffuse_strength = clamp(cosTheta, 0, 1);
    vec3 diffuse = diffuse_strength * lightColor;
    
    // Specular
    float cosPhi = 0;
    if (dot(l, normal)>0) {   //if bounced on outside surface
        vec3 r = normalize(2*dot(normal,l)*normal-l);
        cosPhi = dot(r, v);
    }
    float specular_strength = pow(clamp(cosPhi, 0, 1), ns);
    vec3 specular = specular_strength * lightColor;

    // Total
    vec3 total = ka*ambient + kd*diffuse + ks*specular;
    color = vec4(color0 * total, 1);
    
}
