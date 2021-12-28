CadBerry/Module/Module.h
########################

Imports
=======
* :doc:`/API/cdbpch.h`
* :doc:`/API/CadBerry/Core.h`
* :doc:`Viewport.h`
* imgui.h

Module class
============
Functions
---------
virtual std::string* GetViewportNames()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns a pointer to an array (of size NumViewports) of std::strings that are the names of each viewport added by this module

virtual Viewport* CreateViewport(std::string Name)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Creates a viewport based on its name (I will change this to use reference in the future)

virtual void InitImGui(ImGuiContext* Context)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
You (probably) shouldn't be messing with this function. Because global variables aren't preserved across dll boundaries, this function exists so 
you can set the ImGuiContext. Like I said, you don't need to mess with this function. This header defines a macro (UseImGui) that handles this for 
you. Just add that somewhere in your class, and you're set. In the future, I might change this to be a non-virtual function. 

Constructor(std::string name)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sets the module's name

Fields
------
int NumViewports
^^^^^^^^^^^^^^^^
The number of viewports added by the module

std::string ModuleName
^^^^^^^^^^^^^^^^^^^^^^
The name of the module

Defines
=======
UseImGui
--------
Defined as: virtual void InitImGui(ImGuiContext* Context) {ImGui::SetCurrentContext(Context);}

Sets the ImGui context across dll boundaries so you can use ImGui