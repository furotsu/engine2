#include <vector>
#include <stdint.h>

#include <DirectXMath.h>


namespace PoissonGenerator
{
	class DefaultPRNG
	{
	public:
		DefaultPRNG() = default;
		explicit DefaultPRNG(unsigned int seed) :seed_(seed) {}
		inline float randomFloat()
		{
			seed_ *= 521167;
			uint32_t a = (seed_ & 0x007fffff) | 0x40000000;
			// remap to 0..1
			return 0.5f * (*((float*)&a) - 2.0f);
		}
		inline uint32_t randomInt(uint32_t maxInt)
		{
			return uint32_t(randomFloat() * maxInt);
		}
		inline uint32_t getSeed() const { return seed_; }
	private:
		uint32_t seed_ = 7133167;
	};

	struct Point
	{
		Point() = default;
		Point(float X, float Y)
			: x(X), y(Y), valid_(true)
		{}
		float x = 0.0f;
		float y = 0.0f;
		bool valid_ = false;
		
		bool isInRectangle() const
		{
			return x >= 0 && y >= 0 && x <= 1 && y <= 1;
		}
		
		bool isInCircle() const
		{
			const float fx = x - 0.5f;
			const float fy = y - 0.5f;
			return (fx * fx + fy * fy) <= 0.25f;
		}

		Point& operator+(const Point& p)
		{
			x += p.x;
			y += p.y;
			return *this;
		}

		Point& operator-(const Point& p)
		{
			x -= p.x;
			y -= p.y;
			return *this;
		}
	};

	struct GridPoint
	{
		GridPoint() = delete;
		GridPoint(int X, int Y)
			: x(X), y(Y)
		{}
		int x;
		int y;
	};

	float getDistance(const Point& P1, const Point& P2)
	{
		return sqrt((P1.x - P2.x) * (P1.x - P2.x) + (P1.y - P2.y) * (P1.y - P2.y));
	}

	GridPoint imageToGrid(const Point& P, float cellSize)
	{
		return GridPoint((int)(P.x / cellSize), (int)(P.y / cellSize));
	}

	struct Grid
	{
	private:
		int w_;
		int h_;
		float cellSize_;
		std::vector< std::vector<Point> > grid_;

	public:
		Grid(int w, int h, float cellSize)
			: w_(w), h_(h), cellSize_(cellSize)
		{
			grid_.resize(h_);
			for (auto i = grid_.begin(); i != grid_.end(); i++) { i->resize(w); }
		}

		void insert(const Point& p)
		{
			const GridPoint g = imageToGrid(p, cellSize_);
			grid_[g.x][g.y] = p;
		}

		bool isInNeighbourhood(const Point& point, float minDist, float cellSize)
		{
			const GridPoint g = imageToGrid(point, cellSize);

			// number of adjucent cells to look for neighbour points
			const int D = 5;

			// scan the neighbourhood of the point in the grid
			for (int i = g.x - D; i <= g.x + D; i++)
			{
				for (int j = g.y - D; j <= g.y + D; j++)
				{
					if (i >= 0 && i < w_ && j >= 0 && j < h_)
					{
						const Point P = grid_[i][j];

						if (P.valid_ && getDistance(P, point) < minDist)
							return true;

					}
				}
			}

			return false;
		}
	};

	template <typename PRNG>
	Point popRandom(std::vector<Point>& points, PRNG& generator)
	{
		const int idx = generator.randomInt(static_cast<int>(points.size()) - 1);
		const Point p = points[idx];
		points.erase(points.begin() + idx);
		return p;
	}

	template <typename PRNG>
	Point generateRandomPointAround(const Point& p, float minDist, PRNG& generator)
	{
		// start with non-uniform distribution
		const float R1 = generator.randomFloat();
		const float R2 = generator.randomFloat();

		// radius should be between MinDist and 2 * MinDist
		const float radius = minDist * (R1 + 1.0f);

		// random angle
		const float angle = 2 * 3.141592653589f * R2;

		// the new point is generated around the point (x, y)
		const float x = p.x + radius * cos(angle);
		const float y = p.y + radius * sin(angle);

		return Point(x, y);
	}

	/**
		Return a vector of generated points
		NewPointsCount - refer to bridson-siggraph07-poissondisk.pdf for details (the value 'k')
		Circle  - 'true' to fill a circle, 'false' to fill a rectangle
		MinDist - minimal distance estimator, use negative value for default
	**/
	template <typename PRNG = DefaultPRNG>
	std::vector<Point> generatePoissonPoints(
		uint32_t numPoints,
		PRNG& generator,
		bool isCircle = true,
		uint32_t newPointsCount = 30,
		float minDist = -1.0f
	)
	{
		numPoints *= 2;

		// if we want to generate a Poisson square shape, multiply the estimate number of points by PI/4 due to reduced shape area
		if (!isCircle)
		{
			const double Pi_4 = 0.785398163397448309616; // PI/4
			numPoints = static_cast<int>(Pi_4 * numPoints);
		}

		if (minDist < 0.0f)
		{
			minDist = sqrt(float(numPoints)) / float(numPoints);
		}

		std::vector<Point> samplePoints;
		std::vector<Point> processList;

		if (!numPoints)
			return samplePoints;

		// create the grid
		const float cellSize = minDist / sqrt(2.0f);

		const int gridW = (int)ceil(1.0f / cellSize);
		const int gridH = (int)ceil(1.0f / cellSize);

		Grid grid(gridW, gridH, cellSize);

		Point firstPoint;
		do {
			firstPoint = Point(generator.randomFloat(), generator.randomFloat());
		} while (!(isCircle ? firstPoint.isInCircle() : firstPoint.isInRectangle()));

		// update containers
		processList.push_back(firstPoint);
		samplePoints.push_back(firstPoint);
		grid.insert(firstPoint);

		// generate new points for each point in the queue
		while (!processList.empty() && samplePoints.size() <= numPoints)
		{
			const Point point = popRandom<PRNG>(processList, generator);

			for (uint32_t i = 0; i < newPointsCount; i++)
			{
				const Point newPoint = generateRandomPointAround(point, minDist, generator);
				const bool canFitPoint = isCircle ? newPoint.isInCircle() : newPoint.isInRectangle();

				if (canFitPoint && !grid.isInNeighbourhood(newPoint, minDist, cellSize))
				{
					processList.push_back(newPoint);
					samplePoints.push_back(newPoint);
					grid.insert(newPoint);
					continue;
				}
			}
		}

		return samplePoints;
	}
} // namespace PoissonGenerator