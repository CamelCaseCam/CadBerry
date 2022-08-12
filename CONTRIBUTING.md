# Contributing to CadBerry
____________________________
First things first, thank you for taking the time to contribute. Below are some guidelines and information on how to get started. These are only guidelines, so if you disagree with any of them, feel free to open an issue or PR on GitHub. 

## What you should know
____________________________

### Modularity
CadBerry is designed to be as modular as possible. Because of this, almost every component of CadBerry is contained in a shared library that is loaded at runtime. 

The core CadBerry editor (`CadBerry/CadBerry/`) loads the shared libraries from the `Modules/` directory and the build engine from the `Build/` directory. It then handles input and output and provides a rendering API and a couple of other things that are used across all the modules.

GILBuildEngine contains the build engine that translates GIL files into DNA. While CadBerry is designed to use GIL, you *can* write your own build engine and use it with CadBerry.

The Core CadBerry module contains every viewport built-in to CadBerry. This includes, but is not limited to:
 * DNA Editor - Provides DNA manipulation that can't easily be built into GIL (ex. reverse complement, complement, etc.)
 * Modelling - A modelling framework that lets you run simulations (needs an update, consider it deprecated)

VSCodeIntegration allows you to open VSCode from CadBerry and precompiles GIL files while VSCode is open. This is kept seperate from Core because it requires VSCode to be installed.

IRESGenerator was created for a seperate project and will likely be moved into its own repository. 

### Style, Smart Pointers, Exceptions, and OOP
There are several common code conventions used in C++ that aren't used in CadBerry. Below are some of the things that you should know about, along with the reasoning behind the convention.

#### Style
The C++ standard library has a bunch of functions that are extremely slow while providing almost no benefit. For example, std::find. Functions like this **should not** be used. Using standard library functions is encouraged, but only when those functions provide a benefit without a segnificant performance penalty.

#### Smart Pointers
Smart pointers are used in CadBerry, but so are raw pointers. CadBerry has its own slightly less memory-safe smart pointer class (`CDB::scoped_ptr`) that is used in place of `std::unique_ptr`. While `std::shared_ptr` is acceptable if there is no other option, it is not recommended. In many situations where a shared pointer can be used, a raw pointer is preferable. For example, the GIL lexer outputs a vector of raw pointers to tokens, this way duplicates of tokens can be added.

#### Exceptions
Exceptions are avoided in the core CadBerry editor, but are used in the build engine. In CadBerry, fatal errors kill the program, so no exception is needed. In the build engine, exceptions are used to terminate compilation without crashing the program.

As a general rule, exceptions **should not** be used in CadBerry. Exceptions can be used in modules, but they should be used for fatal errors that stop whatever the module is doing without crashing the program. They should be caught and handled in the module, and then the module should exit.

#### OOP
CadBerry uses objects without being object-oriented. As a general rule, objects should be used for data structures, but if a class ends in -er (parser, builder, etc.) it should be a namespace. 


### Rendering
CadBerry uses a combination of OpenGL and Dear ImGui to render the editor. The OpenGL may change in the future, but the Dear ImGui will remain the same.

### Testing
CadBerry uses [Catch2](https://github.com/catchorg/Catch2) to test the editor and GIL. Before submitting any pull requests, you should run CadBerry_test to make sure everything is working correctly.

### CadBerry vs GIL
CadBerry is designed to be an IDE for synthetic biology. GIL is a language for synthetic biology. While they are both in the same repository, GIL is older and has more features. They are designed to be used together, but they could be seperated with a little work.

## How you can contribute
____________________________
### Reporting bugs
All bugs should be submitted as issues on GitHub. To submit a helpful bug report:
 * Give it a clear title
 * Describe exactly what you did to get the bug 
 * Describe what you expected to happen
 * Describe what actually happened. This should include:
    * The console output. If the window disappears and you can't see the console, try running the program in a terminal.
    * Screenshots/screen recordings if possible
    * If you're debugging, include the stack trace and exact error message. 
 * Include steps to reproduce the bug
 * Include the following information:
    * What OS are you using?
    * The CadBerry project file (if applicable, it will be named `ProjectName.berry`)
    * Do you have any custom/3rd party modules?

### Suggesting new features
Suggestions are welcome! Keep in mind that they may or may not be implemented, and the best way to make sure that they're implemented is to submit a pull request. When suggesting a feature:
 * Give it a clear title
 * Provide a clear description of the feature
 * Explain how it would be used
 * Explain why it would be useful (really sell it here)
 * If possible, list some other implementations of this feature

### Contributing to the project
If you're new to this project, you should start by checking out the [Todos](https://github.com/CamelCaseCam/CadBerry/issues/4). This has a bunch of small stuff that should probably get done at some point, but probably isn't being worked on right now. Remember to fork the project if you want to make changes!

## How to build CadBerry from source
____________________________
### Requirements
You will need:
 * Either Visual Studio 2022 (Windows) or GCC (Linux). Anything else isn't officially supported, but you can try to get it to work.
 * CMake for some of the dependencies.
 * The GitHub repository for CadBerry.

### Building
#### Windows
1. Clone the repository
2. Open a terminal and navigate to the root of the repository
3. Initialize all submodules
4. Navigate to `CadBerry/CadBerry/vendor` (relative to the root of the repository)
5. Make a cmake build directory, navigate to it, and run `cmake ..` (this could take a while)
6. Open the cpr solution file and build the solution in Debug, Release, and RelWithDebInfo configurations
7. Navigate to `vendor/nfd/build/vs2010` and open the NativeFileDialog solution file and build the solution in Debug and Release configurations (you'll have to retarget the projects)
8. Navigate to `vendor/yaml-cpp` and run `cmake .`
9. Open `YAML_CPP.sln` and build the solution in Debug and RelWithDebInfo configurations
10. Navigate to `CadBerry/CadBerry/` and run `vendor/bin/premake/premake5 vs2022`