#pragma once
#include <engine.h>

class AnimationController {
public:
	AnimationController(engine::ref<engine::skinned_mesh>& skinnedMesh);
	~AnimationController();

	static engine::ref<AnimationController> create(engine::ref<engine::skinned_mesh>& skinnedMesh);

	void on_update(const engine::timestep& time_step);
	void queueAnimation(int new_animation);
	void playAnimation(int new_animation);
	void pauseAnimation(bool b_val);
	void loopAnimation(int new_animation);

	bool is_anim_in_proress() {
		return m_anim_in_progress;
	}

	bool is_anim_paused() {
		return m_skinned_mesh->is_animation_paused();
	}

	float current_animation_remaining_time() {
		return m_current_anim_length - m_timer;
	}

	float calculate_animation_length(int anim_index) {
		return m_skinned_mesh->animations().at(anim_index)->mDuration / m_skinned_mesh->animations().at(anim_index)->mTicksPerSecond;
	}

	AnimState save_state() {
		AnimState saved_state = m_skinned_mesh->get_state();
		saved_state.m_anim_scheduler_timer = m_timer;
		return saved_state;
	}

	void set_state(AnimState state) {
		m_skinned_mesh->set_state(state);
		m_current_anim_length = state.m_current_animation_length;
		m_timer = state.m_anim_scheduler_timer;
	}

	void set_skinned_mesh(engine::ref<engine::skinned_mesh>& skinnedMesh) {
		m_skinned_mesh = skinnedMesh;
	}

private:
	float m_timer = 0.0f;
	float m_current_anim_length = 0.0f;
	bool  m_anim_in_progress = false;

	// FIFO queue
	std::queue<int>                   m_anim_queue;
	engine::ref<engine::skinned_mesh> m_skinned_mesh;
};
