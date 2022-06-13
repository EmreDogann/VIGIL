#pragma once
#include "platform/opengl/gl_shader.h"

namespace engine
{

	struct BaseLight
	{
		glm::vec3 Color;
		float AmbientIntensity;
		float DiffuseIntensity;

		BaseLight()
		{
			Color = glm::vec3(0.0f, 0.0f, 0.0f);
			AmbientIntensity = 0.0f;
			DiffuseIntensity = 0.0f;
		}
	};

	struct DirectionalLight : public BaseLight
	{
		glm::vec3 Direction;

		DirectionalLight()
		{
			Direction = glm::vec3(0.0f, 0.0f, 0.0f);
		}

		void submit(const engine::ref<engine::shader> shader);
	};

	struct PointLight : public BaseLight
	{
		glm::vec3 Position;

		struct
		{
			float Constant;
			float Linear;
			float Exp;
		} Attenuation;

		PointLight()
		{
			Position = glm::vec3(0.0f, 0.0f, 0.0f);
			Attenuation.Constant = 1.0f;
			Attenuation.Linear = 0.001f;
			Attenuation.Exp = 0.001f;
		}

		void submit(const engine::ref<engine::shader> shader, uint32_t point_light_index);
	};

	struct SpotLight : public PointLight
	{
		glm::vec3 Direction;
		float Cutoff;

		SpotLight()
		{
			Direction = glm::vec3(0.0f, 0.0f, 0.0f);
			Cutoff = 0.0f;
		}

		void submit(const engine::ref<engine::shader> shader, uint32_t spot_light_index);
	};
}
