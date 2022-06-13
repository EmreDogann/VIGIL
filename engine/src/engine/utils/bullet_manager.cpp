#include "pch.h"

#include "engine/utils/bullet_manager.h"
#include "engine/renderer/mesh.h"
#include "LinearMath/btIDebugDraw.h"
#include "BulletDynamics/Dynamics/btDynamicsWorld.h"

#include "BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"//picking
#include "BulletDynamics/ConstraintSolver/btGeneric6DofConstraint.h"//picking

#include "BulletCollision/CollisionShapes/btCollisionShape.h"
#include "BulletCollision/CollisionShapes/btBoxShape.h"
#include "BulletCollision/CollisionShapes/btSphereShape.h"
#include "BulletCollision/CollisionShapes/btCompoundShape.h"
#include "BulletCollision/CollisionShapes/btUniformScalingShape.h"
#include "BulletDynamics/ConstraintSolver/btConstraintSolver.h"
#include "LinearMath/btQuickprof.h"
#include "LinearMath/btDefaultMotionState.h"
#include "LinearMath/btSerializer.h"

#include "math.h"

///btBulletDynamicsCommon.h is the main Bullet include file, contains most common include files.
#include "btBulletDynamicsCommon.h"

engine::physical_object::physical_object(btRigidBody* body) : body(body) {}

//returns the forward vector of the physical object
btVector3 engine::physical_object::get_forward()
{
	btVector3 body_forward_vector;
	btTransform trans = body->getWorldTransform();
	body_forward_vector = (trans * forward_vector - trans.getOrigin());
	return body_forward_vector;
}

//Returns the up vector of the physical object
btVector3 engine::physical_object::get_up()
{
	btVector3 body_up_vector;
	btTransform trans = body->getWorldTransform();
	body_up_vector = (trans * up_vector - trans.getOrigin());
	return body_up_vector;
}

//Returns the right vector of the physical object
btVector3 engine::physical_object::get_right()
{
	btVector3 body_right_vector;
	btTransform trans = body->getWorldTransform();
	body_right_vector = (trans * right_vector - trans.getOrigin());
	return body_right_vector;
}

//Constructor passing the game object vector
engine::bullet_manager::bullet_manager(const std::vector<engine::ref<engine::game_object>>& game_objects)
{
	init_physics(game_objects, m_dynamics_world);
}

//Default constructor
engine::bullet_manager::bullet_manager()
{
	init_physics({}, m_dynamics_world);
}

//Destructor
engine::bullet_manager::~bullet_manager()
{
	for (uint32_t i = 0; i < physical_objects.size(); i++)
	{
		delete_shape(physical_objects.at(i)->get_body());
	}
	delete m_dynamics_world;
	delete m_overlapping_pair_cache;
	delete m_dispatcher;
	delete m_constraint_solver;
	delete m_collision_configuration;
}

//Physics initialisation
void	engine::bullet_manager::init_physics(const std::vector<engine::ref<engine::game_object>>& game_objects, btDynamicsWorld* dynamics_world)
{

	// init world
	m_collision_configuration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collision_configuration);
	btVector3 world_min(-1000, -1000, -1000);
	btVector3 world_max(1000, 1000, 1000);
	m_overlapping_pair_cache = new btAxisSweep3(world_min, world_max);

	m_constraint_solver = new btSequentialImpulseConstraintSolver();

	btDiscreteDynamicsWorld* wp = new btDiscreteDynamicsWorld(m_dispatcher, m_overlapping_pair_cache, m_constraint_solver, m_collision_configuration);
	//	wp->getSolverInfo().m_numIterations = 20; // default is 10
	m_dynamics_world = wp;
	//m_dynamicsWorld->setInternalTickCallback(MyTickCallback, static_cast<void *>(this), true);

	for (int32_t i = 0; i < game_objects.size(); i++)
	{
		game_objects.at(i)->set_physical_object_index(i);
		add_physical_object(game_objects.at(i), dynamics_world);
	}

	wp->setGravity(btVector3(0, -9.8f, 0));
}

