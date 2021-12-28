CadBerry/Application.h
######################
CadBerry's main application class

Imports
=======
* :doc:`/API/CadBerry/Core.h`
* :doc:`/API/CadBerry/Window.h`
* :doc:`/API/CadBerry/Events/ApplicationEvent.h`
* :doc:`/API/CadBerry/LayerStack.h`
* :doc:`/API/CadBerry/ImGui/ImGuiLayer.h`
* :doc:`/API/CadBerry/Project/Project.h`
* :doc:`/API/CadBerry/Threading/ThreadPool.h`
* :doc:`/API/CadBerry/Module/Viewport.h`
* :doc:`/API/CadBerry/Module/Module.h`
* :doc:`/API/CadBerry/BuildEngine/BuildEngine.h`
* :doc:`/API/CadBerry/Rendering/Shader.h`
* :doc:`/API/CadBerry/Rendering/Buffer.h`
* :doc:`/API/CadBerry/Rendering/VertexArray.h`
* :doc:`/API/CadBerry/Utils/memory.h`

Application class
=================
Functions
---------
Constructor
^^^^^^^^^^^
1. Initializes the log
2. Creates a new GuiLayer and thread pool
3. Loads the modules from the Modules folder
4. Loads the BuildEngine from the build folder

Destructor
^^^^^^^^^^
Calls the thread pool's CompleteTasksAndDelete function which runs any queued tasks and then deletes the thread pool. Everything else is 
automatically deleted by scoped_ptr's destructor

void GetProject()
^^^^^^^^^^^^^^^^^
Either loads an existing project from the disk or prompts the user to create a new one. 

void StartEditor()
^^^^^^^^^^^^^^^^^^
Prints that CadBerry is starting and calls the private Main function

void OnEvent(Event& e)
^^^^^^^^^^^^^^^^^^^^^^
Dispatches the event to the layers until one of them sets Event.IsHandled to true

void PushLayer(Layer* layer)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Pushes a layer to the LayerStack

void PushOverlay(Layer* overlay)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Pushes an overlay to the LayerStack

void PrecompileFile(std::string Path, std::string PreBuildPath)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Instructs the build engine to precompile the file at the local path Path, with PreBuildPath as the output directory

static Application& Get()
^^^^^^^^^^^^^^^^^^^^^^^^^
Returns a reference to the application

Window& GetWindow()
^^^^^^^^^^^^^^^^^^^
Returns the open window

void AddViewport(Viewport* viewport)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Adds a viewport to the viewport layer

void Main()
^^^^^^^^^^^
The core editor loop. Main does the following things:

1. Sets PreBuildDir to the absolute path to the prebuild directory
2. Creates the editor window
3. Initializes the ImGuiLayer and the ViewportLayer
4. Enters the core editor loop

The core editor loop does this:

1. Calls the window's OnUpdate function
2. Calls each of the layers' OnUpdate functions
3. Sets up the next frame (using ImGui)
4. Calls each of the layers' Draw functions
5. Calls each of the layers' OnImGuiRender functions
6. Shows the build dialog if the build dialog is active
7. Cleans up, swaps buffers, and checks if it needs to exit

void CheckExit()
^^^^^^^^^^^^^^^^
CadBerry can't exit in the middle of a frame because that would cause problems, so when something tries to exit the window it sets ShouldExit to 
true. CheckExit is called at the end of the frame, and exits the window if ShouldExit is true. 

bool WindowCloseEvent(WindowCloseEvent& e)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Handles WindowCloseEvents by setting ShouldExit to true. 

Fields
------
scoped_ptr<Project> OpenProject
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Scoped pointer to the currently open project

scoped_ptr<BuildEngine> m_BuildEngine
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Scoped pointer to the build engine. The build engine should exist for the lifetime of the program, but using scoped_ptr makes it clear thet it's 
owned by the application class

std::vector<Module*> Modules
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
A vector of all the modules

std::string PathToEXE
^^^^^^^^^^^^^^^^^^^^^
The path to the executable, given by the WhereAmI library

int DirNameLength
^^^^^^^^^^^^^^^^^
The length of PathToEXE, WhereAmI gives the path as a non null terminated char array, so we have to convert it to a string

std::string PreBuildDir
^^^^^^^^^^^^^^^^^^^^^^^
The absolute path to the prebuild directory

bool ShouldExit
^^^^^^^^^^^^^^^
Should the application exit at the end of this frame?

bool NewProject
^^^^^^^^^^^^^^^
should we create a new project or check if one already exists

bool ShowBuildWindow
^^^^^^^^^^^^^^^^^^^^
Should we display the build window

ImGuiLayer* GuiLayer
^^^^^^^^^^^^^^^^^^^^
Pointer to the GUI layer (owned by the layer stack)

ViewportLayer* Viewports
^^^^^^^^^^^^^^^^^^^^^^^^
Pointer to the viewport layer (owned by the layer stack)

ThreadPool* m_ThreadPool
^^^^^^^^^^^^^^^^^^^^^^^^
Pointer to the thread pool. The thread pool is owned by itself. Don't create a thread pool using new, instead use ThreadPool::Get(). If one 
doesn't exist, it will create it for you. Likewise, never delete the thread pool. Instead, call CompleteTasksAndDelete. This prevents new tasks 
from being added and waits for all queued tasks to be completed and deletes itself. 

static Application* s_Instance
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Global application instance

Window* EditorWindow
^^^^^^^^^^^^^^^^^^^^
A pointer to the editor window. Using a raw pointer here might seem a little iffy, but EditorWindow lives for the lifetime of the program and has 
to be deleted before the program can exit. If EditorWindow leaks, the program's still running. 