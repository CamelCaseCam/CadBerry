CadBerry/Events/BuildEvent.h
############################

Imports
=======
* :doc:`/API/cdbpch.h`
* :doc:`Event.h`

PrebuildEvent class
===================
Inherits from :doc:`CDB::Event <Event.h>`. This event is dispatched before CadBerry builds the project. This event is intended for saving any open 
files. 

Categories
----------
* EventCategoryBuild
* EventCategoryApplication

Event type
----------
Prebuild

BuildStartEvent class
=====================
Inherits from :doc:`CDB::Event <Event.h>`. This event is dispatched right before CadBerry builds the project. This event is intended for any 
actions that need to be taken with up to date files before the build process. 

Categories
----------
* EventCategoryBuild
* EventCategoryApplication

Event type
----------
BuildStart

PostbuildEvent
==============
Inherits from :doc:`CDB::Event <Event.h>`. This event is dispatched after the build engine finishes building the project. This event is intended 
for clean-up operations

Categories
----------
* EventCategoryBuild
* EventCategoryApplication

Event type
----------
Postbuild