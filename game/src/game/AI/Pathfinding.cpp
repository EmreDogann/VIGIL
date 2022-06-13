#include "Pathfinding.h"

#include "engine/events/key_event.h"

// Basic file operations
#include <fstream>

// Adapted from https://github.com/OneLoneCoder/videos/blob/master/OneLoneCoder_PathFinding_AStar.cpp
Pathfinding::Pathfinding() {
	// Initialize Node map.
	// y * m_map_width + x is the current node's index.
	m_nodes = new Node[m_map_width * m_map_height];
	for (int x = 0; x < m_map_width; x++) {
		for (int y = 0; y < m_map_height; y++) {
			m_nodes[y * m_map_width + x].node_map_position = {x, y};
			m_nodes[y * m_map_width + x].node_world_position = {x / m_map_scale - 5.5f, y / m_map_scale - 2.0f};
			m_nodes[y * m_map_width + x].bObstacle = false;
			m_nodes[y * m_map_width + x].parent = nullptr;
			m_nodes[y * m_map_width + x].bVisited = false;
		}
	}

	// Create connections between all nodes. 
	for (int x = 0; x < m_map_width; x++) {
		for (int y = 0; y < m_map_height; y++) {

			// Connect horizontally & vertically.
			if (y > 0) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[(y - 1) * m_map_width + x]); // North neighbour
			}
			if (y < m_map_height - 1) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[(y + 1) * m_map_width + x]); // South neighbour
			}

			if (x > 0) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[y * m_map_width + (x - 1)]); // West neighbour
			}
			if (x < m_map_width - 1) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[y * m_map_width + (x + 1)]); // East neighbour
			}

			// Also connect diagonally.
			if (y > 0 && x > 0) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[(y - 1) * m_map_width + (x - 1)]); // North-West neighbour
			}
			if (y < m_map_height - 1 && x > 0) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[(y + 1) * m_map_width + (x - 1)]); // South-West neighbour
			}

			if (y > 0 && x < m_map_width - 1) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[(y - 1) * m_map_width + (x + 1)]); // North-East neighbour
			}
			if (y < m_map_height - 1 && x < m_map_width - 1) {
				m_nodes[y * m_map_width + x].neighbours.push_back(&m_nodes[(y + 1) * m_map_width + (x + 1)]); // South-East neighbour
			}
		}
	}

	m_node_start = &m_nodes[static_cast<int>((2.8f + 2.0f) * m_map_scale) * m_map_width + static_cast<int>((-1.2f + 5.5f) * m_map_scale)];
	//m_node_end = &m_nodes[(m_map_height / 2) * m_map_width + m_map_width - 2];
	m_node_end = &m_nodes[static_cast<int>((4.6f + 2.0f) * m_map_scale) * m_map_width + static_cast<int>((1.2f + 5.5f) * m_map_scale)];

	// Set up quad game object for map & path visualization.
	m_quad_cell_textures.push_back(engine::texture_2d::create("assets/textures/debug/NotVisitedCell.png", false));
	m_quad_cell_textures.push_back(engine::texture_2d::create("assets/textures/debug/VisitedCell.png", false));
	m_quad_cell_textures.push_back(engine::texture_2d::create("assets/textures/debug/PathCell.png", false));
	m_quad_cell_textures.push_back(engine::texture_2d::create("assets/textures/debug/ObstacleCell.png", false));

	const engine::ref<engine::quad> quad_shape = engine::quad::create({1, 1}, -1);
	engine::game_object_properties  quad_props;
	quad_props.position = {0.f, 0.f, 0.0f};
	quad_props.meshes = {quad_shape->mesh()};
	quad_props.textures = {m_quad_cell_textures[0]};
	quad_props.scale = glm::vec3(1 / m_map_scale);
	quad_props.rotation_axis = {1.0f, 0.0f, 0.0f};
	quad_props.rotation_amount = glm::radians(-90.0f);
	quad_props.type = 1;
	m_quad_cell = engine::game_object::create(quad_props);
}

Pathfinding::~Pathfinding() {}

engine::ref<Pathfinding> Pathfinding::create() {
	return std::make_shared<Pathfinding>();
}

