#ifndef ADVENTURE_COMMANDER_TWIST_HPP
#define ADVENTURE_COMMANDER_TWIST_HPP

#include <random>
#include <iostream>
#include <algorithm>
#include <functional>

#include "point.hpp"

class Twist {
private:
	std::mt19937 mt;

public:
	Twist(Twist const&) = delete;
	void operator=(Twist const&) = delete;

	/**
	 * Generates a float between [0.0, 1.0)
	 * @return a random float
	 */
	static double rand() {
		return rand(1.0);
	}

	/**
	 * Generates an int between [0, max]
	 * @param max The maximum integer (Closed)
	 * @return a random int
	 */
	static int rand(int max) {
		return rand(0, max);
	}

	/**
	 * Generates a double between [0, max)
	 * @param max The maximum double (Open)
	 * @return a random double
	 */
	static double rand(double max) {
		return rand(0.0, max);
	}

	/**
	 * Generates a random point within the specified
	 * dimension from ( [0, x], [0, y] )
	 * @param p The maximum point (Closed)
	 * @return a random point
	 */
	static Point rand(const Point& max) {
		return rand(Point(), max);
	}

	/**
	 * Generates an int between [min, max]
	 * @param min The minimum integer (Closed)
	 * @param max The maximum integer (Closed)
	 * @return a random int
	 */
	static int rand(int min, int max) {
		return std::uniform_int_distribution<>(min, max)(self().mt);
	}

	/**
	 * Generates a double between [min, max)
	 * @param min The minimum double (Closed)
	 * @param max The maximum double (Open)
	 * @return a random double
	 */
	static double rand(double min, double max) {
		return std::uniform_real_distribution<>(min, max)(self().mt);
	}

	/**
	 * Generates a random point within the specified
	 * dimension from ( [min.x, max.x], [min.y, max.y] )
	 * @param min The minimum point (Closed)
	 * @param max The maximum point (Closed)
	 * @return a random point
	 */
	static Point rand(const Point& min, const Point& max) {
		return Point(rand(min.x, max.x), rand(min.y, max.y));
	}

	/**
	 * Generates a random number based on geometric
	 * probability. The higher `probability`, the
	 * higher chance to generate lower numbers.
	 * @param probability A geometric probability
	 * @return a random int
	 */
	static int geom(double probability) {
		return std::geometric_distribution<>(probability)(self().mt);
	}

	/**
	 * Clamps a geometric sequence between min and max
	 * @param probability A geometric probability
	 * @param min Lower bound (Closed)
	 * @param max Upper bound (Closed)
	 * @return a random int
	 */
	static int geom(double probability, int min, int max) {
		return std::min(geom(probability) + min, max);
	}

	/**
	 * Seeds the internal engine
	 * @param str A string, like how Minecraft seeds it's worlds.
	 */
	static void seed(const std::string& str) {
		std::seed_seq seed (str.begin(), str.end());
		self().mt = std::mt19937(seed);
	}

	/**
	 * Gets the internal engine for external use.
	 * @return The internal mt19937 engine.
	 */
	static std::mt19937 getTwister() {
		return self().mt;
	}

private:
	Twist() {
		//Apparently you can't trust random_device to
		//actually give you a random source of numbers...
		//Seems to work correctly on my machine though.
		std::random_device source;
		std::mt19937::result_type data[std::mt19937::state_size];
		generate(std::begin(data), std::end(data), ref(source));
		std::seed_seq seeds(std::begin(data), std::end(data));
		mt = std::mt19937(seeds);
	}

	static Twist& self() {
		static Twist twist;
		return twist;
	}
};

#endif
