CadBerry viewport API
=====================
Class: CDB::Viewport
Location: CadBerry\Module\Viewport.h

## Methods
### void Start()
Function called when viewport is created

### void Draw()
Called once per frame, before ImGui rendering. This function is intended for OpenGL rendering to the default RenderTarget. 

### void GUIDraw
Called once per frame for rendering GUI elements. If you have your own RenderTarget, this is where you call RenderTarget.Draw().

In order to use any ImGui functions, you **must** add the UseImGui macro to your module class. This overrides the InitImGui function and uses it 
to set the ImGui context across dll boundaries. If you don't do this, you'll get a weird assert error from ImGui

### void Update(float dt)
This function is called once per frame and provides the delta time since the last frame. 

### void OnClose()
This function is called whenever the window is closed (most of the time this will be because the user closed the window, but it can be closed by 
the application). 

You can use this function to save any open files and clean up before the window's destructor is called. This function **is** called when the 
application closes if the window is still open. 

## Fields
### std::string CDB::Viewport::Name
The name of the viewport

### bool CDB::Viewport::IsOpen
Bool containing whether the window is open. You can set this to false yourself to close the window

### bool CDB::Viewport::Background
This variable is used by ImGui to keep track of whether a window is visible. I'm not sure what would happen if you messed with this, I've never 
tried it. 