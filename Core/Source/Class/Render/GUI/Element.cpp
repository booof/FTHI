#include "Element.h"
#include "Render\Struct\DataClasses.h"

Render::GUI::ElementData& DataClass::Data_Element::getElementData()
{
    return element_data;
}

int& DataClass::Data_Element::getScript()
{
    return element_data.script;
}

glm::vec2& DataClass::Data_Element::getPosition()
{
    return element_data.position;
}

void DataClass::Data_Element::updateTraveresPositionHelper(float deltaX, float deltaY)
{
    element_data.position.x += deltaX;
    element_data.position.y += deltaY;
}

void DataClass::Data_Element::setInfoPointers(int& index1, int& index2, int& index3, glm::vec2** position1, glm::vec2** position2, glm::vec2** position3)
{
    // Position is at Index 2
    *position1 = &element_data.position;
    index1 = 2;

    // No Other Positions are Used
    position23Null(index2, index3, position2, position3);
}

void Render::GUI::Element::updateObject()
{
}

glm::vec2* Render::GUI::Element::pointerToPosition()
{
    return &element_data.position;
}

glm::vec2 Render::GUI::Element::returnPosition()
{
    return element_data.position;
}

void Render::GUI::Element::updateSelectedPosition(float deltaX, float deltaY)
{
    // Update Base Element Position
    element_data.position.x += deltaX;
    element_data.position.y += deltaY;
}

uint8_t Render::GUI::Element::getElementType()
{
    return element_data.element_type;
}
