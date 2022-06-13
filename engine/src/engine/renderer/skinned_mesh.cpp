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


#include "pch.h"
#include "engine/core.h"
#include "skinned_mesh.h"

#include <glad/glad.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>
#include "engine/utils/assimp_extensions.h"
#include "renderer.h"
#include "platform/opengl/gl_shader.h"

#define POSITION_LOCATION    0
#define NORMAL_LOCATION      1
#define TEX_COORD_LOCATION   2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4

void engine::skinned_mesh::VertexBoneData::AddBoneData(uint32_t BoneID, float Weight) {
	for (uint32_t i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}

	}

	// if more than 4 bones per vertex, keep the 4 biggest weight values and discard the rest
	uint32_t smallest_index = 4;
	float    smallest_weight = Weight;
	for (uint32_t i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) {
		if (Weights[i] < smallest_weight) {
			smallest_weight = Weights[i];
			smallest_index = i;
		}
	}

	if (smallest_index < 4) {
		IDs[smallest_index] = BoneID;
		Weights[smallest_index] = Weight;
		return;
	}

	return;

	// should never get here - more bones than we have space for
	assert(0);
}

engine::skinned_mesh::skinned_mesh() {
	m_VAO = 0;
	ZERO_MEM(m_Buffers);
	m_NumBones = 0;
	m_current_animation_index = 0;
	m_default_animation_index = 0;
	m_running_time = 0.f;
}


engine::skinned_mesh::~skinned_mesh() {
	Clear();
}


void engine::skinned_mesh::Clear() {
	if (m_Buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}


bool engine::skinned_mesh::LoadMesh(const std::string& Filename) {
	// Release the previously loaded mesh (if it exists)
	Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Create the buffers for the vertices attributes
	glGenBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);

	bool Ret = false;

	m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	//m_pScene = m_Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenUVCoords | aiProcess_CalcTangentSpace);

	if (&m_pScene) {
		m_GlobalInverseTransform = glm::inverse(Assimp::ToGlm(m_pScene->mRootNode->mTransformation));
		Ret = InitFromScene(m_pScene, Filename);
	} else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), m_Importer.GetErrorString());
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	aiVector3D min_and_max = m_max_point + m_min_point;
	m_offset = glm::vec3(min_and_max.x, min_and_max.y, min_and_max.z) / 2.0f;
	aiVector3D size = m_max_point - m_min_point;
	m_size = glm::vec3(size.x, size.y, size.z);

	return Ret;
}


bool engine::skinned_mesh::InitFromScene(const aiScene* pScene, const std::string& Filename) {
	AddAnimations(pScene);
	m_Entries.resize(pScene->mNumMeshes);
	m_textures.resize(pScene->mNumMaterials);

	std::vector<glm::vec3>      Positions;
	std::vector<glm::vec3>      Normals;
	std::vector<glm::vec2>      TexCoords;
	std::vector<VertexBoneData> Bones;
	std::vector<uint32_t>       Indices;

	uint32_t NumVertices = 0;
	uint32_t NumIndices = 0;

	// Count the number of vertices and indices
	for (uint32_t i = 0; i < m_Entries.size(); i++) {
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);

	// Initialize the meshes in the scene one by one
	for (uint32_t i = 0; i < m_Entries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
	}

	if (!InitMaterials(pScene, Filename)) {
		return false;
	}

	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return glGetError() == GL_NO_ERROR;
}


void engine::skinned_mesh::InitMesh(uint32_t                     MeshIndex,
                                    const aiMesh*                paiMesh,
                                    std::vector<glm::vec3>&      Positions,
                                    std::vector<glm::vec3>&      Normals,
                                    std::vector<glm::vec2>&      TexCoords,
                                    std::vector<VertexBoneData>& Bones,
                                    std::vector<uint32_t>&       Indices) {
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (uint32_t i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		if (m_first_point) {
			m_min_point = *pPos;
			m_max_point = *pPos;
			m_first_point = false;
		} else {
			min_max_compare(*pPos);
		}

		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, 1.f - pTexCoord->y));
	}

	LoadBones(MeshIndex, paiMesh, Bones);

	// Populate the index buffer
	for (uint32_t i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}


