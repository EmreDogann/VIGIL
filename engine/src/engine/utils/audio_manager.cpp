#include "pch.h"
#include "audio_manager.h"

#include "track.h"
#include "event_sound.h"

#include <fmod_errors.h>

//#pragma comment(lib, "lib/fmodex_vc.lib")

//=============================================================================

FMOD::System* engine::audio_manager::m_fmod_system{ nullptr };

uint32_t engine::audio_manager::max_channels{ 32 };
uint32_t engine::audio_manager::used_channels{ 0 };

//-----------------------------------------------------------------------------

engine::audio_manager::~audio_manager()
{
	clean_all();
	SAFE_RELEASE(m_fmod_system);
}

//-----------------------------------------------------------------------------

bool engine::audio_manager::init()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_fmod_system);
	fmod_error_check(result);
	if (result != FMOD_OK)
		return false;

	// Initialise the system
	result = m_fmod_system->init(max_channels, FMOD_INIT_NORMAL, nullptr);
	fmod_error_check(result);

	return result == FMOD_OK;
}

void engine::audio_manager::on_update() const
{
	m_fmod_system->update();
}

bool engine::audio_manager::clean_all()
{
	for (auto sound : m_sounds)
	{
		SAFE_RELEASE(sound.second);
	}
	m_sounds.clear();

	result = m_fmod_system->close();
	SAFE_RELEASE(m_fmod_system);

	fmod_error_check(result);
	return result == FMOD_OK;
}

void engine::audio_manager::stop_all()
{
	for (uint32_t i = 0; i < max_channels; i++)
	{
		FMOD::Channel* pChannel = nullptr;
		result = m_fmod_system->getChannel(i, &pChannel);
		fmod_error_check(result);

		if (result == FMOD_OK && pChannel)
		{
			pChannel->stop();
		}
	}
}

void engine::audio_manager::update_with_camera(engine::perspective_camera camera)
{

	FMOD_VECTOR listener_position, listener_velocity, up_vector, view_vector;

	glm::vec3 camera_position = camera.position();
	glm::vec3 dir = camera.front_vector();  // note: viewVector pointing backwards due to right-handed coordinate system
	dir = glm::normalize(dir);
	glm::vec3 up = camera.up_vector();

	listener_position.x = camera_position.x;
	listener_position.y = camera_position.y;
	listener_position.z = camera_position.z;
	listener_velocity.x = 0;
	listener_velocity.y = 0;
	listener_velocity.z = 0;
	up_vector.x = up.x;
	up_vector.y = up.y;
	up_vector.z = up.z;
	view_vector.x = dir.x;
	view_vector.y = dir.y;
	view_vector.z = dir.z;

	// Update the listener position, velocity, and orientation based on the camera
	m_fmod_system->set3DListenerAttributes(0, &listener_position, &listener_velocity, &view_vector, &up_vector);
	m_fmod_system->update();

}

//-----------------------------------------------------------------------------

bool engine::audio_manager::load_sound(const std::string& file_path, const engine::sound_type& type, const std::string& name)
{
	LOG_CORE_INFO("[sound] Creating sound '{0}'.", name);

	bool result = false;
	if (type == engine::sound_type::event)
		result = load_event(file_path, type, name);
	else if (type == engine::sound_type::track)
		result = load_track(file_path, type, name);
	else if (type == engine::sound_type::spatialised)
		result = load_spatialised_sound(file_path, type, name);

	return result;
}

// Play an event sound
void engine::audio_manager::play(const std::string& sound)
{
	if (m_sounds.find(sound) != m_sounds.end())
	{
		if (m_sounds[sound]->type() != sound_type::spatialised)
			m_sounds[sound]->play();
		return;
	}

	LOG_CORE_ERROR("[sound] Could not play '{0}'.", sound);
}

engine::sound* engine::audio_manager::sound(const std::string& sound) const
{
	if (m_sounds.find(sound) != m_sounds.end())
	{
		return m_sounds.at(sound);
	}
	return nullptr;
}

void engine::audio_manager::pause(const std::string& track)
{
	engine::sound* music = audio_manager::sound(track);
	if (music->type() != sound_type::spatialised)
		((engine::track*)music)->pause();
}

void engine::audio_manager::unpause(const std::string& track)
{
	engine::sound* music = audio_manager::sound(track);
	if (music->type() != sound_type::spatialised)
		((engine::track*)music)->unpause();
}

