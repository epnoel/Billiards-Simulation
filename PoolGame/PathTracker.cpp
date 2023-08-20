#include "PathTracker.h"

PathTracker::PathTracker(float ballRadius, int radius, sf::Color circleColor)
{
	this->ballRadius = ballRadius;
	this->circleRadius = radius;
	this->circleColor = circleColor;
}

void PathTracker::addPosition(sf::Vector2f pos)
{
	if (pathPositions.size() > 1)
	{
		sf::Vertex vertex1(pathPositions.back().first);
		sf::Vertex vertex2(pos);

		pathLines.push_back({ vertex1, vertex2 });

	}

	sf::CircleShape circle(circleRadius);
	circle.setPosition({ pos.x, pos.y});
	circle.setFillColor(circleColor);

	pathPositions.push_back({ pos, circle });


}

void PathTracker::clearAllVectors()
{
	pathLines.clear();
	pathPositions.clear();
}