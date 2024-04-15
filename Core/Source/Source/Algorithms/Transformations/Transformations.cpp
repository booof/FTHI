#include "Transformations.h"
#include "Globals.h"

float Source::Algorithms::Transformations::transformStaticScreenCoordsX(float x)
{
    return Global::screenWidth * (0.5f + x / (Global::halfScalarX + Global::halfScalarX));
}

float Source::Algorithms::Transformations::transformStaticScreenCoordsY(float y)
{
    return Global::screenHeight * (0.5f + y * 0.01f);
}

float Source::Algorithms::Transformations::transformStaticScreenWidth(float width)
{
    return Global::screenWidth * width / (Global::halfScalarX + Global::halfScalarX);
}

float Source::Algorithms::Transformations::transformStaticScreenHeight(float height)
{
    return Global::screenHeight * height * 0.01f;
}

float Source::Algorithms::Transformations::transformRelativeScreenCoordsX(float x)
{
    return Global::screenWidth * (0.5f + x / (Global::halfScalarX));
}

float Source::Algorithms::Transformations::transformRelativeScreenCoordsY(float y)
{
    return Global::screenHeight * (0.5f + y * 0.02f);
}

float Source::Algorithms::Transformations::transformRelativeScreenWidth(float width)
{
    return Global::screenWidth * width / (Global::halfScalarX);
}

float Source::Algorithms::Transformations::transformRelativeScreenHeight(float height)
{
    return Global::screenHeight * height * 0.02f;
}
