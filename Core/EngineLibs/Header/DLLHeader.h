#ifndef DLL_HEADER_H
#define DLL_HEADER_H

// Set Mode to DLL Header Mode
#define DLL_HEADER

// Enable Editor Values
#define EDITOR

// Standard Library
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <functional>
#include <map>
#include <filesystem>
#include <Windows.h>

// GLM
#include "../../../Com/GLM/glm.hpp"
#include "../../../Com/GLFW/glfw3.h"

// Camera
#include "../../Source/Class/Render/Camera/Camera.h"

// Textures
#include "../../Source/Class/Render/Struct/Struct.h"
#include "../../Source/Class/Render/Struct/Texture.h"

// Shapes
#include "../../Source/Class/Render/Shape/Shape.h"
#include "../../Source/Class/Render/Shape/Rectangle.h"
#include "../../Source/Class/Render/Shape/Trapezoid.h"
#include "../../Source/Class/Render/Shape/Triangle.h"
#include "../../Source/Class/Render/Shape/Circle.h"
#include "../../Source/Class/Render/Shape/Polygon.h"

// Main Object File
#include "../../Source/Class/Object/Object.h"

// Collision Masks
#include "../../Source/Class/Object/Collision/CollisionMask.h"
#include "../../Source/Class/Object/Collision/Horizontal/Line/HorizotnalLine.h"
#include "../../Source/Class/Object/Collision/Horizontal/Slant/Slant.h"
#include "../../Source/Class/Object/Collision/Horizontal/Slope/Slope.h"
#include "../../Source/Class/Object/Collision/Vertical/Line/VerticalLine.h"
#include "../../Source/Class/Object/Collision/Vertical/Curve/Curve.h"

// Floor Masks
#include "../../Source/Class/Object/Collision/Horizontal/FloorMask.h"
#include "../../Source/Class/Object/Collision/Horizontal/Line/FloorMaskLine.h"
#include "../../Source/Class/Object/Collision/Horizontal/Slant/FloorMaskSlant.h"
#include "../../Source/Class/Object/Collision/Horizontal/Slope/FloorMaskSlope.h"

// Left Masks
#include "../../Source/Class/Object/Collision/Vertical/LeftMask.h"
#include "../../Source/Class/Object/Collision/Vertical/Line/LeftMaskLine.h"
#include "../../Source/Class/Object/Collision/Vertical/Curve/LeftMaskCurve.h"

// Right Masks
#include "../../Source/Class/Object/Collision/Vertical/RightMask.h"
#include "../../Source/Class/Object/Collision/Vertical/Line/RightMaskLine.h"
#include "../../Source/Class/Object/Collision/Vertical/Curve/RightMaskCurve.h"

// Ceiling Masks
#include "../../Source/Class/Object/Collision/Horizontal/CeilingMask.h"
#include "../../Source/Class/Object/Collision/Horizontal/Line/CeilingMaskLine.h"
#include "../../Source/Class/Object/Collision/Horizontal/Slant/CeilingMaskSlant.h"
#include "../../Source/Class/Object/Collision/Horizontal/Slope/CeilingMaskSlope.h"

// Trigger Masks
#include "../../Source/Class/Object/Collision/Trigger/TriggerMask.h"

// Terrain
#include "../../Source/Class/Object/Terrain/TerrainBase.h"
#include "../../Source/Class/Object/Terrain/Foreground.h"
#include "../../Source/Class/Object/Terrain/Formerground.h"
#include "../../Source/Class/Object/Terrain/Background.h"
#include "../../Source/Class/Object/Terrain/Backdrop.h"

// Lights
#include "../../Source/Class/Object/Lighting/LightBase.h"
#include "../../Source/Class/Object/Lighting/Directional.h"
#include "../../Source/Class/Object/Lighting/Point.h"
#include "../../Source/Class/Object/Lighting/Spot.h"
#include "../../Source/Class/Object/Lighting/Beam.h"

// Physics
#include "../../Source/Class/Object/Physics/PhysicsBase.h"

// Rigid Body Physics
#include "../../Source/Class/Object/Physics/RigidBody/RigidBody.h"
#include "../../Source/Class/Object/Physics/RigidBody/RigidRectangle.h"
#include "../../Source/Class/Object/Physics/RigidBody/RigidTrapezoid.h"
#include "../../Source/Class/Object/Physics/RigidBody/RigidTriangle.h"
#include "../../Source/Class/Object/Physics/RigidBody/RigidCircle.h"
#include "../../Source/Class/Object/Physics/RigidBody/RigidPolygon.h"

// Soft Body Physics
#include "../../Source/Class/Object/Physics/Softody/SoftBody.h"
#include "../../Source/Class/Object/Physics/Softody/SpringMass.h"
#include "../../Source/Class/Object/Physics/Softody/Wire.h"

// Hinge Physics
#include "../../Source/Class/Object/Physics/Hinge/Anchor.h"
#include "../../Source/Class/Object/Physics/Hinge/Hinge.h"

// Entities
#include "../../Source/Class/Object/Entity/EntityBase.h"
#include "../../Source/Class/Object/Entity/NPC.h"
#include "../../Source/Class/Object/Entity/Controllables.h"
#include "../../Source/Class/Object/Entity/Interactables.h"
#include "../../Source/Class/Object/Entity/Dynamics.h"

#endif