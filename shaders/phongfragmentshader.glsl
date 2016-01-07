#version 330 core

uniform mat4 NM;
uniform vec4 color;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;
uniform float specularity;
uniform float shinyness;

in vec3 normal;
in vec3 v;
in vec3 light_direction;

out vec4 frag_color;


vec4 calcShading( vec3 N, vec3 L ) {
    
    //Ambient contribution
    vec4 Iamb = ambientColor;

    //Diffuse contribution
    vec4 Idiff = diffuseColor * max(dot(N,L), 0.0);
    Idiff = clamp(Idiff, 0.0, 1.0);

    return Iamb + Idiff;
}

vec4 calcSpecularShading( vec3 N, vec3 L ) {

    //Specular contribution
    vec3 v_dir = normalize(-v);
    vec3 R = normalize(reflect(-L, normalize(N)));
    const float specExp = 10.0;
    float spec = dot(R, v_dir);
    spec = (spec > 0.0) ? (1.0 * pow(spec, specularity)) : 0.0;

    vec4 Ispec = specularColor
        * spec;
    Ispec = clamp(Ispec, 0.0, 1.0);

    return Ispec;
}

void main() {

    v;
    light_direction;

    frag_color = color;

    frag_color.rgb *= calcShading(
        normalize(vec3(NM * vec4(normal, 1.0))),
        light_direction).rgb * 1.0;
    
    frag_color.rgb += calcSpecularShading(
        normalize(vec3(NM * vec4(normal, 1.0))),
        light_direction).rgb * shinyness;
}