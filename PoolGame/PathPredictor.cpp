#include "PathPredictor.h"

PathPredictor::PathPredictor(Ball targetBall, std::vector<Ball> balls, std::vector<sf::Vertex> tangentLine, std::vector<std::vector<sf::Vector2f>> contactTableLines, std::vector<sf::Vector2f> allTableCircles, 
								float circleRadii, sf::Vector2i screenSize)
{

	pathPoints.push_back(targetBall.position);
	sf::Vector2f tLine = { tangentLine[1].position.x - tangentLine[0].position.x, tangentLine[1].position.y - tangentLine[0].position.y };
	float magTLine = sqrtf(powf(tLine.x, 2) + powf(tLine.y, 2));
	tLine.x /= magTLine;
	tLine.y /= magTLine;

	float numberOfPoints = 3;

	for (int i = 0; i < numberOfPoints; i++) {


		double slope = tLine.y / tLine.x;
		double yIntercept = (double)pathPoints.back().y - slope * (double)pathPoints.back().x;

		sf::Vector2f thePoint = { 0.f,0.f };
		double shortestDistance = (double)INT_MAX;
		// sf::Vector2f newVelocity = { 0.f,0.f };

		sf::Vector2f boundEndPoints = getBounds(pathPoints.back(), tLine, screenSize);
		std::vector<sf::Vector2f> bounds = { {std::min(pathPoints.back().x, boundEndPoints.x), std::min(pathPoints.back().y, boundEndPoints.y)},
											 {std::max(pathPoints.back().x, boundEndPoints.x), std::max(pathPoints.back().y, boundEndPoints.y)} };

		std::vector<sf::Vector2f> theLine = { {0,0}, {0,0} };

		// Debug
		//std::cout << "tLine = (" << tLine.x << "," << tLine.y << ")" << std::endl;
		//std::cout << "slope = " << slope << std::endl;
		//std::cout << "yIntercept = " << yIntercept << std::endl;
		//std::cout << "targetBall.position = (" << pathPoints.back().x << "," << pathPoints.back().y << ")" << std::endl;
		//std::cout << "boundEndPoints = (" << boundEndPoints.x << "," << boundEndPoints.y << ")" << std::endl;
		//std::cout << "contactTableLines.size() = " << contactTableLines.size() << std::endl;
		//std::cout << "bounds = (" << bounds[0].x << "," << bounds[0].y << ") and (" << bounds[1].x << "," << bounds[1].y << ")" << std::endl;

		bool ballIntersection = false;
		bool circleHoleIntersection = false;

		for (auto ball : balls)
		{

			if (ball.position == targetBall.position)
			{
				continue;
			}

			double a, b, c;
			a = 1.0 + pow(slope, 2);
			b = 2.0 * (slope * (yIntercept - ball.position.y) - ball.position.x);
			c = pow(ball.position.x, 2) + pow(yIntercept - ball.position.y, 2) - pow(ball.radius * 2.f, 2); // added ball1.radius

			double remainder = pow(b, 2) - 4.0 * a * c;

			if (remainder >= 0)
			{
				float x1, x2, yEnd;
				sf::Vector2f normalEnds, tangentEnds;

				x1 = (float)((-b - sqrt(remainder)) / (2.0 * a));
				x2 = (float)((-b + sqrt(remainder)) / (2.0 * a));

				float yEnd1 = (float)slope * x1 + (float)yIntercept;
				float yEnd2 = (float)slope * x2 + (float)yIntercept;

				float distance1 = sqrtf(powf(x1 - pathPoints.back().x, 2) + powf(yEnd1 - pathPoints.back().y, 2));
				float distance2 = sqrtf(powf(x2 - pathPoints.back().x, 2) + powf(yEnd2 - pathPoints.back().y, 2));


				sf::Vector2f theXY;

				if (distance1 < distance2) {
					theXY = { x1,yEnd1 };
				}
				else {
					theXY = { x2, yEnd2 };
				}

				if (theXY.x >= bounds[0].x && theXY.x <= bounds[1].x
					&& theXY.y >= bounds[0].y && theXY.y <= bounds[1].y)
				{

				}
				else {
					continue;
				}

				thePoint = theXY;

				ballIntersection = true;
				break;

			}
			else {
				continue;
			}
		}

		if (!ballIntersection) {
			for (auto line : contactTableLines)
			{

				double theX, theY;

				if (line[1].x == line[0].x)
				{
					if (tLine.x != 0.f)
					{
						theX = line[1].x;
						theY = slope * theX + yIntercept;
					}
				}
				else {
					double otherSlope, otherYIntercept;
					otherSlope = (line[1].y - line[0].y) / (line[1].x - line[0].x);
					otherYIntercept = line[1].y - otherSlope * line[1].x;

					theX = (yIntercept - otherYIntercept) / (otherSlope - slope);
					theY = slope * theX + yIntercept;
				}


				if ((float)theX >= bounds[0].x && (float)theX <= bounds[1].x
					&& (float)theY >= bounds[0].y && (float)theY <= bounds[1].y)
				{
					//std::cout << "BOUNDS: (theX, theY) = (" << theX << "," << theY << ")" << std::endl;
					//std::cout << "Line: (" << line[0].x << "," << line[0].y << ") and (" << line[1].x << "," << line[1].y << ")" << std::endl;
				}
				else {
					continue;
				}

				//std::cout << "(theX, theY) = (" << theX << "," << theY << ")" << std::endl;

				double threshold = 0.02;

				bool xPart = theX >= std::min(line[0].x, line[1].x) && theX <= std::max(line[0].x, line[1].x);
				bool yPart = theY >= std::min(line[0].y, line[1].y) - threshold && theY <= std::max(line[0].y, line[1].y) + threshold;

				// Debug
				//std::cout << "xPart" << xPart << "; ";
				//std::cout << "yPart" << yPart << std::endl;

				if (xPart && yPart)
				{

					// Debug
					//std::cout << "CALLED: Line: (" << line[0].x << "," << line[0].y << ") and (" << line[1].x << "," << line[1].y << ")" << std::endl;

					theX += -tLine.x * 1.f * targetBall.radius;
					theY += -tLine.y * 1.f * targetBall.radius;

					double currentDistance = sqrt(pow(theX - pathPoints.back().x, 2) + pow(theY - pathPoints.back().y, 2));
					if (currentDistance < shortestDistance)
					{
						shortestDistance = currentDistance;
						thePoint = { (float)theX, (float)theY };
						theLine = line;
					}
				}



			}
		}

		if (/*thePoint.x == 0.f && thePoint.y == 0.f*/ true)
		{

			if (!ballIntersection) {
				for (auto circle : allTableCircles)
				{
					double a, b, c;
					a = 1.0 + pow(slope, 2);
					b = 2.0 * (slope * (yIntercept - circle.y) - circle.x);
					c = pow(circle.x, 2) + pow(yIntercept - circle.y, 2) - pow(circleRadii, 2); // added ball1.radius

					double remainder = pow(b, 2) - 4.f * a * c;

					if (remainder >= 0)
					{
						float x1, x2, yEnd;
						sf::Vector2f normalEnds, tangentEnds;

						x1 = (float)((-b - sqrt(remainder)) / (2.0 * a));
						x2 = (float)((-b + sqrt(remainder)) / (2.0 * a));

						float yEnd1 = (float)slope * x1 + (float)yIntercept;
						float yEnd2 = (float)slope * x2 + (float)yIntercept;

						float distance1 = sqrtf(powf(x1 - pathPoints.back().x, 2) + powf(yEnd1 - pathPoints.back().y, 2));
						float distance2 = sqrtf(powf(x2 - pathPoints.back().x, 2) + powf(yEnd2 - pathPoints.back().y, 2));


						sf::Vector2f theXY;

						if (distance1 < distance2) {
							theXY = { x1,yEnd1 };
						}
						else {
							theXY = { x2, yEnd2 };
						}

						if (theXY.x >= bounds[0].x && theXY.x <= bounds[1].x
							&& theXY.y >= bounds[0].y && theXY.y <= bounds[1].y)
						{

						}
						else {
							continue;
						}

						if (thePoint.x == 0.f && thePoint.y == 0.f) {
							thePoint = theXY;
						}
						else {
							float distance1 = sqrtf(powf(theXY.x - pathPoints.back().x, 2) + powf(theXY.y - pathPoints.back().y, 2));
							float distance2 = sqrtf(powf(thePoint.x - pathPoints.back().x, 2) + powf(thePoint.y - pathPoints.back().y, 2));

							if (distance1 < distance2)
							{
								thePoint = theXY;
							}
							else {
								continue;
							}

						}

						circleHoleIntersection = true;
						break;

					}
					else {
						continue;
					}
				}
			}
		}

		if (!circleHoleIntersection && !ballIntersection) {
			sf::Vector2f reflectedTLine;
			sf::Vector2f normalizedNormalVector = { -(theLine[1].y - theLine[0].y),theLine[1].x - theLine[0].x };

			if (theLine[1].y - theLine[0].y == 0.f) {
				normalizedNormalVector.x = 0.f;
			}

			float normalVectorMagnitude = sqrtf(powf(normalizedNormalVector.x, 2) + powf(normalizedNormalVector.y, 2));
			normalizedNormalVector.x /= normalVectorMagnitude;
			normalizedNormalVector.y /= normalVectorMagnitude;
			float normalVelocityVectorDotProduct = normalizedNormalVector.x * tLine.x + normalizedNormalVector.y * tLine.y;
			reflectedTLine.x = tLine.x - 2.f * normalVelocityVectorDotProduct * normalizedNormalVector.x;
			reflectedTLine.y = tLine.y - 2.f * normalVelocityVectorDotProduct * normalizedNormalVector.y;

			tLine = reflectedTLine;
		}

		//float m = -normalizedNormalVector.x / normalizedNormalVector.y;

		float lineLength = sqrtf(powf(thePoint.x - pathPoints.back().x, 2) + powf(thePoint.y - pathPoints.back().y, 2));
		float dotProduct = ((thePoint.x - pathPoints.back().x) * 1.f + (thePoint.y - pathPoints.back().y) * 0.f) / lineLength;
		float crossProduct = ((thePoint.y - pathPoints.back().y) * 1.f + (thePoint.x - pathPoints.back().x) * 0.f) / lineLength;

		float tanTheta = atan2f(crossProduct, dotProduct);
		tanTheta *= (180.f / PI);

		float theThickness = 1.8f;

		sf::RectangleShape rectLine;
		rectLine.setSize({ lineLength, theThickness });
		rectLine.setOrigin({ 0.f, theThickness / 2.f });
		rectLine.setPosition(pathPoints.back());
		rectLine.setFillColor(sf::Color::Red);
		rectLine.rotate(tanTheta);

		theRectLines.push_back(rectLine);

		pathPoints.push_back(thePoint);

		if (circleHoleIntersection || ballIntersection)
		{
			break;
		}

	}

}

sf::Vector2f PathPredictor::getBounds(sf::Vector2f targetBallPosition, sf::Vector2f tangentLine, sf::Vector2i screenSize)
{
	if (tangentLine.x == 0)
	{
		if (tangentLine.y < 0.f)
		{
			return { targetBallPosition.x, 0.f };
		}
		else {
			return { targetBallPosition.x, (float)screenSize.y };
		}
	}
	else if (tangentLine.y == 0)
	{
		if (tangentLine.x < 0.f)
		{
			return { 0.f, targetBallPosition.y };
		}
		else {
			return { (float)screenSize.x, targetBallPosition.y };
		}
	}
	else if (tangentLine.x < 0.f)
	{
		if (tangentLine.y < 0.f)
		{
			return { 0.f,0.f };
		}
		else {
			return { 0.f, (float)screenSize.y };
		}
	}
	else if (tangentLine.x > 0.f)
	{
		if (tangentLine.y < 0.f)
		{
			return { (float)screenSize.x, 0.f };
		}
		else
		{
			return { (float)screenSize.x, (float)screenSize.y };
		}
	}
	
	return { 0.f,0.f };
}