void Pathfinding::update_map(std::vector<engine::ref<engine::bounding_box>>& vec_obstacles) {
	// Reset all nodes on map to not obstacles.
	for (int x = 0; x < m_map_width; x++) {
		for (int y = 0; y < m_map_height; y++) {
			m_nodes[y * m_map_width + x].bObstacle = false;
		}
	}

	for (auto b_box : vec_obstacles) {
		bounding_box_info b_box_info{};

		b_box->get(b_box_info.position, b_box_info.width, b_box_info.height, b_box_info.depth);
		// The bounding box's world position mapped to node map coordinates.
		std::pair<int, int> b_box_map_pos = {
			100 * (((b_box_info.position.x + (b_box_info.width / 2.0f)) + 5.5f) * m_map_scale),
			100 * (((b_box_info.position.z + (b_box_info.depth / 2.0f)) + 2.0f) * m_map_scale)
		};

		std::pair<int, int> b_box_map_dimensions = {
			100 * (b_box_info.width * m_map_scale / 2),
			100 * (b_box_info.depth * m_map_scale / 2)
		};

		// Each quadrant of the bounding box to check.
		// Top Right, Bottom Left, Bottom Right, Top Left
		int quadrant_x[4] = {1, -1, 1, -1};
		int quadrant_y[4] = {1, -1, -1, 1};

		// Set every cell inside the bounding box to be an obstacle.
		// Technically we don't need to mark the entire bounding box area as being an obstacle,
		// just the outer border of the bounding box is required and A* will still work fine,
		// but for the purposes of this game, this is fine.
		for (int i = 0; i < 4; i++) {
			for (int x = 0; x < ((b_box_map_dimensions.first + 99) / 100) + 1 + m_obstacle_padding; x++) {
				for (int y = 0; y < ((b_box_map_dimensions.second + 99) / 100) + 1 + m_obstacle_padding; y++) {
					// Due to floating point rounding errors, when mapping from world to map coordinates,
					// we store the number as a large integer, then round to the nearest hundred using
					// the fact that integer division truncates a floating point number.
					// Source: https://stackoverflow.com/a/18407671/10439539
					int b_box_x = ((b_box_map_pos.first + 99) / 100) + (x * quadrant_x[i]);
					int b_box_y = ((b_box_map_pos.second + 99) / 100) + (y * quadrant_y[i]);

					// Check if map position is in bounds.
					if ((b_box_y >= 0 && b_box_y < m_map_height) && (b_box_x >= 0 && b_box_x < m_map_width)) {
						m_nodes[b_box_y * m_map_width + b_box_x].bObstacle = true;
					}
				}
			}
		}
	}
}

std::vector<glm::vec3> Pathfinding::solve_aStar(glm::vec3 start_pos, glm::vec3 player_pos) {
	m_node_start = &m_nodes[static_cast<int>((start_pos.z + 2.0f) * m_map_scale) * m_map_width + static_cast<int>((start_pos.x + 5.5f) * m_map_scale)];
	m_node_end = &m_nodes[static_cast<int>((player_pos.z + 2.0f) * m_map_scale) * m_map_width + static_cast<int>((player_pos.x + 5.5f) * m_map_scale)];

	// Reset navigation graph.
	for (int x = 0; x < m_map_width; x++) {
		for (int y = 0; y < m_map_height; y++) {
			m_nodes[y * m_map_width + x].parent = nullptr;
			m_nodes[y * m_map_width + x].bVisited = false;
			m_nodes[y * m_map_width + x].fGlobalGoal = INFINITY;
			m_nodes[y * m_map_width + x].fLocalGoal = INFINITY;
		}
	}

	// Lambda function to perform Pythagoras' Theorem.
	auto distance = [](Node* a, Node* b)
	{
		glm::vec2 vec_a = {a->node_map_position.first, a->node_map_position.second};
		glm::vec2 vec_b = {b->node_map_position.first, b->node_map_position.second};
		return sqrtf((vec_a.x - vec_b.x) * (vec_a.x - vec_b.x) + (vec_a.y - vec_b.y) * (vec_a.y - vec_b.y));
	};

	// Compute the heuristic for the map
	auto heuristic = [distance](Node* a, Node* b)
	{
		return distance(a, b);
	};

	// Setup Starting Conditions
	Node* current_node = m_node_start;
	m_node_start->fLocalGoal = 0.0f;
	m_node_start->fGlobalGoal = heuristic(m_node_start, m_node_end);

	// Keep track of nodes that have been found but not visited.
	std::list<Node*> not_tested_nodes;
	not_tested_nodes.push_back(m_node_start);

	// Don't check all cells.
	while (!not_tested_nodes.empty() && current_node != m_node_end) {
		// Sort list based on global goal. Smallest to largest.
		not_tested_nodes.sort([](const Node* lhs, const Node* rhs)
		{
			return lhs->fGlobalGoal < rhs->fGlobalGoal;
		});

		// If the front of the list (the node with the smallest global goal after the sort) has already been visited, remove it from the list.
		while (!not_tested_nodes.empty() && not_tested_nodes.front()->bVisited) {
			not_tested_nodes.pop_front();
		}

		// If there are no valid nodes left...
		if (not_tested_nodes.empty()) {
			break;
		}

		current_node = not_tested_nodes.front();
		current_node->bVisited = true;

		// Check each neighbour of the current node & build not_tested_node list.
		for (auto neighbour : current_node->neighbours) {
			// Only add neighbour if it hasn't been visited and it isn't an obstacle.
			if (!neighbour->bVisited && neighbour->bObstacle == 0) {
				not_tested_nodes.push_back(neighbour);
			}

			// Calculate neighbours potential lowest parent distance.
			float possible_lower_goal = current_node->fLocalGoal + distance(current_node, neighbour);

			// If local goal is lower than neighbour's local goal, update the neighbour.
			if (possible_lower_goal < neighbour->fLocalGoal) {
				neighbour->parent = current_node;
				neighbour->fLocalGoal = possible_lower_goal;

				neighbour->fGlobalGoal = neighbour->fLocalGoal + heuristic(neighbour, m_node_end);
			}

		}
	}

	// Construct array of positions for entities to follow.
	Node* p = m_node_end;

	std::vector<glm::vec3> path;
	while (p->parent != nullptr) {
		path.push_back(glm::vec3(p->node_world_position.x, 0.0f, p->node_world_position.y));
		// Next parent.
		p = p->parent;
	}
	std::reverse(path.begin(), path.end());

	return path;
}

