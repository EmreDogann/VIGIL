#pragma once

namespace engine {
	class mesh;
	class texture_2d;
	class shader;
	class skinned_mesh;
	class bounding_box;

	/// \brief Struct used to initialise/create a game object
	struct game_object_properties final {
		std::vector<ref<mesh>>       meshes{};
		ref<skinned_mesh>            animated_mesh{};
		std::vector<ref<texture_2d>> textures{};
		glm::vec3                    position{0};
		glm::vec3                    velocity{0};
		glm::vec3                    scale{1};
		glm::vec3                    bounding_shape{0};
		glm::vec3                    rotation_axis{0.f, 1.f, 0.f};
		int32_t                      type = 0;
		std::string                  tag = ""; // Represents what type of object this is (e.g. Enemy, Player, etc).
		float                        mass = 1.f;
		float                        restitution = 0.1f;
		float                        friction = 0.9f;
		float                        rotation_amount = 0.f;
		bool                         is_static = false;
	};


	/// \brief Base class for storing the physical information about the objects in the game
	class game_object {
	public:
		// TODO - use the following commented ctor only
		game_object(const game_object_properties& props);
		// TODO - lines shouldn't exceed 100 characters, for better readability.
		// TODO - comments in header files should start with "/// \brief ", and there should be one per member/method so that intellisense can show tooltips properly
		/*game_object(const glm::vec3 position, const glm::vec3 velocity, std::vector<ref<mesh>> meshes, const glm::vec3 scale, const bool is_static, float mass, int32_t type, glm::vec3 bounding_shape, glm::vec3 rotation_axis, float rotation_amount);
		game_object(const bool is_static, int32_t type, glm::vec3 bounding_shape);
		game_object(std::vector<ref<mesh>> meshes, int32_t type, glm::vec3 bounding_shape);*/
		/// \brief Destructor -TODO: comments like these are not useful. If the method does something other ppl need to be aware of that's what you should write.
		~game_object();

		/// \brief Getter methods
		glm::vec3 position() const {
			return m_position;
		}

		glm::vec3 velocity() const {
			return m_velocity;
		}

		glm::vec3 acceleration() const {
			return m_acceleration;
		}

		glm::vec3 right() const {
			return m_right;
		}

		glm::vec3 up() const {
			return m_up;
		}

		glm::vec3 forward() const {
			return m_forward;
		}

		glm::vec3 rotation_axis() const {
			return m_rotation_axis;
		}

		float rotation_amount() const {
			return m_rotation_amount;
		}

		glm::vec3 angular_velocity() const {
			return m_angular_velocity;
		}

		glm::vec3 torque() const {
			return m_torque;
		}

		glm::vec3 scale() const {
			return m_scale;
		}

		bool is_static() const {
			return s_static;
		}

		bool lock_angular_factor() const {
			return s_lock_angular_factor;
		}

		float mass() const {
			return m_mass;
		}

		float restitution() const {
			return m_restitution;
		}

		float friction() const {
			return m_friction;
		}

		std::string tag() const {
			return m_tag;
		}

		bool should_remove() const {
			return to_remove;
		}

		int32_t type() const {
			return m_type;
		}

		glm::vec3 bounding_shape() const {
			return m_bounding_shape;
		}

		std::vector<ref<bounding_box>> bounding_box() const {
			return m_bounding_boxes;
		}

		const std::vector<ref<mesh>>& meshes() const {
			return m_meshes;
		}

		const std::vector<ref<texture_2d>>& textures() const {
			return m_textures;
		}

		ref<skinned_mesh> animated_mesh() const {
			return m_animated_mesh;
		}

		glm::vec3 offset() const {
			return m_offset;
		}

		int32_t physical_object_index() const {
			return m_physical_object_index;
		}

		/// \brief Setter methods
		void set_position(glm::vec3 position) {
			m_position = position;
		}

		void set_velocity(glm::vec3 velocity) {
			m_velocity = velocity;
		}

		void set_acceleration(glm::vec3 acceleration) {
			m_acceleration = acceleration;
		}

		void set_right(glm::vec3 right) {
			m_right = right;
		}

		void set_up(glm::vec3 up) {
			m_up = up;
		}

