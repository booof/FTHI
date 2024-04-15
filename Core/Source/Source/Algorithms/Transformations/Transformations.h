#pragma once
#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

namespace Source::Algorithms::Transformations
{
	// Convert Static X Position Into Screen Coordinates
	float transformStaticScreenCoordsX(float x);

	// Convert Static Y Position Into Screen Coordinates
	float transformStaticScreenCoordsY(float y);

	// Convert Static X Width Into Screen Width
	float transformStaticScreenWidth(float width);

	// Convert Static Y Height Into Screen Height
	float transformStaticScreenHeight(float height);

	// Convert Relative X Position Into Screen Coordinates
	float transformRelativeScreenCoordsX(float x);

	// Convert Relative Y Position Into Screen Coordinates
	float transformRelativeScreenCoordsY(float y);

	// Convert Relative X Width Into Screen Width
	float transformRelativeScreenWidth(float width);

	// Convert Relative Y Height Into Screen Height
	float transformRelativeScreenHeight(float height);
}

#endif