//Creating a rigidBody from a collision shape
btRigidBody*	engine::bullet_manager::local_create_rigid_body(float mass, const btTransform& start_transform, btCollisionShape* shape, btDynamicsWorld* dynamics_world)
{
	btAssert((!shape || shape->getShapeType() != INVALID_SHAPE_PROXYTYPE));

	//rigidbody is dynamic if and only if mass is non zero, otherwise static
	bool is_dynamic = (mass != 0.f);

	btVector3 local_inertia(0, 0, 0);
	if (is_dynamic)
		shape->calculateLocalInertia(mass, local_inertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects

#define USE_MOTIONSTATE 1
#ifdef USE_MOTIONSTATE
	btDefaultMotionState* my_motion_state = new btDefaultMotionState(start_transform);



	btRigidBody::btRigidBodyConstructionInfo c_info(mass, my_motion_state, shape, local_inertia);

	btRigidBody* body = new btRigidBody(c_info);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);

#else
	btRigidBody* body = new btRigidBody(mass, 0, shape, local_inertia);
	body->setWorldTransform(start_transform);
#endif//
	get_dynamics_world()->addRigidBody(body);
	
	return body;
}

//Adding a new physical object to the physics simulation by using info from the passed game object
void engine::bullet_manager::add_physical_object(engine::ref<engine::game_object> game_object, btDynamicsWorld* dynamics_world)
{
	btCollisionShape* shape;
	bool done = false;

	switch (game_object->type())
	{
		//if type is a box shape
	case 0:
	{
		shape = new btBoxShape(btVector3(btScalar(game_object->bounding_shape().x) * btScalar(game_object->scale().x),
										 btScalar(game_object->bounding_shape().y) * btScalar(game_object->scale().y),
										 btScalar(game_object->bounding_shape().z) * btScalar(game_object->scale().z)));
		break;
	}
	//if type is a sphere shape
	case 1:
	{
		shape = new btSphereShape(btScalar(game_object->bounding_shape().x) * btScalar(game_object->scale().x));
		break;
	}
	//if type is a convex shape 
	case 2:
	{
		btConvexHullShape * c_shape = new btConvexHullShape();
		for (int32_t i = 0; i < game_object->meshes().at(0)->vertices().size(); i++)
		{
			glm::vec3 vertex = game_object->meshes().at(0)->vertices().at(i).position;
			c_shape->addPoint(btVector3(btScalar(vertex.x), btScalar(vertex.y), btScalar(vertex.z)));
		}
		shape = c_shape;
		break;
	}
	}
	m_collision_shapes.push_back(shape);

	if (!done)
	{
		btTransform trans;
		trans.setIdentity();
		btVector3 pos(btScalar(game_object->position().x), btScalar(game_object->position().y), btScalar(game_object->position().z));
		trans.setOrigin(pos);
		btQuaternion quat;
		quat.setRotation(to_bt_vector3(game_object->rotation_axis()), btScalar(game_object->rotation_amount()));
		trans.setRotation(quat);

		btScalar mass;
		if (!game_object->is_static())
		{
			mass = btScalar(game_object->mass());
		}
		else
		{
			mass = 0.0f;
		}

		btVector3 local_inertia;
		shape->calculateLocalInertia(mass, local_inertia);



		btRigidBody* body = local_create_rigid_body(mass, trans, shape, dynamics_world);

		body->setRestitution(game_object->restitution());
		body->setFriction(game_object->friction());
		if (game_object->lock_angular_factor())
			body->setAngularFactor(btVector3(0.f, 0.f, 0.f));
		//body->setRestitution(game_object->restitution());
		physical_object* object = new physical_object(body);
		physical_objects.push_back(object);
	}
}