void Pathfinding::on_update(const engine::timestep& time_step) {}

void Pathfinding::on_render(const engine::ref<engine::shader>& shader) {
	for (int x = 0; x < m_map_width; x++) {
		for (int y = 0; y < m_map_height; y++) {
			if (m_nodes[y * m_map_width + x].bVisited || m_nodes[y * m_map_width + x].bObstacle) {
				if (m_nodes[y * m_map_width + x].bObstacle) {
					m_quad_cell->set_textures({m_quad_cell_textures[3]});
				} else if (m_nodes[y * m_map_width + x].bVisited) {
					m_quad_cell->set_textures({m_quad_cell_textures[1]});
				}

				// Uncomment the else statement below to enable rendering of grid nodes that haven't been traversed. WARNING: HUGE PERFORMANCE COST.
				//else {
				//	m_quad_cell->set_textures({m_quad_cell_textures[0]});
				//}

				glm::vec3 cell_world_pos = {m_nodes[y * m_map_width + x].node_world_position.x, 0.35f, m_nodes[y * m_map_width + x].node_world_position.y};

				glm::mat4 quad_cell_transform(1.0f);
				quad_cell_transform = glm::translate(quad_cell_transform, cell_world_pos);
				quad_cell_transform = glm::rotate(quad_cell_transform, m_quad_cell->rotation_amount(), m_quad_cell->rotation_axis());
				quad_cell_transform = glm::scale(quad_cell_transform, m_quad_cell->scale() / glm::vec3(2.0f));
				engine::renderer::submit(shader, quad_cell_transform, m_quad_cell);
			}
		}
	}

	// Render shortest path. Iterate back through the chain.
	if (m_node_end != nullptr) {
		Node* p = m_node_end;
		while (p->parent != nullptr) {
			m_quad_cell->set_textures({m_quad_cell_textures[2]});
			glm::mat4 quad_cell_transform(1.0f);
			quad_cell_transform = glm::translate(quad_cell_transform, {p->node_world_position.x, 0.36f, p->node_world_position.y});
			quad_cell_transform = glm::rotate(quad_cell_transform, m_quad_cell->rotation_amount(), m_quad_cell->rotation_axis());
			quad_cell_transform = glm::scale(quad_cell_transform, m_quad_cell->scale() / glm::vec3(2.0f));
			engine::renderer::submit(shader, quad_cell_transform, m_quad_cell);

			// Next parent.
			p = p->parent;
		}
	}
}

void Pathfinding::on_event(engine::event& event, engine::perspective_camera& m_3d_camera_debug) {}