void engine::skinned_mesh::LoadBones(uint32_t MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones) {
	for (uint32_t i = 0; i < pMesh->mNumBones; i++) {
		uint32_t    BoneIndex = 0;
		std::string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = Assimp::ToGlm(pMesh->mBones[i]->mOffsetMatrix);
			m_BoneMapping[BoneName] = BoneIndex;
		} else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (uint32_t j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint32_t VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float    Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}


bool engine::skinned_mesh::InitMaterials(const aiScene* pScene, const std::string& Filename) {
	// Extract the directory part from the file name
	std::string::size_type SlashIndex = Filename.find_last_of("/");
	std::string            Dir;

	if (SlashIndex == std::string::npos) {
		Dir = ".";
	} else if (SlashIndex == 0) {
		Dir = "/";
	} else {
		Dir = Filename.substr(0, SlashIndex);
	}

	bool Ret = true;


	// Initialize the materials
	for (uint32_t i = 0; i < pScene->mNumMaterials; i++) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];

		bool hasTexture = false;
		m_textures[i] = NULL;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string p(Path.data);
				uint32_t    count = 0;

				while ((p.size() - 1 - count > 0) && (p[p.size() - 1 - count] != '.'))
					count++;

				if (p.size() > (int)(count + 1))
					hasTexture = true;

				std::string extension = p.substr((int)p.size() - count, count);
				if (extension != "jpg" && extension != "png" && extension != "bmp" && extension != "tga" &&
					extension != "gif" && extension != "hdr" && extension != "psd" && extension != "pic" &&
					extension != "ppm" && extension != "pgm")
					p = p.substr(0, (int)p.size() - count) + "png";


				if (p.substr(0, 2) == ".\\") {
					p = p.substr(2, p.size() - 2);
				}

				std::string FullPath = Dir + "/" + p;

				engine::ref<engine::texture_2d> tex = texture_2d::create(FullPath.c_str(), false);

				if (!tex->loaded()) {
					count = 0;

					while ((count < p.size()) && (p[p.size() - 1 - count] != '/' && p[p.size() - 1 - count] != '\\' || count == 0))
						count++;
					if (count == p.size())
						count = (uint32_t)p.size();

					p = p.substr((int)p.size() - count, count);

					FullPath = Dir + "/" + p;
					tex = texture_2d::create(FullPath.c_str(), false);
				}

				m_textures[i] = tex;
			}
		}

		if (!hasTexture) {
			aiColor3D color(0.0f, 0.0f, 0.0f);
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);

			BYTE data[3];
			data[0] = (BYTE)(color[0] * 255);
			data[1] = (BYTE)(color[1] * 255);
			data[2] = (BYTE)(color[2] * 255);
			m_textures[i] = engine::texture_2d::create(data[2], data[1], data[0]);
		}
	}

	return Ret;
}


