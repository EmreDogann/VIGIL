#pragma once
#include "engine/core/timestep.h"
#define GLM_FORCE_CTOR_INIT
#include "glm/glm.hpp"


namespace engine {
	//================= CAMERA INTERFACE =================
	class camera {
	public:
		enum e_rotation {
			clock_wise = 0,
			anticlock_wise
		};

		enum e_axis {
			x = 0,
			y,
			z
		};

	public:
		virtual ~camera() = default;

		virtual void on_update(const timestep& timestep) = 0;

		virtual const glm::mat4& projection_matrix() const = 0;
		virtual const glm::mat4& view_matrix() const = 0;
		virtual const glm::mat4& view_projection_matrix() const = 0;
		virtual float            movement_speed() const = 0;
		virtual float            rotation_speed() const = 0;
		virtual void             set_movement_speed(float move_speed) = 0;
		virtual void             set_rotation_speed(float rot_speed) = 0;

		virtual void      position(const glm::vec3& pos) = 0;
		virtual glm::vec3 position() const = 0;
	};

	//================= 2D CAMERA =================

	class orthographic_camera final : public camera {
	public:
		enum e_direction {
			up = 0,
			down,
			left,
			right
		};

	public:
		orthographic_camera(float left, float right, float bottom, float top);

		void on_update(const timestep& timestep) override {};

		glm::vec3 position() const override {
			return m_position;
		}

		void position(const glm::vec3& pos) override {
			m_position = pos;
			update_view_matrix();
		}

		float rotation() const {
			return m_rotation;
		}

		void rotation(float rot) {
			m_rotation = rot;
			update_view_matrix();
		}

		const glm::mat4& projection_matrix() const override {
			return m_projection_mat;
		}

		const glm::mat4& view_matrix() const override {
			return m_view_mat;
		}

		const glm::mat4& view_projection_matrix() const override {
			return m_view_projection_mat;
		}

		float movement_speed() const override {
			return s_movement_speed;
		}

		float rotation_speed() const override {
			return s_rotation_speed;
		}

		void set_movement_speed(float move_speed) override {
			s_movement_speed = move_speed;
		}

		void set_rotation_speed(float rot_speed) override {
			s_rotation_speed = rot_speed;
		}

	private:
		void move(e_direction direction, timestep ts);
		void rotate(e_rotation rotation, e_axis rotation_axis, timestep ts);
		void update_view_matrix();

	private:
		glm::mat4 m_projection_mat{1};
		glm::mat4 m_view_mat{1};
		glm::mat4 m_view_projection_mat{1};

		glm::vec3 m_position{0};
		float     m_rotation{0};

		/// \bief Movement speed in units per second
		inline static float s_movement_speed = 1.0f;
		/// \brief Rotation speed in degrees per second
		inline static float s_rotation_speed = 180.0f;
	};

	//================= 3D CAMERA =================

	// Default camera values
	const float YAW = -90.0f;
	const float PITCH = 0.0f;
	const float SPEED = 2.5f;
	const float SENSITIVITY = 0.1f;
	const float ZOOM = 45.0f;

	class perspective_camera : public camera {
	public:
		enum e_direction {
			forward = 0,
			backward,
			left,
			right
		};

	public:
		perspective_camera(
			float width, float height,
			float fov = 45.f,
			float near_z = 0.1f, float far_z = 100.f);

		void on_update(const timestep& timestep) override;

		glm::vec3 position() const override {
			return m_position;
		}

		void position(const glm::vec3& pos) override {
			m_position = pos;
			update_view_matrix();
		}

		float movement_speed() const override {
			return s_movement_speed;
		}

		float rotation_speed() const override {
			return s_rotation_speed;
		}

		void set_movement_speed(float move_speed) override {
			s_movement_speed = move_speed;
		}

		void set_rotation_speed(float rot_speed) override {
			s_rotation_speed = rot_speed;
		}

		glm::vec3 front_vector() const {
			return m_front_vector;
		}

		glm::vec3 up_vector() const {
			return m_up_vector;
		}

		glm::vec3 right_vector() const {
			return m_right_vector;
		}

		const glm::mat4& projection_matrix() const override;
		const glm::mat4& view_matrix() const override;
		const glm::mat4& view_projection_matrix() const override;

		void set_view_matrix(glm::vec3 position, glm::vec3 look_at);

	private:
		void process_mouse(float mouse_delta_x, float mouse_delta_y, bool constrain_pitch = true);
		void move(e_direction direction, timestep ts);
		void rotate(e_rotation rotation, e_axis rotation_axis, timestep ts);
		void update_camera_vectors();

	protected:
		void update_view_matrix();

	private:
		glm::mat4 m_projection_mat{1};
		glm::mat4 m_view_mat{1};
		glm::mat4 m_view_projection_mat{1};

		glm::vec3 m_position{0.f};
		/// \brief rotation angles for each axis in degrees. 
		glm::vec3 m_rotation_angle{0.f};

		glm::vec3 m_front_vector{0.f, 0.f, 1.f};
		glm::vec3 m_up_vector{0.f, 1.f, 0.f};
		glm::vec3 m_right_vector{1.f, 0.f, 0.f};
		glm::vec3 m_world_up_vector{0.f, 1.f, 0.f};

		/// \brief 
		float m_yaw = YAW;
		/// \brief 
		float m_pitch = PITCH;
		/// \brief 
		float m_aspect_ratio = 1.f;
		/// \brief Field of view in degrees. 
		float m_fov = ZOOM;
		/// \brief Near clipping plane. 
		float m_near_plane = 0.1f;
		/// \brief ar clipping plane. 
		float m_far_plane = 100.f;

		/// \brief in units per seconds. 
		inline static float s_movement_speed = SPEED;
		/// \brief in degrees per second. 
		inline static float s_rotation_speed = 90.f;
		/// \brief in degrees per second. 
		inline static const float s_mouse_sensitivity = SENSITIVITY;
	};

}
