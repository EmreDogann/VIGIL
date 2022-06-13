#pragma once
#include "sound.h"

namespace engine
{
	/// \brief Short lasting sound, generally paired up with events
	class event_sound : public sound
	{

	public:
		event_sound() = default;
		event_sound(const std::string& name);
		~event_sound() = default;

		bool load(const std::string& file_path, bool loop = false);
		bool play() override;

	};

	/// \brief Short lasting sound, generally paired up with events
	class spatialised_sound : public event_sound
	{

	public:
		spatialised_sound() = default;
		spatialised_sound(const std::string& name);
		~spatialised_sound() = default;

		bool load(const std::string& file_path);
		bool play(glm::vec3 camera_position, glm::vec3 position);

	};
}
