#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
} fs_in;

// the different types of lights we have, notice that the order must be consistent with the order in the light component
#define TYPE_DIRECTIONAL 0
#define TYPE_POINT 1
#define TYPE_SPOT 2

// the light struct for all types, similar to the light component in the ecs
struct Light {
    int type;
    vec3 position;
    vec3 direction;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    float attenuationConstant;
    float attenuationLinear;
    float attenuationQuadratic;
    float innerCutoff;
    float outerCutoff;
};

// we have a maximum number of lights that can be used in a scene
#define MAX_LIGHTS 16
// the uniforms for the lights and the lightcount
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

// the struct for the material of the object we are currently drawing, should be consistent with the litMaterial component in the ecs
struct Material {
    sampler2D albedo;
    vec3 albedo_tint;
    sampler2D specular;
    vec3 specular_tint;
    sampler2D roughness;
    vec2 roughness_range;
    sampler2D ambient_occlusion;
    sampler2D emission;
    vec3 emission_tint;
    float shininess;
    float alphaThreshold;
};
// the uniform for the material we're drawing
uniform Material material;

out vec4 frag_color;

//Light computation functions

// this function computes all the lighting in the scene for the fragment
vec3 ComputeLight();
// this function computes the effect of a directional light for the fragment in the scene
vec3 ComputeDirectionalLight(Light light);
// this function computes the effect of a point light for the fragment in the scene
vec3 ComputePointLight(Light light);
// this function computes the effect of a spot light for the fragment in the scene
vec3 ComputeSpotLight(Light light);


void main(){
    // the color of the fragment is the color of the object in the lightings in the scene, alpha is not affected though by lighting
    frag_color = vec4(ComputeLight(), texture(material.albedo, fs_in.tex_coord).a);
    // discard if the fragment is not visible (transparent)
    if (frag_color.a < material.alphaThreshold) {
        discard;
    }
    // if the fragment is not to be discarded, then add to it the emissive texture as it is independent of lighting
    frag_color += vec4(texture(material.emission, fs_in.tex_coord).rgb * material.emission_tint, 0.0f);
}

vec3 ComputeLight()
{
    vec3 color = vec3(0.0f);
    for(int i = 0; i < lightCount; i++)
	{
        if (lights[i].type == TYPE_DIRECTIONAL)
            color += ComputeDirectionalLight(lights[i]);
        else if (lights[i].type == TYPE_POINT)
            color += ComputePointLight(lights[i]);
        else if (lights[i].type == TYPE_SPOT)
            color += ComputeSpotLight(lights[i]);
    }
	return color;
}

vec3 ComputeDirectionalLight(Light light)
{
    vec3 normal = normalize(fs_in.normal);
    vec3 view = normalize(fs_in.view);
    vec3 reflected = reflect(light.direction, normal);

    float lambert = max(dot(normal, -light.direction), 0.0);

    float roughness = mix(material.roughness_range.x, material.roughness_range.y, texture(material.roughness, fs_in.tex_coord).r);
    float shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
    if (material.shininess > 0.1)
        shininess = material.shininess;
    float phong = pow(max(dot(reflected, view), 0.0), shininess);

    vec3 albedo = lambert * texture(material.albedo, fs_in.tex_coord).rgb * material.albedo_tint * light.diffuse;
    vec3 specular = phong * texture(material.specular, fs_in.tex_coord).rgb * material.specular_tint  * light.specular;
    vec3 ambient = texture(material.albedo, fs_in.tex_coord).rgb  * light.ambient * material.albedo_tint;
    ambient *= texture(material.ambient_occlusion, fs_in.tex_coord).r;
    vec3 ret = (albedo + ambient + specular);

	return ret.rgb * fs_in.color.rgb;
}

vec3 ComputePointLight(Light light)
{
    vec3 normal = normalize(fs_in.normal);
    vec3 view = normalize(fs_in.view);

    vec3 light_direction = fs_in.world - light.position;
    float d = length(light_direction);
    light_direction /= d;

    float attenuation = 1.0 / (light.attenuationConstant + light.attenuationLinear * d + light.attenuationQuadratic * d * d);

    vec3 reflected = reflect(light_direction, normal);

    float lambert = max(dot(normal, -light_direction), 0.0);

    float roughness = mix(material.roughness_range.x, material.roughness_range.y, texture(material.roughness, fs_in.tex_coord).r);
    float shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
    if (material.shininess > 0.1)
        shininess = material.shininess;
    float phong = pow(max(dot(reflected, view), 0.0), shininess);

    vec3 albedo = lambert * texture(material.albedo, fs_in.tex_coord).rgb * light.diffuse;
    vec3 specular = phong * texture(material.specular, fs_in.tex_coord).rgb  * light.specular;
    vec3 ambient = texture(material.albedo, fs_in.tex_coord).rgb  * light.ambient * material.albedo_tint;
    ambient *= texture(material.ambient_occlusion, fs_in.tex_coord).r;
    vec3 ret = ambient + ((albedo + specular) * attenuation);

	return ret.rgb * fs_in.color.rgb;
}

vec3 ComputeSpotLight(Light light)
{
    vec3 normal = normalize(fs_in.normal);
    vec3 view = normalize(fs_in.view);

    vec3 light_direction = fs_in.world - light.position;
    float d = length(light_direction);
    light_direction /= d;

    float attenuation = 1.0 / (light.attenuationConstant + light.attenuationLinear * d + light.attenuationQuadratic * d * d);

    float angle = acos(dot(normalize(light.direction), light_direction));
    attenuation *= smoothstep(light.outerCutoff, light.innerCutoff, angle);

    vec3 reflected = reflect(light_direction, normal);

    float lambert = max(dot(normal, -light_direction), 0.0);

    float roughness = mix(material.roughness_range.x, material.roughness_range.y, texture(material.roughness, fs_in.tex_coord).r);
    float shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
    if (material.shininess > 0.1)
        shininess = material.shininess;
    float phong = pow(max(dot(reflected, view), 0.0), shininess);

    vec3 albedo = lambert * texture(material.albedo, fs_in.tex_coord).rgb * light.diffuse;
    vec3 specular = phong * texture(material.specular, fs_in.tex_coord).rgb  * light.specular;
    vec3 ambient = texture(material.albedo, fs_in.tex_coord).rgb  * light.ambient * material.albedo_tint;
    ambient *= texture(material.ambient_occlusion, fs_in.tex_coord).r;
    vec3 ret = ambient + ((albedo + specular) * attenuation);

	return ret.rgb * fs_in.color.rgb;
}