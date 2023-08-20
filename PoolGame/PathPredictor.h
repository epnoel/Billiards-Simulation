#pragma once
#include <SFML/Graphics.hpp>
#include "Ball.h"
#include <vector>


class PathPredictor {
public:

	PathPredictor(Ball targetBall, std::vector<Ball> balls, std::vector<sf::Vertex> tangentLine, std::vector<std::vector<sf::Vector2f>> contactTableLines, std::vector<sf::Vector2f> allTableCircles, float circleRadii,
		sf::Vector2i screenSize);

	sf::Vector2f getBounds(sf::Vector2f targetBallPosition, sf::Vector2f tangentLine, sf::Vector2i screenSize);
public:

	std::vector<std::vector<sf::Vector2f>> contactTableLines;
	std::vector<sf::Vector2f> allTableCircles;

	std::vector<sf::Vector2f> pathPoints;
	std::vector<sf::RectangleShape> theRectLines;

};
