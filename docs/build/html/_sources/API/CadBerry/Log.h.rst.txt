CadBerry/Log.h
##############
CadBerry logging macros using spdlog

Imports
=======
* :doc:`/API/cdbpch.h`
* :doc:`/API/CadBerry/Core.h`
* spdlog/spdlog.h
* spdlog/sinks/stdout_color_sinks.h
* spdlog/fmt/ostr.h

Log class
=========

Functions
---------

static std::shared_ptr<spdlog::logger>& GetEditorLogger()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns a reference to a shared pointer to the editor logger. In the future, I will change this to use scoped_ptr and raw pointers

static std::shared_ptr<spdlog::logger>& GetBuildLogger()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns a reference to a shared pointer to the build engine logger. In the future, I will change this to use scoped_ptr and raw pointers

static void Init()
^^^^^^^^^^^^^^^^^^
Initializes the logger. This function is called by CadBerry when the editor is opened. 

Fields
------
static std::shared_ptr<spdlog::logger> EditorLogger
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Shared pointer to the editor logger. In the future, I will change this to use scoped_ptr

static std::shared_ptr<spdlog::logger> BuildLogger
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Shared pointer to the build engine logger. In the future, I will change this to use scoped_ptr

Macros
======
CDB_EditorFatal
---------------
Prints a fatal error to the console

CDB_EditorError
---------------
Prints an error to the console

CDB_EditorWarning
-----------------
Prints a warning to the console

CDB_EditorInfo
--------------
Prints an info-level message to the console (green text)

CDB_EditorTrace
---------------
Prints a trace message to the console

CDB_BuildFatal
--------------
Prints a fatal error from the build logger (will be changed in the future so the build engine can't crash CadBerry)

CDB_BuildError
--------------
Prints an error from the build logger

CDB_BuildWarning
----------------
Prints a warning from the build logger

CDB_BuildInfo
-------------
Prints an info-level message from the build logger

CDB_BuildTrace
--------------
Prints a trace message from the build logger