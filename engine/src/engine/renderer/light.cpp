#include "pch.h"
#include "light.h"

void engine::DirectionalLight::submit(const engine::ref<engine::shader> shader)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.Color", Color);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Direction", glm::normalize(Direction));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", DiffuseIntensity);
}

void engine::PointLight::submit(const engine::ref<engine::shader> shader, uint32_t point_light_index)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gPointLights[" + std::to_string(point_light_index) + "].Base.Color", Color);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gPointLights[" + std::to_string(point_light_index) + "].Base.AmbientIntensity", AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gPointLights[" + std::to_string(point_light_index) + "].Position", Position);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gPointLights[" + std::to_string(point_light_index) + "].Base.DiffuseIntensity", DiffuseIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gPointLights[" + std::to_string(point_light_index) + "].Atten.Constant", Attenuation.Constant);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gPointLights[" + std::to_string(point_light_index) + "].Atten.Linear", Attenuation.Linear);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gPointLights[" + std::to_string(point_light_index) + "].Atten.Exp", Attenuation.Exp);
}

void engine::SpotLight::submit(const engine::ref<engine::shader> shader, uint32_t spot_light_index)
{
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Base.Base.Color", Color);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Base.Base.AmbientIntensity", AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Base.Position", Position);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Base.Base.DiffuseIntensity", DiffuseIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Base.Atten.Constant", Attenuation.Constant);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Base.Atten.Linear", Attenuation.Linear);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Base.Atten.Exp", Attenuation.Exp);
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Direction", glm::normalize(Direction));
	std::dynamic_pointer_cast<engine::gl_shader>(shader)->set_uniform("gSpotLights[" + std::to_string(spot_light_index) + "].Cutoff", Cutoff);

}
