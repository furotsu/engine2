#pragma once
#include <DirectXMath.h>


#include <limits>
using namespace DirectX;

//main
//Initial values for window size
constexpr int SCREEN_WIDTH = 400;
constexpr int SCREEN_HEIGHT = 200;
constexpr float NEAR_PLANE = 0.01f;
constexpr float FAR_PLANE = 1000.0f;

//Controller
constexpr float FRAME_DURATION = 1.0f / 60.0f;
constexpr float CAMERA_SPEED = 15.0f;
constexpr float ROLL_ROTATION_SPEED = 60.0f;
constexpr float MIN_CAMERA_SPEED = 0.1f;
constexpr float MAX_CAMERA_SPEED = 1000.0f;
constexpr float CAMERA_NEAR_PLANE = 1.0f;
constexpr float CAMERA_FAR_PLANE = 1000.0f;
constexpr uint16_t MAX_ID = 50000;

// half of screen - 180 degrees/second 
constexpr float MOUSE_SENSITIVITY = 360.0f;

//Renderer
constexpr float LIGHTMODEL_SIZE = 5.0f;
constexpr float LIGHT_POWER_MULTIPLIER = 10.0f;
constexpr float INSULATOR_F0[3] = { 0.04f, 0.04f, 0.04f };
constexpr float START_EV100 = 2.0f;
constexpr int   MAX_POINT_LIGHTS = 5;
constexpr float REFLECTION_MIPS_COUNT = 9.0f;
constexpr int   MULTISAMPLES_COUNT = 1;
constexpr int   SHADOWMAP_SIDE_SIZE = 1024;
constexpr int	SHADOWMAP_RESOURCE_SLOT = 4;
constexpr float DISSOLUTION_SPAWNTIME = 5.0f;
constexpr float DISAPPEARANCE_ERASETIME = 5.0f;
constexpr uint32_t MAX_PARTICLE_NUM = 100000;
constexpr uint32_t GRASS_ID = 0;

//ray minimum and maximum length
constexpr float RAY_MIN = 0.00000000001f;
constexpr float RAY_MAX = std::numeric_limits<float>::infinity();

//utility
constexpr float G_PI = 3.14159265358979323846f;
constexpr float FLOAT_COMPARE_EPSILON = 0.00000001f;
constexpr float GOLDEN_RATIO = 1.618033988749895; // same as : (1.0f + sqrtf(5.0f)) / 2.0f;

//small offset to avoid visual bugs
constexpr float SMALL_OFFSET = 0.00000001f;