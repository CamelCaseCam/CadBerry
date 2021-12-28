CadBerry/Module/Viewport.h
##########################
An ImGui window with some other useful functions

Imports
=======
* :doc:`/API/cdbpch.h`
* :doc:`/API/CadBerry/Core.h`
* :doc:`/API/CadBerry/Layer.h`
* :doc:`/API/CadBerry/Utils/memory.h`

Viewport class
==============
Functions
---------
Constructor(std::string name)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sets the name of the viewport

virtual void Start()
^^^^^^^^^^^^^^^^^^^^
Called when the viewport is created

virtual void Draw()
^^^^^^^^^^^^^^^^^^^
Called once per frame before ImGui rendering (designed for OpenGL rendering to the default RenderTarget)

virtual void GUIDraw()
^^^^^^^^^^^^^^^^^^^^^^
Called once per frame to draw GUI elements. GUI elements drawn here will affect the viewport, not the main editor. I don't have an official way to 
draw GUI elements to the main editor from inside a viewport, but you could try calling ImGui functions from Draw()

virtual void Update(float dt)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Called once per frame and gives the delta time since the last frame. 

virtual void OnClose()
^^^^^^^^^^^^^^^^^^^^^^
Called when the user closes the viewport. 

Fields
------
std::string Name
^^^^^^^^^^^^^^^^
The name of the viewport

bool IsOpen
^^^^^^^^^^^
Is the window open? If you set this to false, the window will close

bool Background
^^^^^^^^^^^^^^^
Is the window visible?

ViewportLayer class
===================
Handles viewport rendering. Inherits from :doc:`CDB::Layer </API/CadBerry/Layer.h>`

Functions
---------
virtual void OnImGuiRender()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
1. Creates (most of) the CadBerry editor window (this should be moved)
2. Creates any viewports
3. Updates viewports
4. Creates viewport windows
5. Calls GUIDraw functions
6. Deletes any closed viewports

virtual void Draw()
^^^^^^^^^^^^^^^^^^^
Calls each of the viewports' Draw functions

Destructor
^^^^^^^^^^
Closes all open viewports and deletes them

void ViewportLayer::AddViewport(Viewport* viewport)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Adds a viewport. If you (for some reason) want to prevent the user from closing your viewport, you could theoretically use std::move in OnClose() 
to move the viewport into a new viewport and add that. The only problem is that doing that would freeze the window when it's closed. Maybe you 
could use a WindowCloseEvent and delete this?

Fields
------
std::vector<Viewport*> OpenViewports
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The open viewports