void engine::skinned_mesh::on_render(const glm::mat4& transform /*= glm::mat4(1.f)*/, const ref<shader>& meshShader) {
	glBindVertexArray(m_VAO);


	glm::mat4 new_transform = transform;
	new_transform = glm::scale(new_transform, glm::vec3(0.01f));
	if (!m_root_movement_on)
		new_transform = glm::translate(new_transform, m_current_root_movement_offset);

	std::static_pointer_cast<gl_shader>(meshShader)->set_uniform("u_transform", new_transform);
	std::static_pointer_cast<gl_shader>(meshShader)->set_uniform("num_bones", (int)m_BoneTransforms.size());
	for (size_t i = 0; i < m_BoneTransforms.size(); i++) {
		std::string uniformName = std::string("gBones[") + std::to_string(i) + std::string("]");
		std::static_pointer_cast<gl_shader>(meshShader)->set_uniform(uniformName, m_BoneTransforms[i]);
	}

	for (uint32_t i = 0; i < m_Entries.size(); i++) {
		const uint32_t MaterialIndex = m_Entries[i].MaterialIndex;

		assert(MaterialIndex < m_textures.size());

		if (m_textures[MaterialIndex]) {
			m_textures[MaterialIndex]->bind();
		}

		glDrawElementsBaseVertex(GL_TRIANGLES,
		                         m_Entries[i].NumIndices,
		                         GL_UNSIGNED_INT,
		                         (void*)(sizeof(uint32_t) * m_Entries[i].BaseIndex),
		                         m_Entries[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);

	m_update_root_offset = true;
}


void engine::skinned_mesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform) {
	std::string        name(pNode->mName.data);
	const aiAnimation* animation = m_pAnimations[m_current_animation_index];
	glm::mat4          nodeTransform(Assimp::ToGlm(pNode->mTransformation));
	const aiNodeAnim*  nodeAnim = FindNodeAnim(animation, name);

	if (nodeAnim) {
		glm::vec3 translation = InterpolateTranslation(AnimationTime, nodeAnim);
		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

		glm::quat rotation = InterpolateRotation(AnimationTime, nodeAnim);
		glm::mat4 rotationMatrix = glm::toMat4(rotation);

		glm::vec3 scale = InterpolateScale(AnimationTime, nodeAnim);
		glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

		nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
	}

	glm::mat4 transform = ParentTransform * nodeTransform;

	if (m_BoneMapping.find(name) != m_BoneMapping.end()) {
		uint32_t BoneIndex = m_BoneMapping[name];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * transform * m_BoneInfo[BoneIndex].BoneOffset;
	}

	for (uint32_t i = 0; i < pNode->mNumChildren; i++)
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], transform);
}

void engine::skinned_mesh::on_update(const timestep& ts) {
	if (m_AnimationPlaying) m_running_time += ts;
	if (m_pAnimations.size() != 0) {
		if (m_pAnimations[m_current_animation_index]) {
			float TicksPerSecond = (float)(
				m_pAnimations[m_current_animation_index]->mTicksPerSecond != 0 ? m_pAnimations[m_current_animation_index]->mTicksPerSecond : 25.0f); // *m_TimeMultiplier;
			float TimeInTicks = m_running_time * TicksPerSecond;
			float AnimationTime = fmod(TimeInTicks, (float)m_pAnimations[m_current_animation_index]->mDuration);

			BoneTransform(AnimationTime);
		}
	}
}

void engine::skinned_mesh::BoneTransform(float time) {
	glm::mat4 Identity = glm::mat4(1.f);

	ReadNodeHeirarchy(time, m_pScene->mRootNode, Identity);

	m_BoneTransforms.resize(m_NumBones);

	for (uint32_t i = 0; i < m_NumBones; i++) {
		m_BoneTransforms[i] = m_BoneInfo[i].FinalTransformation;
	}
}

const aiNodeAnim* engine::skinned_mesh::FindNodeAnim(const aiAnimation* animation, const std::string& nodeName) {
	for (uint32_t i = 0; i < animation->mNumChannels; i++) {
		const aiNodeAnim* nodeAnim = animation->mChannels[i];
		if (std::string(nodeAnim->mNodeName.data) == nodeName)
			return nodeAnim;
	}
	return nullptr;
}

uint32_t engine::skinned_mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
			return i;
	}

	return 0;
}

uint32_t engine::skinned_mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	CORE_ASSERT(pNodeAnim->mNumRotationKeys > 0);

	for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
			return i;
	}

	return 0;
}

uint32_t engine::skinned_mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim) {
	CORE_ASSERT(pNodeAnim->mNumScalingKeys > 0);

	for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
			return i;
	}

	return 0;
}

