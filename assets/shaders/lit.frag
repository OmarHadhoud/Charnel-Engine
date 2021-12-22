#version 330 core

in Varyings {
    vec4 color;
    vec2 tex_coord;
    vec3 world;
    vec3 view;
    vec3 normal;
} fs_in;

#define TYPE_DIRECTIONAL 0
#define TYPE_POINT 1
#define TYPE_SPOT 2

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

#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

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

uniform Material material;

out vec4 frag_color;

//Light computation functions
vec3 ComputeLight();
vec3 ComputeDirectionalLight();
vec3 ComputePointLights();
vec3 ComputeSpotLights();

void main(){
    frag_color = vec4(ComputeLight(), texture(material.albedo, fs_in.tex_coord).a);
    if (frag_color.a < material.alphaThreshold) {
        discard;
    }
    frag_color += vec4(texture(material.emission, fs_in.tex_coord).rgb * material.emission_tint, 0.0f);
}

vec3 ComputeLight()
{
	vec3 color = ComputeDirectionalLight();
	color += ComputePointLights();
	color += ComputeSpotLights();
	return color;
}

vec3 ComputeDirectionalLight()
{
	vec3 ret = vec3(0.0f);
	//Loop on all lights in the scene
	for(int i = 0; i < lightCount; i++)
	{
        if (lights[i].type != TYPE_DIRECTIONAL)
            continue;

		vec3 normal = normalize(fs_in.normal);
        vec3 view = normalize(fs_in.view);
        vec3 reflected = reflect(lights[i].direction, normal);

        float lambert = max(dot(normal, -lights[i].direction), 0.0);

        float roughness = mix(material.roughness_range.x, material.roughness_range.y, texture(material.roughness, fs_in.tex_coord).r);
        float shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
        if (material.shininess > 0.1)
            shininess = material.shininess;
        float phong = pow(max(dot(reflected, view), 0.0), shininess);

        vec3 albedo = lambert * texture(material.albedo, fs_in.tex_coord).rgb * material.albedo_tint * lights[i].diffuse;
        vec3 specular = phong * texture(material.specular, fs_in.tex_coord).rgb * material.specular_tint  * lights[i].specular;
        vec3 ambient = texture(material.albedo, fs_in.tex_coord).rgb  * lights[i].ambient * material.albedo_tint;
        ambient *= texture(material.ambient_occlusion, fs_in.tex_coord).r;
        ret += (albedo + ambient + specular);
	}

	return ret.rgb * fs_in.color.rgb;
}

vec3 ComputePointLights()
{
    vec3 ret = vec3(0.0f);
	//Loop on all point lights in the scene
	for(int i = 0; i < lightCount; i++)
	{
        if (lights[i].type != TYPE_POINT)
            continue;

		vec3 normal = normalize(fs_in.normal);
        vec3 view = normalize(fs_in.view);

        vec3 light_direction = fs_in.world - lights[i].position;
        float d = length(light_direction);
        light_direction /= d;

        float attenuation = 1.0 / (lights[i].attenuationConstant + lights[i].attenuationLinear * d + lights[i].attenuationQuadratic * d * d);

        vec3 reflected = reflect(light_direction, normal);

        float lambert = max(dot(normal, -light_direction), 0.0);

        float roughness = mix(material.roughness_range.x, material.roughness_range.y, texture(material.roughness, fs_in.tex_coord).r);
        float shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
        if (material.shininess > 0.1)
            shininess = material.shininess;
        float phong = pow(max(dot(reflected, view), 0.0), shininess);

        vec3 albedo = lambert * texture(material.albedo, fs_in.tex_coord).rgb * lights[i].diffuse;
        vec3 specular = phong * texture(material.specular, fs_in.tex_coord).rgb  * lights[i].specular;
        vec3 ambient = texture(material.albedo, fs_in.tex_coord).rgb  * lights[i].ambient * material.albedo_tint;
        ambient *= texture(material.ambient_occlusion, fs_in.tex_coord).r;
        ret += ambient + ((albedo + specular) * attenuation);
	}

	return ret.rgb * fs_in.color.rgb;
}

vec3 ComputeSpotLights()
{
    vec3 ret = vec3(0.0f);
	//Loop on all point lights in the scene
	for(int i = 0; i < lightCount; i++)
	{
        if (lights[i].type != TYPE_SPOT)
            continue;

		vec3 normal = normalize(fs_in.normal);
        vec3 view = normalize(fs_in.view);

        vec3 light_direction = fs_in.world - lights[i].position;
        float d = length(light_direction);
        light_direction /= d;

        float attenuation = 1.0 / (lights[i].attenuationConstant + lights[i].attenuationLinear * d + lights[i].attenuationQuadratic * d * d);

        float angle = acos(dot(normalize(lights[i].direction), light_direction));
        attenuation *= smoothstep(lights[i].outerCutoff, lights[i].innerCutoff, angle);

        vec3 reflected = reflect(light_direction, normal);

        float lambert = max(dot(normal, -light_direction), 0.0);

        float roughness = mix(material.roughness_range.x, material.roughness_range.y, texture(material.roughness, fs_in.tex_coord).r);
        float shininess = 2.0f/pow(clamp(roughness, 0.001f, 0.999f), 4.0f) - 2.0f;
        if (material.shininess > 0.1)
            shininess = material.shininess;
        float phong = pow(max(dot(reflected, view), 0.0), shininess);

        vec3 albedo = lambert * texture(material.albedo, fs_in.tex_coord).rgb * lights[i].diffuse;
        vec3 specular = phong * texture(material.specular, fs_in.tex_coord).rgb  * lights[i].specular;
        vec3 ambient = texture(material.albedo, fs_in.tex_coord).rgb  * lights[i].ambient * material.albedo_tint;
        ambient *= texture(material.ambient_occlusion, fs_in.tex_coord).r;
        ret += ambient + ((albedo + specular) * attenuation);
	}

	return ret.rgb * fs_in.color.rgb;
}