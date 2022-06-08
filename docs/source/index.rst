.. GIL documentation master file, created by
   sphinx-quickstart on Wed Jun  2 08:15:23 2021.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.
.. To build, run command "sphinx-build -b html source build"

Welcome to CadBerry's documentation!
====================================
CadBerry is an open-source IDE for synthetic biology built around a C++ version of the GIL compiler. 
CadBerry is still early in development, so built versions might be unstable. 

If you're new here, check out :doc:`User/GettingStarted` for a quickstart guide. 

Features
--------
* Codon optimization
* Auto-generated `TRAPzymes <https://pubmed.ncbi.nlm.nih.gov/12022862/>`_ to detect cellular RNAs
* Support for reusing DNA using libraries
* Add-on support using C++ and `Dear ImGui <https://github.com/ocornut/imgui>`_
* Included math modelling system
* Included thread pool

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
Option 1: Bulding from source
-----------------------------
Download the source code and use visual C++ 2020 to build. You may have some trouble with downloading submodules (this will be fixed soon). 

Option 2: Just download the compiled folder
-------------------------------------------
Download the BasicInstall folder or the CadBerry\bin\Release-windows-x86_64\Berry folder and run Berry.exe (you may get some errors if you download the 
Berry folder, if you do you should delete CDBLastProj.cfg and CDBProjectList.cfg)

.. toctree::
   :maxdepth: 2
   :caption: Contents:
   
   User/TheBasics



Indices and tables
==================

* :ref:`genindex`
* :ref:`search`