void engine::audio_manager::stop(const std::string& track)
{
	engine::sound* music = audio_manager::sound(track);
	if (music->type() != sound_type::spatialised)
		((engine::track*)music)->stop();
}

void engine::audio_manager::volume(const std::string& track, float volume)
{
	engine::sound* music = audio_manager::sound(track);
	if (music->type() != sound_type::spatialised)
		((engine::track*)music)->volume(volume);
}

void engine::audio_manager::loop(const std::string& track, bool loop)
{
	engine::sound* music = audio_manager::sound(track);
	if (music->type() != sound_type::spatialised)
		((engine::track*)music)->loop(loop);
}

void engine::audio_manager::play_spatialised_sound(const std::string& spatialised_sound, glm::vec3 camera_position, glm::vec3 position)
{
	engine::sound* spatialised = audio_manager::sound(spatialised_sound);
	if (spatialised->type() == sound_type::spatialised)
		((engine::spatialised_sound*)spatialised)->play(camera_position, position);
}


//-----------------------------------------------------------------------------

bool engine::audio_manager::load_event(const std::string& file_path, const engine::sound_type& type, const std::string& name)
{
	auto event = new engine::event_sound(name);
	if (event->load(file_path))
	{
		event->set_type(type);
		m_sounds[name] = event;
		return true;
	}

	LOG_CORE_ERROR("[sound] Could not load event '{0}'.", name);

	return false;
}

bool engine::audio_manager::load_track(const std::string& file_path, const engine::sound_type& type, const std::string& name)
{
	// block track creation when no more channels are available
	if (used_channels >= max_channels)
	{
		LOG_CORE_ERROR("Cannot create a new track. No more channels available.");

		return false;
	}

	auto track = new engine::track(name);
	if (track->load(file_path))
	{
		track->set_type(type);
		m_sounds[name] = track;
		++used_channels;

		LOG_CORE_INFO("[sound] Channels count: '{0}'.", used_channels);

		return true;
	}

	LOG_CORE_ERROR("[sound] Could not load track '{0}'.", name);

	return false;
}

// Load an event sound
bool engine::audio_manager::load_spatialised_sound(const std::string& file_path, const engine::sound_type& type, const std::string name)
{
	auto spatialised = new engine::spatialised_sound(name);
	if (spatialised->load(file_path))
	{
		spatialised->set_type(type);
		m_sounds[name] = spatialised;
		return true;
	}

	LOG_CORE_ERROR("[sound] Could not load event '{0}'.", name);

	return false;
}

//-----------------------------------------------------------------------------

// Check for error
void engine::audio_manager::fmod_error_check(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		const std::string& error_string = FMOD_ErrorString(result);
		LOG_CORE_ERROR("[sound] '{0}'", error_string);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

FMOD::System* engine::audio_manager::system()
{
	return m_fmod_system;
}

engine::ref<engine::audio_manager> engine::audio_manager::instance()
{
	static engine::audio_manager instance;
	return std::make_shared<engine::audio_manager>(instance);
}

uint32_t engine::audio_manager::available_channels()
{
	return max_channels - used_channels;
}

//-----------------------------------------------------------------------------

bool engine::audio_manager::create_high_pass_filter()
{
	auto result = m_fmod_system->createDSPByType(FMOD_DSP_TYPE_HIGHPASS, &m_dsp_high_pass);

	for (auto sound : m_sounds)
	{
		if (sound.second->type() == sound_type::track)
			((engine::track*)sound.second)->add_dsp_high(m_dsp_high_pass);
	}

	return result == FMOD_OK;
}

bool engine::audio_manager::set_high_pass_filter(float freq)
{
	auto result = m_dsp_high_pass->setParameterFloat(FMOD_DSP_HIGHPASS_CUTOFF, freq);
	fmod_error_check(result);
	if (result != FMOD_OK)
		return false;

	return true;

}

bool engine::audio_manager::create_low_pass_filter()
{
	auto result = m_fmod_system->createDSPByType(FMOD_DSP_TYPE_LOWPASS, &m_dsp_low_pass);

	for (auto sound : m_sounds)
	{
		if (sound.second->type() == sound_type::track)
			((engine::track*)sound.second)->add_dsp_low(m_dsp_low_pass);
	}

	return result == FMOD_OK;
}

bool engine::audio_manager::set_low_pass_filter(float freq)
{
	auto result = m_dsp_low_pass->setParameterFloat(FMOD_DSP_LOWPASS_CUTOFF, freq);
	fmod_error_check(result);
	if (result != FMOD_OK)
		return false;

	return true;

}

//=============================================================================
