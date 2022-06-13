#include "pch.h"
#include "track.h"
#include "audio_manager.h"


engine::track::track(const std::string& name)
{
	m_name = name;
}

bool engine::track::load(const std::string& file_path)
{
	auto result = engine::audio_manager::system()->createStream(file_path.c_str(), FMOD_DEFAULT | FMOD_LOOP_NORMAL, nullptr, &m_sound);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::play()
{
	auto result = engine::audio_manager::system()->playSound(m_sound, NULL, false, &m_channel);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::pause()
{
	auto result = m_channel->setPaused(true);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::unpause()
{
	auto result = m_channel->setPaused(false);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::stop() const
{
	auto result = m_channel->stop();
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::volume(float vol) const
{
	// making sure the volume is between 0 and 1
	vol = glm::clamp(vol, 0.f, 1.f);
	auto result = m_channel->setVolume(vol);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::loop(bool shouldLoop) const
{
	auto result = m_sound->setMode(FMOD_LOOP_OFF + static_cast<int>(shouldLoop));
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::add_dsp_high(FMOD::DSP* dsp_high_pass) const
{
	auto result = m_channel->addDSP(0, dsp_high_pass);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

bool engine::track::add_dsp_low(FMOD::DSP* dsp_low_pass) const
{
	auto result = m_channel->addDSP(0, dsp_low_pass);
	engine::audio_manager::fmod_error_check(result);
	return result == FMOD_OK;
}

