CadBerry/Rendering/RenderCommand.h
##################################

Imports
=======
* :doc:`RendererAPI.h`
* glm/glm.hpp

RenderCommand class
===================
Used to call renderer commands

Functions
---------
static void SetClearColour(const glm::vec4& Colour)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Sets the screen's clear colour

static void Clear()
^^^^^^^^^^^^^^^^^^^
Clears the screen

static void DrawIndexed(const VertexArray* vertexArray)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Draws the vertex array

Fields
------
static RendererAPI* s_RendererAPI
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The API used by the commands