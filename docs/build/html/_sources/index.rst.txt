.. GIL documentation master file, created by
   sphinx-quickstart on Wed Jun  2 08:15:23 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.
.. To build, run command "sphinx-build -b html source build"

Welcome to CadBerry's documentation!
====================================
CadBerry is an open-source IDE for synthetic biology built around a C++ version of the GIL compiler. 
CadBerry is still early in development, so built versions might be unstable

Features
--------
* Allows user-created libraries to avoid rewriting code
* Supports user-created modules to quickly add functionality
* Multithreading and rendering support

Contribute
----------
- Discord server: https://discord.gg/ezZTcwZGKD
- Issue Tracker: https://github.com/CamelCaseCam/CadBerry/issues
- Source Code: https://github.com/CamelCaseCam/CadBerry

To see the latest features, check out the developer branch on github

Support
-------
If you're having problems, create an issue on github and/or email me at Cameron.kroll@gmail.com

License
-------
CadBerry is licensed under the GNU GPL-3.0 license. 



Installation
============
Option 1: bulding from source
-----------------------------
Download the source code and use visual C++ 2020 to build.

Option 2: Just download the compiled folder
-------------------------------------------
Download the bin folder if on windows. If you're on another platform, you'll have to compile the source code for your OS.

.. toctree::
   :maxdepth: 2
   :caption: Contents:
   
   GIL/GIL

   Modules/Creating modules
   Modules/Packages

   API/CadBerry API

   What I'm working on



Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
