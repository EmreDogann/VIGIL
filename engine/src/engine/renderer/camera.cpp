#include "pch.h"
#include "camera.h"
#include "glm/gtc/matrix_transform.hpp"
#include "engine/core/input.h"
#include "engine/key_codes.h"


//================== Orthographic Camera [2D] =================================

engine::orthographic_camera::orthographic_camera(float left, float right, float bottom, float top) :
	m_projection_mat(glm::ortho(left, right, bottom, top, -5.f, 5.f)) {
	m_view_projection_mat = m_projection_mat * m_view_mat;
}

void engine::orthographic_camera::move(e_direction direction, timestep ts) {
	float speed = s_movement_speed;

	if (direction == up)
		m_position.y += speed * ts;
	else if (direction == down)
		m_position.y -= speed * ts;

	if (direction == left)
		m_position.x -= speed * ts;
	else if (direction == right)
		m_position.x += speed * ts;

	update_view_matrix();
}

void engine::orthographic_camera::rotate(e_rotation rotation, e_axis, timestep ts) {
	float speed = s_rotation_speed;

	if (rotation == clock_wise) {
		m_rotation -= speed * ts;
	} else if (rotation == anticlock_wise) {
		m_rotation += speed * ts;
	}

	update_view_matrix();
}

void engine::orthographic_camera::update_view_matrix() {
	glm::mat4 transform(1);
	glm::rotate(transform, glm::radians(m_rotation), glm::vec3(0, 0, 1));
	transform = glm::translate(transform, m_position);

	// inverting the transform matrix 
	m_view_mat = glm::inverse(transform);
	m_view_projection_mat = m_projection_mat * m_view_mat;
}

//================== Perspective Camera [3D] ==================================

engine::perspective_camera::perspective_camera(
	float width, float height,
	float fov /*= 45.f*/,
	float near_z /*= 0.1f*/, float far_z /*= 100.f*/) :
	m_projection_mat(glm::perspective(glm::radians(fov), width / height, near_z, far_z)),
	m_aspect_ratio(width / height),
	m_fov(fov),
	m_near_plane(near_z),
	m_far_plane(far_z) {
	m_position = glm::vec3(0.0f, 1.0f, 0.0f);
	m_front_vector = glm::vec3(0.0f, 0.0f, -1.0f);
	m_up_vector = glm::vec3(0.0f, 1.0f, 0.0f);
	m_view_mat = glm::lookAt(m_position, m_position + m_front_vector, m_up_vector);

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//m_front_vector = glm::normalize(m_position - cameraTarget);

	//glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
	//m_righ_vector = glm::normalize(glm::cross(up, m_front_vector));

	//m_up_vector = glm::cross(m_front_vector, m_righ_vector);
	// note that we're translating the scene in the reverse direction of where we want to move 
	//m_view_mat = glm::translate(m_view_mat, start_position);
	m_view_projection_mat = m_projection_mat * m_view_mat;
	LOG_CORE_TRACE("3d cam position: [{},{},{}]", m_position.x, m_position.y, m_position.z);
	LOG_CORE_TRACE("3d cam rotation: [{},{},{}]", m_rotation_angle.x, m_rotation_angle.y, m_rotation_angle.z);
}

void engine::perspective_camera::on_update(const timestep& timestep) {
	auto [mouse_delta_x, mouse_delta_y] = input::mouse_position();
	process_mouse(mouse_delta_x, mouse_delta_y);
	//update_rail(timestep);

	update_camera_vectors();

	float y_value = m_position.y;
	if (input::key_pressed(engine::key_codes::KEY_J)) // left
		move(e_direction::left, timestep);
	else if (input::key_pressed(engine::key_codes::KEY_L)) // right
		move(e_direction::right, timestep);

	if (input::key_pressed(engine::key_codes::KEY_K)) // down
		move(e_direction::backward, timestep);
	else if (engine::input::key_pressed(engine::key_codes::KEY_I)) // up
		move(e_direction::forward, timestep);

	//m_position.y = y_value;

	//float delta = input::mouse_scroll();
	//process_mouse_scroll(delta);
}

const glm::mat4& engine::perspective_camera::projection_matrix() const {
	return m_projection_mat;
}

