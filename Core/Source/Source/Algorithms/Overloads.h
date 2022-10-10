// Test If Materials Are Equal
bool operator==(const Material& mat1, const Material& mat2)
{
	// Compare All Results of Material
	bool ambient = mat1.Ambient == mat2.Ambient;
	bool specular = mat1.Specular == mat2.Specular;
	bool shininess = mat1.Shininess == mat2.Shininess;

	// Return Boolean
	return (ambient && specular && shininess);
}

// Test if Textures Are Equal
bool operator==(const Texture& texture1, const Texture& texture2)
{
	// Compare Bound Textures
	bool equal = texture1.texture == texture2.texture;

	// Return Boolean
	return (equal);
}

// Compare Z Positions of Formerground
bool operator>(const Formerground& object1, const Formerground& object2)
{
	// Compare Z of Objects
	bool greater_than = object1.zPercent > object2.zPercent;

	// Return Boolean
	return (greater_than);
}

// Compare Z Positions of Foreground
bool operator>(const Foreground& object1, const Foreground& object2)
{
	// Compare Z of Objects
	bool greater_than = object1.zPercent > object2.zPercent;

	// Return Boolean
	return (greater_than);
}

// Compare Z Positions of Background
bool operator>(const Background& object1, const Background& object2)
{
	// Z Boolean Variable
	bool greater_than = false;

	// Layer of Object 1 is Greater Than that of Object2
	if (object1.Layer < object2.Layer)
	{
		greater_than = true;
	}

	// Layer of Object 2 is Equal to that of Object2
	else if (object1.Layer == object2.Layer)
	{
		greater_than = object1.zPercent > object2.zPercent;
	}

	// Return Boolean
	return (greater_than);
}

// Compare Z Positions of Backdrop
bool operator>(const Backdrop& object1, const Backdrop& object2)
{
	// Compare Z of Objects
	bool greater_than = object1.zPercent > object2.zPercent;

	// Return Boolean
	return (greater_than);
}

// Test if Objects are the Same Object
bool operator==(const Object_Identifier& object1, const Object_Identifier& object2)
{
	return object1.levelX == object2.levelX && object1.levelY == object2.levelY && object1.Object_Type == object2.Object_Type && object1.Object_Number == object2.Object_Number;
}

// Test if Point is Inside Editing Box
bool operator==(const glm::vec2& Point, const box& Box)
{
	return Point.x > Box.left_x && Point.x < Box.right_x && Point.y > Box.bottom_y && Point.y < Box.top_y;
}

// Compare Values of Named Nodes
bool operator==(const Named_Node node1, const Named_Node node2)
{
	return node1.value == node2.value;
}

// Compare Values of Named Node
bool operator>(const Named_Node node1, const Named_Node node2)
{
	return node1.value > node2.value;
}

// Compare Values of Named Nodes
bool operator<(const Named_Node node1, const Named_Node node2)
{
	return node1.value < node2.value;
}

// Compare Values of Named Nodes
bool operator==(const Named_Node node1, const float value)
{
	return node1.value == value;
}

// Compare Values of Named Node
bool operator>(const Named_Node node1, const float value)
{
	return node1.value > value;
}

// Compare Values of Named Nodes
bool operator<(const Named_Node node1, const float value)
{
	return node1.value < value;
}

// Compare Values of Named Nodes
bool operator==(const float value, const Named_Node node1)
{
	return node1.value == value;
}

// Compare Values of Named Node
bool operator>(const float value, const Named_Node node1)
{
	return value > node1.value;
}

// Compare Values of Named Nodes
bool operator<(const float value, const Named_Node node1)
{
	return value < node1.value;
}

