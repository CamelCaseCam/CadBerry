CadBerry/BuildEngine/BuildEngine.h
##################################

Imports
=======
* :doc:`/API/cdbpch.h`

BuildEngine class
=================
Interface for CadBerry build engine. 

Functions
---------
Constructor(std::vector<std::string> buildableTypes, std::vector<std::string> outputTypes)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Gets the source file extensions (for example, "gil") and the output file extensions (for example, "fasta")

std::vector<std::string>* GetBuildableTypes()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns a pointer to a vector of valid source file extensions

std::vector<std::string>* GetOutputTypes()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns a pointer to a vector of valid output file types

virtual void PreBuild(std::string& path, std::string& PreBuildDir)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Precompiles a file with one of the valid source file extensions to an intermediate file in PreBuildDir. PreBuildDir is an absolute path. 

virtual void Build(std::string& path, std::string& EntryPoint, std::string& PreBuildDir, std::string& OutputDir, std::string& OutputType)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Builds the project with path as the path to the project, EntryPoint as the path relative to path to the project's entry point, OutputDir as the 
directory for the compiled output, and OutputType as the project's output file type.

Fields
------
std::vector<std::string> BuildableTypes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The valid source file extensions

std::vector<std::string> OutputTypes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The valid output file types