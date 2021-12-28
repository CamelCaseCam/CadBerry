CadBerry/ImGui/ImGuiLayer.h
###########################

Imports
=======
* :doc:`/API/CadBerry/Layer.h`
* :doc:`/API/CadBerry/Core.h`
* :doc:`/API/CadBerry/Events/ApplicationEvent.h`
* :doc:`/API/CadBerry/Events/KeyEvent.h`
* :doc:`/API/CadBerry/Events/MouseEvent.h`
* imgui.h

ImGuiLayer class
================
Inherits from :doc:`CDB::Layer </API/CadBerry/Layer.h>`

Functions
---------
Constructor
^^^^^^^^^^^
Sets the layer name. 

virtual void OnAttach()
^^^^^^^^^^^^^^^^^^^^^^^
1. Sets up ImGui
2. Enables keyboard, docking, and viewports
3. Calls the InitImGui function on each module

virtual void OnDetach()
^^^^^^^^^^^^^^^^^^^^^^^
Cleans up and deletes the ImGui context

virtual void OnImGuiRender()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Empty function. Does nothing

void Begin()
^^^^^^^^^^^^
Creates a new (ImGui) frame

void End()
^^^^^^^^^^
Renders the ImGui draw data

Fields
------
ImGuiContext* Context
^^^^^^^^^^^^^^^^^^^^^
A pointer to the ImGui context

float m_Time = 0.0f
^^^^^^^^^^^^^^^^^^^
As far as I can tell, this is an unused variable. I will be testing in the future to see if it does anything