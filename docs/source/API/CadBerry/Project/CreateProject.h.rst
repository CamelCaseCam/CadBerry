CadBerry/Project/CreateProject.h
################################

Imports
=======
* :doc:`/API/CadBerry/Layer.h`
* nfd.h (native file dialog library, library to open file selection dialog)

ProjectCreationLayer class
==========================
Inherits from :doc:`CDB::Layer </API/CadBerry/Layer.h>`

Functions
---------
virtual void OnImGuiRender()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Renders ImGui elements. This function controls the project creation window

virtual void OnAttach()
^^^^^^^^^^^^^^^^^^^^^^^
Called when the layer is attached

Fields
------
char ProjectName[64]
^^^^^^^^^^^^^^^^^^^^
Char buffer for project name. In the future, this will be updated to use std::string

nfdchar_t* Path
^^^^^^^^^^^^^^^
Char array returned by nfd (nfd is a C library). In the future, I will change this to use scoped_ptr