#pragma once

#include "LinearMath/btVector3.h"
#include "LinearMath/btMatrix3x3.h"
#include "LinearMath/btTransform.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btAlignedObjectArray.h"

#include "engine/entities/game_object.h"

class btBroadphaseInterface;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
struct btCollisionAlgorithmCreateFunc;
class btDefaultCollisionConfiguration;

class	btCollisionShape;
class	btDynamicsWorld;
class	btRigidBody;
class	btTypedConstraint;

namespace engine
{
	/// \brief Class defining the physical objects in he bullet Dynamics World simulation. 
	class physical_object
	{
		btRigidBody* body;	//boid's rigidBody

	public:
		physical_object(btRigidBody* body);

		btRigidBody* get_body() const { return body; };	//returns object's rigidBody
		btVector3 get_forward();	//returns object's  forward direction vector
		btVector3 get_up();			//returns object's up vector
		btVector3 get_right();		//returns object's right direction vector

		void apply_forces();		//aplies drag, thrust, lift, gravity, etc.
	};

	//vectors
	const btVector3 right_vector(-1, 0, 0);
	const btVector3 up_vector(0, 1, 0);
	const btVector3 forward_vector(0, 0, 1);

	//PI
	const btScalar PI = (btScalar)3.1415926535897;


	/// \brief Interface through which that provides the interaction between the physical simulation in the bullet engine and the layers in the game application 
	class bullet_manager
	{
	public:
		/// \brief Constructors
		bullet_manager();
		bullet_manager(const std::vector<engine::ref<engine::game_object>>& game_objects);

		/// \brief Destructor
		~bullet_manager();

		/// \brief Methods
		// initialises the physics simulation, creates the physical objects vector and sets up all the parameters
		void	init_physics(const std::vector<engine::ref<engine::game_object>>& game_objects, btDynamicsWorld* dynamics_world);

		// Creates a new physical object from a game object and adds it to the vector
		void add_physical_object(engine::ref<engine::game_object> game_object, btDynamicsWorld* dynamics_world);
		// Creates a rigid body given the collision shape defined in the game object
		btRigidBody*	local_create_rigid_body(float mass, const btTransform& start_transform, btCollisionShape* shape, btDynamicsWorld* dynamics_world);

		// updates the variables of each physical object to synchronise with game objects, steps the simulation and then updates the game objects to synchronise with the physical objects
		void dynamics_world_update(const std::vector<engine::ref<engine::game_object>>& game_objects, double dt);

		static ref<bullet_manager> create(const std::vector<ref<game_object>>& game_objects);

		// getter methods
		btDynamicsWorld* get_dynamics_world() const { return m_dynamics_world; }
		inline btVector3 to_bt_vector3(const glm::vec3& vec3) { return btVector3(btScalar(vec3.x), btScalar(vec3.y), btScalar(vec3.z)); }
		inline glm::vec3 to_vec3(const btVector3& bt_vec3) { return { bt_vec3.getX(), bt_vec3.getY(), bt_vec3.getZ() }; }

		void delete_shape(btRigidBody* rigid_body);

		/// \brief Fields
		btBroadphaseInterface*	m_overlapping_pair_cache;
		btCollisionDispatcher*	m_dispatcher;
		btConstraintSolver*	m_constraint_solver;
		btDefaultCollisionConfiguration* m_collision_configuration;
		btAlignedObjectArray<btCollisionShape*>	m_collision_shapes;

		// vector of physical objects. Correspond 1 - 1 to the game objects vector in the active layer of the game.
		std::vector<physical_object*> physical_objects;

	protected:
		// Dynamic world simulation
		btDynamicsWorld*		m_dynamics_world;
	};
}
