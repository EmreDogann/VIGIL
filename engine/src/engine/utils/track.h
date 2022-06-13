#pragma once
#include "sound.h"

namespace engine
{
	/// \brief Long sound usally played in the background in loop
	class track : public sound
	{
	public:
		track() = default;
		track(const std::string& name);
		~track() = default;

		bool load(const std::string& file_path);
		bool play() override;
		bool pause();
		bool unpause();
		bool stop() const;
		bool volume(float vol) const;
		bool loop(bool should_loop) const;
		bool add_dsp_high(FMOD::DSP* dsp_high_pass) const;
		bool add_dsp_low(FMOD::DSP* dsp_low_pass) const;
	};
}