//Step the sinulation (update method)
void engine::bullet_manager::dynamics_world_update(const std::vector<engine::ref<engine::game_object>>& game_objects, double dt)
{
	//check if the vector of game_objects corresponds to the vector of physical_bojects
	for (int32_t i = 0; i < physical_objects.size();)
	{
		if (i < game_objects.size())
		{
			if (game_objects.at(i)->physical_object_index() > i)
			{
				delete_shape(physical_objects.at(i)->get_body());
				delete physical_objects.at(i);
				physical_objects.erase(physical_objects.begin()+i);
			}
			else
				i++;
		}
		else
		{
			delete_shape(physical_objects.at(i)->get_body());
			delete physical_objects.at(i);
			physical_objects.erase(physical_objects.begin() + i);
		}
	}
	for (int32_t i = physical_objects.size(); i < game_objects.size(); i++)
	{
		game_objects.at(i)->set_physical_object_index(i);
		add_physical_object(game_objects.at(i), m_dynamics_world);
	}

	if (physical_objects.size() == game_objects.size())
	{
		int32_t i = 0;
		for (int32_t i = 0; i < physical_objects.size(); i++)
		{
			
				btTransform trans;
				engine::ref<engine::game_object> game_object_i = game_objects.at(i);
				physical_object* physical_object_i = physical_objects.at(i);

				btVector3 pos = to_bt_vector3(game_object_i->position());
				trans.setOrigin(pos);
				trans.setRotation(btQuaternion(to_bt_vector3(game_object_i->rotation_axis()), btScalar(game_object_i->rotation_amount())));
				btDefaultMotionState* myMotionState = new btDefaultMotionState(trans);

				physical_object_i->get_body()->setMotionState(myMotionState);
				physical_object_i->get_body()->setLinearVelocity(to_bt_vector3(game_object_i->velocity()));
				physical_object_i->get_body()->setAngularVelocity(to_bt_vector3(game_object_i->angular_velocity()));

				//If the object is supposed to be moving, set it's activation state
				if (game_object_i->angular_velocity() != glm::vec3(0.0f) ||
					game_object_i->velocity() != glm::vec3(0.0f))
				{
					physical_object_i->get_body()->setActivationState(ACTIVE_TAG);
				}

				//physical_object_i->get_body()->clearForces();
				if (game_object_i->acceleration() != glm::vec3(0.0f))
				{
					physical_object_i->get_body()->applyCentralForce(to_bt_vector3(game_object_i->acceleration()));
					game_object_i->set_acceleration(glm::vec3(0.0f));
					physical_object_i->get_body()->setActivationState(ACTIVE_TAG);
				}
				if (game_object_i->torque() != glm::vec3(0.0f))
				{
					physical_object_i->get_body()->applyTorque(to_bt_vector3(game_object_i->torque()));
					game_object_i->set_torque(glm::vec3(0.0f));
					physical_object_i->get_body()->setActivationState(ACTIVE_TAG);
				}
			
		}
	}
	///step the simulation
	if (m_dynamics_world)
	{
		m_dynamics_world->stepSimulation((float)dt, 0);//ms / 1000000.f);
	}
	for (int32_t i = 0; i < game_objects.size(); i++)
	{
		if (!game_objects.at(i)->is_static())
		{
			engine::ref<engine::game_object> game_object_i = game_objects.at(i);
			physical_object* physical_object_i = physical_objects.at(i);

			game_object_i->set_position(to_vec3(physical_object_i->get_body()->getCenterOfMassPosition()));
			game_object_i->set_rotation_axis(to_vec3(physical_object_i->get_body()->getCenterOfMassTransform().getRotation().getAxis()));
			game_object_i->set_rotation_amount(physical_object_i->get_body()->getCenterOfMassTransform().getRotation().getAngle());

			game_object_i->set_forward(to_vec3(physical_object_i->get_forward()));
			game_object_i->set_up(to_vec3(physical_object_i->get_up()));
			game_object_i->set_right(to_vec3(physical_object_i->get_right()));

			game_object_i->set_velocity(to_vec3(physical_object_i->get_body()->getLinearVelocity()));
			game_object_i->set_angular_velocity(to_vec3(physical_object_i->get_body()->getAngularVelocity()));

			game_object_i->set_physical_object_index(i);
		}
	}
}

engine::ref<engine::bullet_manager> engine::bullet_manager::create(const std::vector<engine::ref<engine::game_object>>& game_objects)
{
	return std::make_shared<engine::bullet_manager>(game_objects);
}

void engine::bullet_manager::delete_shape(btRigidBody* rigid_body) {
	delete rigid_body->getMotionState();
	delete rigid_body->getCollisionShape();
	m_dynamics_world->removeRigidBody(rigid_body);
	delete rigid_body;
}