		void set_forward(glm::vec3 forward) {
			m_forward = forward;
		}

		void set_rotation_axis(glm::vec3 rotation_axis) {
			m_rotation_axis = rotation_axis;
		}

		void set_rotation_amount(float rotation_amount) {
			m_rotation_amount = rotation_amount;
		}

		void set_angular_velocity(glm::vec3 angular_velocity) {
			m_angular_velocity = angular_velocity;
		}

		void set_torque(glm::vec3 torque) {
			m_torque = torque;
		}

		void set_scale(glm::vec3 scale) {
			m_scale = scale;
		}

		void set_mass(float mass) {
			m_mass = mass;
		}

		void set_remove(bool val) {
			to_remove = val;
		}

		void set_type(int32_t type) {
			m_type = type;
		}

		void set_bounding_shape(glm::vec3 bounding_shape) {
			m_bounding_shape = bounding_shape;
		}

		void add_bounding_box(ref<engine::bounding_box>& b_box) {
			m_bounding_boxes.push_back(b_box);
		}

		void set_mesh(ref<mesh> mesh) {
			m_meshes.push_back(mesh);
		}

		void set_mesh(std::vector<ref<mesh>> meshes) {
			m_meshes = meshes;
		}

		void set_textures(std::vector<ref<texture_2d>> textures) {
			m_textures = textures;
		}

		void set_animated_mesh(ref<skinned_mesh> animated_mesh) {
			m_animated_mesh = animated_mesh;
		}

		void set_offset(glm::vec3 offset) {
			m_offset = offset;
		}

		void set_physical_object_index(int32_t index) {
			m_physical_object_index = index;
		}

		// rotate the object to face the specific direction
		void turn_towards(glm::vec3 direction);

		// create a transform matrix using the object's position, rotation and scale member variables
		const void transform(glm::mat4& object_transform);

		// bind the object's textures if there are any
		void bind_textures();


		void set_angular_factor_lock(bool angular_factor_lock) {
			s_lock_angular_factor = angular_factor_lock;
		}

	public:
		static ref<game_object> create(const game_object_properties& props);

	private:
		/// \brief Fields
		// object's position vectors
		glm::vec3 m_position{0.f};
		// object's velocity vector 
		glm::vec3 m_velocity{0.f};
		// object's acceleration vector
		glm::vec3 m_acceleration{0.f};

		// object's right vector
		glm::vec3 m_right{1.0f, 0.0f, 0.0f};
		// object's up vector
		glm::vec3 m_up{0.0f, 1.0f, 0.0f};
		// object's forward vector
		glm::vec3 m_forward{0.0f, 0.0f, 1.0f};

		// object's rotation axis vector 
		glm::vec3 m_rotation_axis{0.0f, 1.0f, 0.0f};
		// object's rotation amount
		float m_rotation_amount{0.f};

		// object's angular velocity vector
		glm::vec3 m_angular_velocity{0.0f};
		// object's torque vector
		glm::vec3 m_torque{0.0f};

		// object's meshes
		std::vector<ref<mesh>> m_meshes;
		// object's textures
		std::vector<ref<texture_2d>> m_textures;

		// object's animated mesh
		ref<skinned_mesh> m_animated_mesh;

		// object's scale vector
		glm::vec3 m_scale{1.0f};
		// object's mass
		float m_mass{1.0f};
		// object's restitution
		float m_restitution{0.1f};
		// object's friction
		float m_friction{0.9f};
		// static/non-static object
		bool s_static{false};

		// object's angular factor
		bool s_lock_angular_factor{false};

		// Represents what type of object this is (e.g. Enemy, Player, etc).
		std::string m_tag = "";

		// Whether or not the game object as been marked for deletion.
		bool to_remove = false;

		// object's bounding shape type
		int32_t m_type;
		// object's bounding shape dimensions
		glm::vec3 m_bounding_shape{0.f};
		//object's offset from the local origin
		glm::vec3 m_offset{0.f};

		//index of the bullet physical_object corresponding to this game_object. -1 if hasn't been assigned yet
		int32_t m_physical_object_index{-1};

	protected:
		// Object's bounding boxes used for collision detection.
		std::vector<ref<engine::bounding_box>> m_bounding_boxes;
	};
}
