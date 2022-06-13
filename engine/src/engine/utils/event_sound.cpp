#include "pch.h"
#include "event_sound.h"
#include "audio_manager.h"


//=============================================================================

engine::event_sound::event_sound(const std::string & name)
{
	m_name = name;
}

bool engine::event_sound::load(const std::string& filePath, bool loop /*= false*/)
{
	auto result = engine::audio_manager::system()->createSound(filePath.c_str(), FMOD_DEFAULT | (FMOD_LOOP_OFF + loop), nullptr, &m_sound);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::event_sound::play()
{
	auto result = engine::audio_manager::system()->playSound(m_sound, NULL, false, &m_channel);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

//=============================================================================

engine::spatialised_sound::spatialised_sound(const std::string& name) : event_sound(name)
{
	m_name = name;
}

bool engine::spatialised_sound::load(const std::string& filePath)
{
	auto result = engine::audio_manager::system()->createSound(filePath.c_str(), FMOD_DEFAULT | FMOD_3D, 0, &m_sound);
	engine::audio_manager::fmod_error_check(result);
	if (result != FMOD_OK)
		return false;

	result = engine::audio_manager::system()->set3DSettings(1.0, 0.5, 1.0);

	return result == FMOD_OK;
}

bool engine::spatialised_sound::play(glm::vec3 camera_position, glm::vec3 position)
{
	// Set position and velocity of listener based on the camera
	FMOD_VECTOR pos, vel;
	pos.x = position.x;
	pos.y = position.y;
	pos.z = position.z;
	vel.x = 0.0f;
	vel.y = 0.0f;
	vel.z = 0.0f;

	FMOD_VECTOR listenerPosition;
	listenerPosition.x = camera_position.x;
	listenerPosition.y = camera_position.y;
	listenerPosition.z = camera_position.z;

	// Play the sound
	float volume = 1.0f;
	auto result = engine::audio_manager::system()->playSound(m_sound, NULL, false, &m_channel);
	result = m_channel->setVolume(1.0);
	m_channel->set3DMinMaxDistance(1.0, 50.0);
	m_channel->set3DAttributes(&pos, &vel);	 // The the 3D position of the sound


	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}