glm::vec3 engine::skinned_mesh::InterpolateTranslation(float animationTime, const aiNodeAnim* nodeAnim) {
	if (nodeAnim->mNumPositionKeys == 1) {
		// No interpolation necessary for single value
		auto v = nodeAnim->mPositionKeys[0].mValue;
		return {v.x, v.y, v.z};
	}

	uint32_t PositionIndex = FindPosition(animationTime, nodeAnim);
	uint32_t NextPositionIndex = (PositionIndex + 1);
	CORE_ASSERT(NextPositionIndex < nodeAnim->mNumPositionKeys);
	ai_real DeltaTime = (float)(nodeAnim->mPositionKeys[NextPositionIndex].mTime - nodeAnim->mPositionKeys[PositionIndex].mTime);
	ai_real Factor = (animationTime - nodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	if (Factor < 0.0f)
		Factor = 0.0f;
	CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
	const aiVector3D& Start = nodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = nodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D        Delta = End - Start;
	aiVector3D        v_factor = {Factor, Factor, Factor};
	aiVector3D        aiVec = Start + Factor * Delta;

	if (m_update_root_offset) {
		const aiVector3D& OffsetStart = m_root_movements.at(m_current_animation_index).at(PositionIndex);
		const aiVector3D& OffsetEnd = m_root_movements.at(m_current_animation_index).at(NextPositionIndex);
		aiVector3D        OffsetDelta = OffsetEnd - OffsetStart;
		aiVector3D        offset = OffsetStart + Factor * OffsetDelta;
		m_current_root_movement_offset = glm::vec3(offset.x, offset.y, offset.z);
		m_update_root_offset = false;
	}

	return {aiVec.x, aiVec.y, aiVec.z};
}

glm::quat engine::skinned_mesh::InterpolateRotation(float animationTime, const aiNodeAnim* nodeAnim) {
	if (nodeAnim->mNumRotationKeys == 1) {
		// No interpolation necessary for single value
		auto v = nodeAnim->mRotationKeys[0].mValue;
		return glm::quat((float)v.w, (float)v.x, (float)v.y, (float)v.z);
	}

	uint32_t RotationIndex = FindRotation(animationTime, nodeAnim);
	uint32_t NextRotationIndex = (RotationIndex + 1);
	CORE_ASSERT(NextRotationIndex < nodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(nodeAnim->mRotationKeys[NextRotationIndex].mTime - nodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (animationTime - (float)nodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if (Factor < 0.0f)
		Factor = 0.0f;
	CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");
	const aiQuaternion& StartRotationQ = nodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = nodeAnim->mRotationKeys[NextRotationIndex].mValue;
	auto                q = aiQuaternion();
	aiQuaternion::Interpolate(q, StartRotationQ, EndRotationQ, Factor);
	q = q.Normalize();
	return glm::quat((float)q.w, (float)q.x, (float)q.y, (float)q.z);
}

glm::vec3 engine::skinned_mesh::InterpolateScale(float animationTime, const aiNodeAnim* nodeAnim) {
	if (nodeAnim->mNumScalingKeys == 1) {
		// No interpolation necessary for single value
		auto v = nodeAnim->mScalingKeys[0].mValue;
		return {v.x, v.y, v.z};
	}

	uint32_t index = FindScaling(animationTime, nodeAnim);
	uint32_t nextIndex = (index + 1);
	CORE_ASSERT(nextIndex < nodeAnim->mNumScalingKeys);
	ai_real deltaTime = (nodeAnim->mScalingKeys[nextIndex].mTime - nodeAnim->mScalingKeys[index].mTime);
	ai_real factor = (animationTime - nodeAnim->mScalingKeys[index].mTime) / deltaTime;
	if (factor < 0.0f)
		factor = 0.0f;
	CORE_ASSERT(factor <= 1.0f, "Factor must be below 1.0f");
	const auto& start = nodeAnim->mScalingKeys[index].mValue;
	const auto& end = nodeAnim->mScalingKeys[nextIndex].mValue;
	auto        delta = end - start;
	auto        aiVec = start + factor * delta;
	return {aiVec.x, aiVec.y, aiVec.z};
}

void engine::skinned_mesh::AddAnimations(const aiScene* pScene) {
	uint32_t numAnimations = pScene->mNumAnimations;
	for (uint32_t i = 0; i != numAnimations; i++) {
		if (pScene->mAnimations[i]) {
			ExtractRootMovement(pScene->mAnimations[i]);
			m_pAnimations.push_back(pScene->mAnimations[i]);
		}
	}
}

void engine::skinned_mesh::LoadAnimationFile(const std::string& Filename) {
	engine::ref<engine::Animation> animation = engine::Animation::create();
	animation->m_pScene = animation->m_aImporter.ReadFile(
		Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if (animation->m_pScene) {
		AddAnimations(animation->m_pScene);
	} else {
		printf("Error parsing '%s': '%s'\n", Filename.c_str(), animation->m_aImporter.GetErrorString());
	}
	m_extra_animations.push_back(animation);
}


engine::ref<engine::skinned_mesh> engine::skinned_mesh::create(const std::string& Filename) {
	engine::ref<engine::skinned_mesh> mesh = std::make_shared<engine::skinned_mesh>();
	mesh->LoadMesh(Filename);
	return mesh;
}

engine::ref<engine::Animation> engine::Animation::create() {
	engine::ref<engine::Animation> animation = std::make_shared<engine::Animation>();
	return animation;
}


void engine::skinned_mesh::min_max_compare(const aiVector3D& point) {
	if (point.x < m_min_point.x)
		m_min_point.x = point.x;
	if (point.x > m_max_point.x)
		m_max_point.x = point.x;
	if (point.y < m_min_point.y)
		m_min_point.y = point.y;
	if (point.y > m_max_point.y)
		m_max_point.y = point.y;
	if (point.z < m_min_point.z)
		m_min_point.z = point.z;
	if (point.z > m_max_point.z)
		m_max_point.z = point.z;
}

void engine::skinned_mesh::ExtractRootMovement(const aiAnimation* animation) {
	std::vector<aiVector3D> frame_offsets;
	aiVector3D              first_frame_offset = animation->mChannels[0]->mPositionKeys[0].mValue;
	for (uint32_t i = 0; i < animation->mChannels[0]->mNumPositionKeys; i++) {
		aiVector3D offset = first_frame_offset - animation->mChannels[0]->mPositionKeys[i].mValue;
		frame_offsets.push_back(offset);
	}
	m_root_movements.push_back(frame_offsets);
}

engine::ref<engine::skinned_mesh> engine::skinned_mesh::create(const engine::ref<engine::skinned_mesh> other_mesh) {
	engine::ref<engine::skinned_mesh> mesh = std::make_shared<engine::skinned_mesh>();

	mesh->m_VAO = other_mesh->m_VAO;
	mesh->m_Entries = other_mesh->m_Entries;
	mesh->m_textures = other_mesh->m_textures;
	mesh->m_BoneTransforms = other_mesh->m_BoneTransforms;
	mesh->m_BoneMapping = other_mesh->m_BoneMapping;
	mesh->m_NumBones = other_mesh->m_NumBones;
	mesh->m_BoneInfo = other_mesh->m_BoneInfo;
	mesh->m_GlobalInverseTransform = other_mesh->m_GlobalInverseTransform;
	mesh->m_pScene = other_mesh->m_pScene;
	mesh->m_extra_animations = other_mesh->m_extra_animations;
	mesh->m_pAnimations = other_mesh->m_pAnimations;
	mesh->m_current_animation_index = other_mesh->m_current_animation_index;
	mesh->m_default_animation_index = other_mesh->m_default_animation_index;
	mesh->m_running_time = other_mesh->m_running_time;
	//mesh->m_Importer = other_mesh->importer();
	mesh->m_AnimationPlaying = other_mesh->m_AnimationPlaying;
	mesh->m_min_point = other_mesh->m_min_point;
	mesh->m_max_point = other_mesh->m_max_point;
	mesh->m_first_point = other_mesh->m_first_point;
	mesh->m_size = other_mesh->m_size;
	mesh->m_offset = other_mesh->m_offset;
	mesh->m_root_movement_on = other_mesh->m_root_movement_on;
	mesh->m_root_movements = other_mesh->m_root_movements;
	mesh->m_current_root_movement_offset = other_mesh->m_current_root_movement_offset;
	mesh->m_update_root_offset = other_mesh->m_update_root_offset;

	return mesh;
}
