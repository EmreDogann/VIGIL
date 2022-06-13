#pragma once

// For use by engine applications

#include "engine/core/application.h"
#include "engine/core/layer.h"
#include "engine/core/logger.h"

#include "engine/core/timestep.h"

#include "engine/core/input.h"
#include "engine/key_codes.h"

// ---renderer------------------------
#include "engine/renderer/renderer.h"
#include "engine/renderer/render_command.h"

#include "engine/renderer/buffer.h"
#include "engine/renderer/shader.h"
#include "engine/renderer/texture.h"
#include "engine/renderer/vertex_array.h"
#include "engine/renderer/model.h"
#include "engine/renderer/camera.h"
#include "engine/renderer/fixed_camera.h"
#include "engine/renderer/light.h"
#include "engine/renderer/material.h"
#include "engine/renderer/text_manager.h"
#include "engine/renderer/skinned_mesh.h"

// ---entities------------------------
#include "engine/entities/game_object.h"
#include "engine/entities/shapes/cuboid.h"
#include "engine/entities/shapes/quad.h"
#include "engine/entities/shapes/car.h"
#include "engine/entities/shapes/first_aid_spray.h"
#include "engine/entities/shapes/ammo_box.h"
#include "engine/entities/shapes/sphere.h"
#include "engine/entities/terrain.h"
#include "engine/entities/skybox.h"

// ---utils---------------------------
#include "engine/utils/bullet_manager.h"
#include "engine/utils/audio_manager.h"
