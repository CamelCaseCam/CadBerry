CadBerry Packages
#################
CadBerry packages let you distribute your modules and modifications in a single file that handles installation and updating. This page will teach 
you how the package system works, but first I need to tell you the #1 rule of CadBerry packages. Unless you're working on the core editor, never 
ever **ever** mess with the CadBerry.cdbpkg file. This is the core editor's package file, and unless you're **EXTREMELY** careful, you could brick 
your CadBerry installation. 

How to write a package
======================
CadBerry packages are written in YAML and parsed using the yaml-cpp library. First, create a new cdbpkg file in the same directory as your 
CadBerry executable. 

Next, open the file and fill in the following fields:

.. code-block:: none

	Name: Package name
	Version: Package version (optional)
	Downloaded: The date the package was last downloaded. You can set this to anything
	Source:
	  Owner: GitHub repo owner username
	  Repo: GitHub repo name
	  Branch: GitHub repo branch
	  Path: path within the GitHub repo to the files. If the files are in the main folder of the repository, you can set this to nothing
	Files: [File 1, File 2, ...] (paths relative to Path)

Something important to note about these files is that they only support github repositories. When the user opens the editor, CadBerry checks if 
the last commit to the source repo is different from the date the package was downloaded. If it is, it opens a window prompting the user to 
update the package. 

If you set Downloaded to something like "Download this package", CadBerry will think the package needs to be updated and will prompt the user to 
update it. This way, you can distribute a single cdbpkg file that works as both a downloader and an updater for your module. 

Help! I messed with CadBerry.cdbpkg
===================================
If you messed with CadBerry.cdbpkg and haven't updated the "CadBerry core" package yet, you're in luck! Your changes haven't been applied, so you 
can just download the unchanged package file from the GitHub and you'll be fine. 

If you did click update, you're in a bit more trouble. You have two options. First, you could re-download CadBerry from the GitHub and continue on 
with whatever you were doing before. If you choose this option, make sure to download a completely new version instead of downloading the 
individual components. 

Your other option is to try to fix your current installation. First, you'll need to get the updated package from the GitHub and download the 
working Berry.exe and CadBerry.dll files. Once you've got these, delete all other dlls being used by CadBerry and run the editor. You should get a 
minimal version that can only update packages. Once you get the package window, click update to recover the rest of the CadBerry packages. 