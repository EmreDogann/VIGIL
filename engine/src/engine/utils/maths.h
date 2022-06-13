#pragma once
#include "pch.h"
#include "engine/entities/camera.h"

namespace engine
{
	namespace maths
	{
		static glm::mat4 create_transformation_matrix(
			const glm::vec3 &translation,
			const glm::vec3 &rotation,
			float scale)
		{
			glm::mat4 mat = glm::identity<glm::mat4>();
			mat = glm::translate(mat, translation);
			mat = glm::rotate(mat, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
			mat = glm::rotate(mat, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
			mat = glm::rotate(mat, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
			mat = glm::scale(mat, glm::vec3(scale));
			return mat;
		}

		/// \brief From https://youtu.be/50Y9u7K0PZo?list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&t=626
		static glm::mat4 create_projection_matrix(float fov, float width, float height, float near_plane, float far_plane)
		{
			float aspectRatio = width / height;
			float y_scale = (1.f / tan(glm::radians(fov / 2.f))) * aspectRatio;
			float x_scale = y_scale / aspectRatio;
			float frustum_length = near_plane - far_plane;

			auto mat = glm::identity<glm::mat4>();
			mat[0][0] = x_scale;
			mat[1][1] = y_scale;
			mat[2][2] = -((far_plane + near_plane) / frustum_length);
			mat[2][3] = -1;
			mat[3][2] = -((2 * near_plane * far_plane) / frustum_length);
			mat[3][3] = 0;
			return mat;
		}

		/// \brief Created the view matrix, which is the opposite of the camera position and rotation.
		/// from: https://youtu.be/50Y9u7K0PZo?list=PLRIWtICgwaX0u7Rf9zkZhLoLuZVfUksDP&t=936
		static glm::mat4 create_view_matrix(const camera &camera)
		{
			auto view_mat = glm::identity<glm::mat4>();
			view_mat = glm::rotate(view_mat, glm::radians(camera.pitch()), {1, 0, 0});
			view_mat = glm::rotate(view_mat, glm::radians(camera.yaw()), {0, 1, 0});
			glm::vec3 cam_pos = camera.position();
			glm::vec3 negativeCameraPos = -1.f * cam_pos;
			view_mat = glm::translate(view_mat, negativeCameraPos);
			return view_mat;
		}
	}
}
