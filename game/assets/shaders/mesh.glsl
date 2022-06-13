// -----------------------------
// -- Hazel Engine PBR shader --
// -----------------------------
// Note: this shader is still very much in progress. There are likely many bugs and future additions that will go in.
//       Currently heavily updated. 
//
// References upon which this is based:
// - Unreal Engine 4 PBR notes (https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf)
// - Frostbite's SIGGRAPH 2014 paper (https://seblagarde.wordpress.com/2015/07/14/siggraph-2014-moving-frostbite-to-physically-based-rendering/)
// - Michał Siejak's PBR project (https://github.com/Nadrin)
// - My implementation from years ago in the Sparky engine (https://github.com/TheCherno/Sparky)

#type vertex
#version 430 core                                                               
                                                                                    
layout (location = 0) in vec3 a_position;                                             
layout (location = 1) in vec3 a_normal;                                             
layout (location = 2) in vec2 a_tex_coord;                                               
layout (location = 3) in ivec4 bone_ids;
layout (location = 4) in vec4 weights;

const int MAX_BONES = 100;

uniform mat4 u_view_projection;
uniform mat4 u_transform;
uniform mat4 gBones[MAX_BONES];
uniform int num_bones;

out vec2 v_tex_coord;
out vec3 v_normal;                                                                   
out vec3 v_position;  
out vec4 v_pos;

void main()
{
  vec4 pos_l;
	vec4 normal_l;
	if(num_bones>0){

		mat4 bone_transform;

		if(weights[0] <= 0)
		{
			 bone_transform = mat4(1.0f);
			 bone_transform += gBones[bone_ids[0]] * weights[0];
		}
		else
		{
			bone_transform      = gBones[bone_ids[0]] * weights[0];
		}
		bone_transform     += gBones[bone_ids[1]] * weights[1];
		bone_transform     += gBones[bone_ids[2]] * weights[2];
		bone_transform     += gBones[bone_ids[3]] * weights[3];
		pos_l = bone_transform * vec4(a_position, 1.0);
		normal_l = bone_transform * vec4(a_normal, 0.0);
		gl_Position  = u_view_projection * u_transform * pos_l;
		v_pos = gl_Position;
		v_tex_coord    = a_tex_coord;
		v_normal      = (u_transform * normal_l).xyz;
		v_position    = vec3(u_transform * vec4(a_position, 1.0));
	} else {
		v_pos = u_view_projection * u_transform * vec4(a_position, 1.0); 
		gl_Position = v_pos;
		v_tex_coord = a_tex_coord;
		v_normal = mat3(transpose(inverse(u_transform))) * a_normal;
		v_position = vec3(u_transform * vec4(a_position, 1.0));
	}
}

#type fragment
#version 430 core

layout(location = 0) out vec4 o_color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
	float transparency;
}; 

const int MAX_POINT_LIGHTS = 2;
const int MAX_SPOT_LIGHTS = 2;

in vec2 v_tex_coord;
in vec3 v_normal;                                                                   
in vec3 v_position;

struct VSOutput
{
    vec2 TexCoord;
    vec3 Normal;                                                                   
    vec3 WorldPos;                                                                 
};

struct BaseLight
{
    vec3 Color;
    float AmbientIntensity;
    float DiffuseIntensity;
};

struct DirectionalLight
{
    BaseLight Base;
    vec3 Direction;
};
                                                                                    
struct Attenuation                                                                  
{                                                                                   
    float Constant;                                                                 
    float Linear;                                                                   
    float Exp;                                                                      
};                                                                                  
                                                                                    
struct PointLight                                                                           
{                                                                                           
    BaseLight Base;                                                                  
    vec3 Position;                                                                          
    Attenuation Atten;                                                                      
};                                                                                          
                                                                                            
struct SpotLight                                                                            
{                                                                                           
    PointLight Base;                                                                 
    vec3 Direction;                                                                         
    float Cutoff;                                                                           
};                                                                                          
                                                                                            
uniform int gNumPointLights;                                                                
uniform int gNumSpotLights;                                                                 
uniform DirectionalLight gDirectionalLight;                                                 
uniform PointLight gPointLights[MAX_POINT_LIGHTS];                                          
uniform SpotLight gSpotLights[MAX_SPOT_LIGHTS];                                             
uniform sampler2D gColorMap;                                                                
uniform vec3 gEyeWorldPos;                                                                  
uniform float gMatSpecularIntensity;                                                        
uniform float gSpecularPower;
uniform bool has_texture = false;
uniform Material material;
uniform bool skybox_rendering = false;
uniform float transparency;
uniform bool lighting_on = true;
uniform bool fog_on = false;
uniform vec3 fog_colour;
uniform int fog_factor_type;
in vec4 v_pos;
uniform float rho = 0.15f;
uniform float fog_start = 3.0f;
uniform float fog_end = 15.0f;

