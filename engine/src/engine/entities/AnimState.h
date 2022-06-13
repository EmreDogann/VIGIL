#pragma once
struct AnimState {
	AnimState(int   current_anim_index, float run_time, bool is_playing, float current_anim_length = 0.0f,
	          float anim_scheduler_timer = 0.0f) : m_current_animation_index(current_anim_index),
	                                               m_running_time(run_time),
	                                               m_animation_playing(is_playing),
	                                               m_current_animation_length(current_anim_length), m_anim_scheduler_timer(anim_scheduler_timer) {}

	~AnimState() {};

	int   m_current_animation_index;
	float m_current_animation_length;
	float m_running_time;
	float m_anim_scheduler_timer;
	bool  m_animation_playing;
};
