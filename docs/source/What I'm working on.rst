To do
=====
* Change GetMousePos so that it returns ImVec2 and get rid of Math.h
* Change log to use scoped_ptr instead of shared_ptr
* Change Input to use ImVec2 and scoped_ptr instead of raw pointer
* Change ProjectCreationLayer::ProjectName to use std::string instead of char[64]
* Make sure all events are being dispatched
* Remove UpdateEvent, since it's redundant
* Add importing GIL files from Build/Libraries