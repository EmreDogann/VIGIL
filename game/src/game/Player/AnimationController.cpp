#include "AnimationController.h"

AnimationController::AnimationController(engine::ref<engine::skinned_mesh>& skinnedMesh) : m_skinned_mesh(skinnedMesh) {};

AnimationController::~AnimationController() {};

engine::ref<AnimationController> AnimationController::create(engine::ref<engine::skinned_mesh>& skinnedMesh) {
	return std::make_shared<AnimationController>(skinnedMesh);
};

void AnimationController::on_update(const engine::timestep& time_step) {
	if (!m_skinned_mesh->is_animation_paused()) m_timer += time_step;
	if (m_timer >= m_current_anim_length) {
		if (!m_anim_queue.empty()) {
			m_anim_queue.pop();
		}

		if (m_anim_queue.size() > 0) {
			playAnimation(m_anim_queue.back());
		} else {
			m_anim_in_progress = false;
			playAnimation(m_skinned_mesh->default_animation());
		}
	}

	m_skinned_mesh->on_update(time_step);
}

void AnimationController::queueAnimation(int new_animation) {
	m_anim_queue.push(new_animation);
	if (m_anim_queue.size() - 1 == 0) {
		m_anim_in_progress = true;

		playAnimation(new_animation);
	}
}

void AnimationController::playAnimation(int new_animation) {
	m_timer = 0;
	// Compute the length of the current animation. Number of ticks in animation / Ticks per second.
	m_current_anim_length = m_skinned_mesh->animations().at(new_animation)->mDuration / m_skinned_mesh->animations().at(new_animation)->mTicksPerSecond;
	m_skinned_mesh->switch_animation(new_animation);
}

void AnimationController::pauseAnimation(bool b_val) {
	m_skinned_mesh->pause_animation(b_val);
}

void AnimationController::loopAnimation(int new_animation) {
	m_skinned_mesh->set_default_animation(new_animation);
};
