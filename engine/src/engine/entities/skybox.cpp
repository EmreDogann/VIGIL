#include "pch.h"
#include "skybox.h"
#include <engine.h>

engine::skybox::skybox(float size, std::vector<engine::ref<engine::texture_2d>> textures) : m_size(size), m_textures(textures)
{
	std::vector<engine::mesh::vertex> skybox_front
	{
		//                position								 normal						 tex coord       
		{ {-1.f * m_size, -1.f * m_size,  1.f * m_size },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 0.f } },
		{ { 1.f * m_size, -1.f * m_size,  1.f * m_size },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 0.f } },
		{ { 1.f * m_size,  1.f * m_size,  1.f * m_size },		{ 0.0f, 0.0f,  -1.0f },		{ 1.f, 1.f } },
		{ {-1.f * m_size,  1.f * m_size,  1.f * m_size },		{ 0.0f, 0.0f,  -1.0f },		{ 0.f, 1.f } },
	};

	std::vector<engine::mesh::vertex> skybox_right
	{
		//                position								 normal						 tex coord   
		{ { 1.f * m_size, -1.f * m_size,  1.f * m_size },		{ -1.0f, 0.0f, 0.0f},		{ 0.f, 0.f } },
		{ { 1.f * m_size, -1.f * m_size, -1.f * m_size },		{ -1.0f, 0.0f, 0.0f},		{ 1.f, 0.f } },
		{ { 1.f * m_size,  1.f * m_size, -1.f * m_size },		{ -1.0f, 0.0f, 0.0f},		{ 1.f, 1.f } },
		{ { 1.f * m_size,  1.f * m_size,  1.f * m_size },		{ -1.0f, 0.0f, 0.0f},		{ 0.f, 1.f } },
	};

	std::vector<engine::mesh::vertex> skybox_back
	{
		//                position								 normal						 tex coord   
		{ { 1.f * m_size, -1.f * m_size, -1.f * m_size },		{ 0.0f, 0.0f, 1.0f },		{ 0.f, 0.f } },
		{ {-1.f * m_size, -1.f * m_size, -1.f * m_size },		{ 0.0f, 0.0f, 1.0f },		{ 1.f, 0.f } },
		{ {-1.f * m_size,  1.f * m_size, -1.f * m_size },		{ 0.0f, 0.0f, 1.0f },		{ 1.f, 1.f } },
		{ { 1.f * m_size,  1.f * m_size, -1.f * m_size },		{ 0.0f, 0.0f, 1.0f },		{ 0.f, 1.f } },
	};

	std::vector<engine::mesh::vertex> skybox_left
	{
		//                position								 normal						 tex coord   
		{ {-1.f * m_size, -1.f * m_size, -1.f * m_size },		{1.0f, 0.0f, 0.0f},	    	{ 0.f, 0.f } },
		{ {-1.f * m_size, -1.f * m_size,  1.f * m_size },		{1.0f, 0.0f, 0.0f},		    { 1.f, 0.f } },
		{ {-1.f * m_size,  1.f * m_size,  1.f * m_size },		{1.0f, 0.0f, 0.0f},		    { 1.f, 1.f } },
		{ {-1.f * m_size,  1.f * m_size, -1.f * m_size },		{1.0f, 0.0f, 0.0f},		    { 0.f, 1.f } },
	};

	std::vector<engine::mesh::vertex> skybox_top
	{
		//                position								 normal						 tex coord   
		{ {-1.f * m_size,  1.f * m_size,  1.f * m_size },		{ 0.0f,  -1.0f, 0.0f},		{ 0.f, 0.f } },
		{ { 1.f * m_size,  1.f * m_size,  1.f * m_size },		{ 0.0f,  -1.0f, 0.0f},		{ 1.f, 0.f } },
		{ { 1.f * m_size,  1.f * m_size, -1.f * m_size },		{ 0.0f,  -1.0f, 0.0f},		{ 1.f, 1.f } },
		{ {-1.f * m_size,  1.f * m_size, -1.f * m_size },		{ 0.0f,  -1.0f, 0.0f},		{ 0.f, 1.f } },
	};

	std::vector<engine::mesh::vertex> skybox_bottom
	{
		//                position								 normal						 tex coord   
		{ {-1.f * m_size, -1.f * m_size, -1.f * m_size },		{ 0.0f, 1.0f, 0.0f},		{ 0.f, 0.f } },
		{ { 1.f * m_size, -1.f * m_size, -1.f * m_size },		{ 0.0f, 1.0f, 0.0f},		{ 1.f, 0.f } },
		{ { 1.f * m_size, -1.f * m_size,  1.f * m_size },		{ 0.0f, 1.0f, 0.0f},		{ 1.f, 1.f } },
		{ {-1.f * m_size, -1.f * m_size,  1.f * m_size },		{ 0.0f, 1.0f, 0.0f},		{ 0.f, 1.f } },
	};

	std::vector<uint32_t> skybox_indices
	{
		 0,  2,  1,		  0,  3,  2
	};

	m_meshes.push_back(engine::mesh::create(skybox_front, skybox_indices));
	m_meshes.push_back(engine::mesh::create(skybox_right, skybox_indices));
	m_meshes.push_back(engine::mesh::create(skybox_back, skybox_indices));
	m_meshes.push_back(engine::mesh::create(skybox_left, skybox_indices));
	m_meshes.push_back(engine::mesh::create(skybox_top, skybox_indices));
	m_meshes.push_back(engine::mesh::create(skybox_bottom, skybox_indices));
}

engine::skybox::~skybox() {}

engine::ref<engine::skybox> engine::skybox::create(float size, std::vector<engine::ref<engine::texture_2d>> textures)
{
	return std::make_shared<engine::skybox>(size, textures);
}
