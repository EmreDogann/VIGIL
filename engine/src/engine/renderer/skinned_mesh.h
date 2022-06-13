/*

    Copyright 2011 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "engine/core.h"
#include "engine/core/timestep.h"
#include "engine/renderer/texture.h"
#include "engine/renderer/shader.h"

#include <map>
#include <vector>
#include "assimp/scene.h"      // Output data structure
#include "assimp/Importer.hpp"
#include "vertex_array.h"

#include "engine/entities/AnimState.h"

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace engine {

	struct Animation;

	class skinned_mesh {
	public:
		skinned_mesh();

		~skinned_mesh();

		void on_update(const timestep& ts);

		void on_render(const glm::mat4& transform, const ref<shader>& meshShader);

		static ref<skinned_mesh> create(const std::string& Filename);

		static ref<skinned_mesh> create(const ref<skinned_mesh> mesh);

		uint32_t NumBones() const {
			return m_NumBones;
		}

		void BoneTransform(float time);

		void switch_animation(uint32_t index) {
			m_current_animation_index = index;
			m_running_time = 0.0f;
		}

		void pause_animation(bool pause) {
			m_AnimationPlaying = !pause;
		}

		AnimState get_state() {
			float anim_length = animations().at(m_current_animation_index)->mDuration / animations().at(m_current_animation_index)->mTicksPerSecond;
			return AnimState(m_current_animation_index, m_running_time, m_AnimationPlaying, anim_length);
		}

		void set_state(AnimState state) {
			m_current_animation_index = state.m_current_animation_index;
			m_running_time = state.m_running_time;
			m_AnimationPlaying = state.m_animation_playing;
		}

		void set_default_animation(uint32_t index) {
			m_default_animation_index = index;
		}

		uint32_t default_animation() {
			return m_default_animation_index;
		}

		void LoadAnimationFile(const std::string& Filename);

		const std::vector<aiAnimation*> animations() {
			return m_pAnimations;
		}

		// switch that denotes whether the root movement is on or not
		void switch_root_movement(const bool& on) {
			m_root_movement_on = on;
		}

		glm::vec3 size() const {
			return m_size;
		}

		glm::vec3 offset() const {
			return m_offset;
		}

		bool is_animation_paused() {
			return !m_AnimationPlaying;
		}

	private:
#define NUM_BONES_PER_VEREX 4
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
		static const uint32_t MAX_BONES = 100;


		struct BoneInfo {
			glm::mat4 BoneOffset;
			glm::mat4 FinalTransformation;

			BoneInfo() {
				BoneOffset = glm::mat4(0.f);
				FinalTransformation = glm::mat4(0.f);
			}
		};

		struct VertexBoneData {
			uint32_t IDs[NUM_BONES_PER_VEREX];
			float    Weights[NUM_BONES_PER_VEREX];

			VertexBoneData() {
				Reset();
			};

			void Reset() {
				ZERO_MEM(IDs);
				ZERO_MEM(Weights);
			}

			void AddBoneData(uint32_t BoneID, float Weight);
		};


		void SetBoneTransform(uint32_t Index, const glm::mat4& Transform);

		bool LoadMesh(const std::string& Filename);

		glm::vec3         InterpolateScale(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::quat         InterpolateRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::vec3         InterpolateTranslation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t          FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t          FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t          FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& NodeName);
		void              ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);
		bool              InitFromScene(const aiScene* pScene, const std::string& Filename);
		void              InitMesh(uint32_t                     MeshIndex,
		                           const aiMesh*                paiMesh,
		                           std::vector<glm::vec3>&      Positions,
		                           std::vector<glm::vec3>&      Normals,
		                           std::vector<glm::vec2>&      TexCoords,
		                           std::vector<VertexBoneData>& Bones,
		                           std::vector<unsigned int>&   Indices);
		void LoadBones(uint32_t MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
		bool InitMaterials(const aiScene* pScene, const std::string& Filename);
		void Clear();

		void AddAnimations(const aiScene* pScene);

		void ExtractRootMovement(const aiAnimation* animation);

		///\brief - compares the values of the vertex that is being loaded with min and max values
		/// if any coordinate is smaller than min or bigger than max, record it as new min/max value
		void min_max_compare(const aiVector3D& point);


#define INVALID_MATERIAL 0xFFFFFFFF

		enum VB_TYPES {
			INDEX_BUFFER,
			POS_VB,
			NORMAL_VB,
			TEXCOORD_VB,
			BONE_VB,
			NUM_VBs
		};

		uint32_t m_VAO;
		uint32_t m_Buffers[NUM_VBs];

		struct MeshEntry {
			MeshEntry() {
				NumIndices = 0;
				BaseVertex = 0;
				BaseIndex = 0;
				MaterialIndex = INVALID_MATERIAL;
			}

			unsigned int NumIndices;
			unsigned int BaseVertex;
			unsigned int BaseIndex;
			unsigned int MaterialIndex;
		};

		std::vector<MeshEntry>       m_Entries;
		std::vector<ref<texture_2d>> m_textures;
		std::vector<glm::mat4>       m_BoneTransforms;

		std::map<std::string, uint32_t> m_BoneMapping; // maps a bone name to its index
		uint32_t                        m_NumBones;
		std::vector<BoneInfo>           m_BoneInfo;
		glm::mat4                       m_GlobalInverseTransform;

		const aiScene*                              m_pScene;
		std::vector<engine::ref<engine::Animation>> m_extra_animations;
		std::vector<aiAnimation*>                   m_pAnimations;
		uint32_t                                    m_current_animation_index;
		uint32_t                                    m_default_animation_index;

		float m_running_time;

		Assimp::Importer m_Importer;

		bool m_AnimationPlaying = true;

		// min values of the model in x, y and z coordinates
		aiVector3D m_min_point;

		// max values of the model in x, y and z coordinates
		aiVector3D m_max_point;

		// switch that checks if the current point is the first point loaded for the model
		bool m_first_point = true;

		// dimensions of the object in x, y and z
		glm::vec3 m_size;

		// offset of the model from the origin of the local coordinates
		glm::vec3 m_offset;

		// switch that denotes whether the root movement is on or not
		bool m_root_movement_on = true;

		// vector of displacements to switch off root movement
		std::vector<std::vector<aiVector3D>> m_root_movements;

		// offset of the model from the origin of the local coordinates due to root movement
		glm::vec3 m_current_root_movement_offset;

		bool m_update_root_offset = true;

	public:
		const Assimp::Importer& importer() {
			return m_Importer;
		}
	};

	struct Animation {
		Assimp::Importer          m_aImporter;
		const aiScene*            m_pScene;
		std::vector<aiAnimation*> m_pAnimations;


		Animation() {
			m_pScene = NULL;
		}

		static ref<Animation> create();
	};

}
