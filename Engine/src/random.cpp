#include "random.hpp"

#include <ctime>
#include <cstdlib>
#include <utility>

void math::setSeed()
{
	std::srand(time(NULL));
}

int math::randRangei(int min, int max)
{
	return std::rand() % (max - min + 1) + min;
}

float math::randRangef(float min, float max)
{
	return min + static_cast <float> (std::rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}