uniform bool colouring_on = false;
uniform vec3 in_colour = vec3(1,0,0);


vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, VSOutput In)            
{                                                                                           
	vec4 AmbientColor;
	if(has_texture)
		AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    else
		AmbientColor = vec4(Light.Color * Light.AmbientIntensity * material.ambient, 1.0f);
    float DiffuseFactor = dot(In.Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0.0) {
        if(has_texture)
			DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f); 
        else
			DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor * material.diffuse, 1.0f);
        vec3 VertexToEye = normalize(gEyeWorldPos - In.WorldPos);                             
        vec3 LightReflect = normalize(reflect(LightDirection, In.Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                                      
        if (SpecularFactor > 0.0) {                                                         
			SpecularFactor = pow(SpecularFactor, gSpecularPower);
			if(has_texture)
				SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
			else
				SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor * material.specular * material.shininess, 1.0f);
        }                                                                                   
    }                                                                                       
                                                                                            
    return (AmbientColor + DiffuseColor + SpecularColor);                                   
}                                                                                           
                                                                                            
vec4 CalcDirectionalLight(VSOutput In)                                                      
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, In);  
}                                                                                           
                                                                                            
vec4 CalcPointLight(PointLight l, VSOutput In)                                       
{                                                                                           
    vec3 LightDirection = In.WorldPos - l.Position;                                           
    float Distance = length(LightDirection);                                                
    LightDirection = normalize(LightDirection);                                             
                                                                                            
    vec4 Color = CalcLightInternal(l.Base, LightDirection, In);                         
    float Attenuation =  l.Atten.Constant +                                                 
                         l.Atten.Linear * Distance +                                        
                         l.Atten.Exp * Distance * Distance;                                 
                                                                                            
    return Color / Attenuation;                                                             
}                                                                                           
                                                                                            
vec4 CalcSpotLight(SpotLight l, VSOutput In)                                         
{                                                                                           
    vec3 LightToPixel = normalize(In.WorldPos - l.Base.Position);                             
    float SpotFactor = dot(LightToPixel, l.Direction);                                      
                                                                                            
    if (SpotFactor > l.Cutoff) {                                                            
        vec4 Color = CalcPointLight(l.Base, In);                                        
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));                   
    }                                                                                       
    else {                                                                                  
        return vec4(0,0,0,0);                                                               
    }                                                                                       
}

void main()
{                                    
    VSOutput In;
    In.TexCoord = v_tex_coord;
    In.Normal   = normalize(v_normal);
    In.WorldPos = v_position;

	vec4 result;

	if(skybox_rendering || !lighting_on)
	{
	  if(has_texture) {
	      result = texture(gColorMap, In.TexCoord.xy);
        result.w *= transparency;
	  }
	  else
	      result = vec4(material.ambient, material.transparency);

		// uncomment the following to affect the skybox with the light colour
		//result = light.colour * texture(gColorMap, In.TexCoord.xy);
	}
	else
	{
		vec4 TotalLight = CalcDirectionalLight(In);                                         
                                                                                            
		for (int i = 0 ; i < gNumPointLights ; i++) {                                           
			TotalLight += CalcPointLight(gPointLights[i], In);                              
		}                                                                                       
                                                                                            
		for (int i = 0 ; i < gNumSpotLights ; i++) {                                            
			TotalLight += CalcSpotLight(gSpotLights[i], In);                                
		}

		if(has_texture) {
		    result = texture(gColorMap, In.TexCoord.xy) * TotalLight;
		    result.w *= transparency;
		}
		
		else {
			if(material.transparency<1.0)
			    TotalLight.w = material.transparency;
			result = TotalLight;
        }
	}

	if(fog_on)
	{
		float d = length(v_pos.xyz);
		float w;
		if(fog_factor_type == 0) {
			if (d < fog_end)
				w = (fog_end - d) / (fog_end-fog_start);
			else
				w = 0;
		} else if (fog_factor_type == 1) {
			w = exp(-(rho*d));
		} else {
			w = exp(-(rho*d)*(rho*d));
		}
		result.rgb = mix(fog_colour, result.rgb, w);
	}

	if(colouring_on)
	{
		result = result * vec4(in_colour, transparency);
	}
	
    o_color = result;
}
