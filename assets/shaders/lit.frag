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
    float inner_cutoff;
    float outer_cutoff;
};

#define MAX_LIGHTS 16
uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

struct Material {
    sampler2D albedo;
    sampler2D specular;
    sampler2D roughness;
    sampler2D ambient_occlusion;
    sampler2D emission;
    float shininess;
    float alphaThreshold;
};

uniform Material material;

out vec4 frag_color;

//Light computation functions
vec3 ComputeLight();
vec3 ComputeDirectionalLight();
// vec3 ComputePointLights();
// vec3 ComputeSpotLights();

void main(){
    frag_color = vec4(ComputeLight(), texture(material.albedo, fs_in.tex_coord).a);
    if (frag_color.a < material.alphaThreshold) {
        discard;
    }
}

vec3 ComputeLight()
{
	vec3 color = ComputeDirectionalLight();
	// color += ComputePointLights();
	// color += ComputeSpotLights();
	return color;
}

vec3 ComputeDirectionalLight()
{
	vec4 ret = vec4(0.0f);
	vec3 ambient = vec3(0.0f);
	vec3 diffuse = vec3(0.0f);
	vec3 specular = vec3(0.0f);
	//Loop on all lights in the scene
	for(int i = 0; i < lightCount; i++)
	{
        if (lights[i].type != TYPE_DIRECTIONAL)
            continue;

		vec3 normal = normalize(fs_in.normal);
        vec3 view = normalize(fs_in.view);
        vec3 reflected = reflect(lights[i].direction, normal);

        float lambert = max(dot(normal, -lights[i].direction), 0.0);
        float phong = pow(max(dot(reflected, view), 0.0), material.shininess);

        vec4 albedo = lambert * texture(material.albedo, fs_in.tex_coord) * vec4(lights[i].diffuse, 1);
        vec4 specular = phong * texture(material.specular, fs_in.tex_coord)  * vec4(lights[i].specular, 1);
        vec4 ambient = texture(material.albedo, fs_in.tex_coord)  * vec4(lights[i].ambient, 1);
        ret += (albedo + ambient + specular);
	}

	return ret.rgb * fs_in.color.rgb;
}