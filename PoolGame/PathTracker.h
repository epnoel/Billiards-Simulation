#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class PathTracker {

public:
	PathTracker(float ballRadius, int radius = 3, sf::Color circleColor = sf::Color::Green);

	void addPosition(sf::Vector2f pos);

	void clearAllVectors();

public:

	float ballRadius;
	int circleRadius;
	sf::Color circleColor;

	std::vector<std::pair<sf::Vector2f,sf::CircleShape>> pathPositions;
	std::vector<std::pair<sf::Vertex, sf::Vertex>> pathLines;
};