const glm::mat4& engine::perspective_camera::view_matrix() const {
	return m_view_mat;
}

const glm::mat4& engine::perspective_camera::view_projection_matrix() const {
	return m_view_projection_mat;
}

void engine::perspective_camera::process_mouse(float mouse_delta_x, float mouse_delta_y, bool constrain_pitch /*= true*/) {
	// Smoothen the values
	mouse_delta_x *= s_mouse_sensitivity;
	mouse_delta_y *= s_mouse_sensitivity;

	m_yaw += mouse_delta_x;
	m_pitch += mouse_delta_y;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrain_pitch) {
		static const float pitch_limit = 89.0f;
		if (m_pitch > pitch_limit)
			m_pitch = pitch_limit;
		if (m_pitch < -pitch_limit)
			m_pitch = -pitch_limit;
	}
}

void engine::perspective_camera::move(e_direction direction, timestep ts) {
	if (direction == forward)
		m_position += s_movement_speed * ts * m_front_vector;
	else if (direction == backward)
		m_position -= s_movement_speed * ts * m_front_vector;

	if (direction == left)
		m_position -= s_movement_speed * ts * m_right_vector;
	else if (direction == right)
		m_position += s_movement_speed * ts * m_right_vector;

	//LOG_CORE_TRACE("3d cam position: [{},{},{}]", m_position.x, m_position.y, m_position.z); 
}

void engine::perspective_camera::rotate(e_rotation rotation, e_axis rotation_axis, timestep ts) {
	float* angle_ref = nullptr;
	switch (rotation_axis) {
	case x: angle_ref = &m_rotation_angle.x;
		break;
	case y: angle_ref = &m_rotation_angle.y;
		break;
	case z: angle_ref = &m_rotation_angle.z;
		break;
	}

	ENGINE_ASSERT(angle_ref, "angle ptr is null!")

	float& angle = *angle_ref;
	if (rotation == anticlock_wise) {
		angle += s_rotation_speed * ts;
		if (angle > 360.f) {
			angle = 0.f;
			LOG_CORE_TRACE("A");
		}
	} else if (rotation == clock_wise) {
		angle -= s_rotation_speed * ts;
		if (angle < 0.f) {
			angle = 360.f;
			LOG_CORE_TRACE("B");
		}
	}

	//LOG_CORE_TRACE("after - 3d cam rotation: [{},{},{}]", m_rotation_angle.x, m_rotation_angle.y, m_rotation_angle.z); 
}

void engine::perspective_camera::update_view_matrix() {
	//glm::mat4 transform{1}; 
	//transform = glm::rotate(transform, glm::radians(m_rotation_angle.x), {1,0,0}); 
	//transform = glm::rotate(transform, glm::radians(m_rotation_angle.y), {0,1,0}); 
	//transform = glm::rotate(transform, glm::radians(m_rotation_angle.z), {0,0,1}); 
	//transform = glm::translate(transform, m_position); 

	// inverting the transform matrix  
	//m_view_mat = glm::inverse(transform); 
	m_view_mat = glm::lookAt(m_position, m_position + m_front_vector, m_up_vector);
	m_view_projection_mat = m_projection_mat * m_view_mat;
}

void engine::perspective_camera::set_view_matrix(glm::vec3 position, glm::vec3 look_at) {
	m_position = position;
	m_front_vector = glm::normalize(look_at - position);
	m_view_mat = glm::lookAt(m_position, m_position + m_front_vector, m_up_vector);
	m_view_projection_mat = m_projection_mat * m_view_mat;
}

void engine::perspective_camera::update_camera_vectors() {
	// Calculate the new Front vector
	glm::vec3 front(0.f);
	float     yaw_radians = glm::radians(m_yaw);
	float     pitch_radians = glm::radians(m_pitch);
	front.x = cos(yaw_radians) * cos(pitch_radians);
	front.y = sin(pitch_radians);
	front.z = sin(yaw_radians) * cos(pitch_radians);

	m_front_vector = glm::normalize(front);
	// Also re-calculate the Right and Up vector
	// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_right_vector = glm::normalize(glm::cross(m_front_vector, m_world_up_vector));
	m_up_vector = glm::normalize(glm::cross(m_right_vector, m_front_vector));
	update_view_matrix();
}
