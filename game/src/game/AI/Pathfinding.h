#pragma once
#include <engine.h>

// Adapted from https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_PathFinding_AStar.cpp
class Pathfinding {
public:
	Pathfinding();
	~Pathfinding();

	static engine::ref<Pathfinding> create();

	void on_update(const engine::timestep& time_step);
	void on_render(const engine::ref<engine::shader>& shader);
	void on_event(engine::event& event, engine::perspective_camera& m_3d_camera_debug);

	void update_map(std::vector<engine::ref<engine::bounding_box>>& b_box);

	std::vector<glm::vec3> solve_aStar(glm::vec3 start_pos, glm::vec3 player_pos);
private:
	struct Node {
		bool                bObstacle = false;
		bool                bVisited = false;
		float               fGlobalGoal;         // Distance to goal so far
		float               fLocalGoal;          // Distance to goal if we took the alternative route.
		std::pair<int, int> node_map_position;   // X and Y positon on the map.
		glm::vec2           node_world_position; // This node's true space in world space.
		std::vector<Node*>  neighbours;
		Node*               parent;
	};

	struct bounding_box_info {
		bounding_box_info(float width = 1.0f, float height = 1.0f, float depth = 1.0f, glm::vec3 position = glm::vec3(1.0f)) : width(width), height(height), depth(depth),
			position(position) {}

		float     width;
		float     height;
		float     depth;
		glm::vec3 position;
	};

	Node* m_nodes = nullptr;
	int   m_map_width = 111;
	int   m_map_height = 76;
	float m_map_scale = 10.0f;    // Scale of nodes in world space.
	int   m_obstacle_padding = 0; // Padding around each obstacle (in number of cells).

	Node* m_node_start = nullptr;
	Node* m_node_end = nullptr;

	engine::ref<engine::game_object>             m_quad_cell; // Game object used to visualize map.
	std::vector<engine::ref<engine::texture_2d>> m_quad_cell_textures{};
};
