#ifndef TEXTURE_WIZARD_H
#define TEXTURE_WIZARD_H

// Welcome to the Texture Wizard!
// This Object Was Originally Intended to Provide a Way to Allow Users to Select Textures Using GUI
// It Has Since Been Adapted to Provide A Method to Edit and Generate Textures
// The Ability to Choose Textures Has be Delegated to the File Wizard Object
// This Texture Editor Can Edit the Colors and Alpha of a Texture Similar to Microsoft Paint
// This Texture is a 4 Chanel, 1 Byte Per Chanel Texture With Pixel Dimensions That can be Specified in the Editor
// In Addition, It Also Includes the Ability to Modify the Material Values of the Material Texture Similarly
// The User is Allowed to create a Lighting Scene Above the Texture to Similate How Lighting Affects the Material
// In Addition, There is an Option to Grayscale the Material Texture to Easily See the Values of Individual Material Values 
// This Texture Editor Also Allows the Ability to Create a 3D scene to Modify the Parallax Map
// 3D Objects, Such as Models and Shapes, As Well as 2D Objects Can be Created to Serve as This 3D Scene
// These Objects, Including the 3D Objects, Will be Created and Modified Using a New and Specialized Editor Window
// There Will be a Special Editor to Allow Modification of Objects in a 3D Space, including a special Selector3D, Camera3D, and EditorWindow3D
// These Objects and Models Can be Moved and Animated for Easy Creation of Animated Textures
// There Will be No Rigid 2D Shapes, Only Vertices of a Quadrilateral, Polygon, or Triangle That Can be Moved
// All 3D Shapes Will be Imported Models and Will be Rigid Except for Models With a Skeleton Attached
// This 3D Scene Also Allows the Use of Lights Directed From the Texture to Observe the Material of the Texture
// The Texture in this 3D Sceen is Seen as a Semi-Transparent Rectangle With a Normal Showing the Direction the Texture Will Copy
// Press an Arbitrary Button to be Able to View the Scene From the Textures POV
// The Texture Can be Moved Around the Scene to Get Different Angles
// While the Size of the Texture Can be Changed Dynamically in the 3D Scene, The Internal Pixel Dimensions of the Texture Will Not be Affected
// To Save a Texture of this Scene, Press a Button That Will Convert the Captured Pixels Into a Color Texture and a Parallax Map
// This Pixel Mapping Will Probably Involve Some Form of Ray Tracing
// The 3D Scene Can be Saved to a File and the Texture can be Saved to a File
// If a 3D Space is Not to be Used, There is Another Editor Feature That Allows For Parallax Mapping to be Created in 2D by Creating Height Maps Ontop of the Texture
// The Texture Should Implement the New and Fast Method of Compressing Files (Forgot the Name)

// 3D Editor Controls:
// Use Mouse to Controll a 3D Camera, WS to Move Backwards / Forwards, AD to Move Left and Right in Parallel Plane, and Shift/Space to Move Up/Down in Parallel Plane
// If Center of Mouse is Pointed at an Object (There Will be a Crosshare That Indicates the Center), an Object Can be Selected
// Left Click on Body of Object To Move the Entire Thing, Left Click on Vertex to Move Only the Vertex
// Drag Mouse to Move Object in Parallel Plane, Scroll to Move Object Front or Back
// These Objects Should Also be Easily Rotated When Selected at the Press of a Button (Press Shift to Shift Rotating)
// This Should use the Same Mouse Inputs as Moving the Object
// Right Click on 2D Objects to Create Editor Window to Edit the Color, Texture, and Normals of Object
// Right Click on a 2D Vertex to Set Its Position
// Right Click on 3D Objects to Select the Model to Use and Skeleton to Apply
// 3D Objects Can Only be Interacted With as a Point in the Object's Object Space
// Right Clicking on a Vertex or Object Can Also Set Its Animation, Which Can Be Applied as a Time Function or Animation File
// Each Animation Will be Controlled by a DeltaTime Variable, Which Can be Controlled By the GUI

#endif
