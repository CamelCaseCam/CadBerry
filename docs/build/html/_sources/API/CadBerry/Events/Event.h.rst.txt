CadBerry/Events/Event.h
#######################

Imports
=======
* :doc:`/API/cdbpch.h`
* :doc:`/API/CadBerry/Core.h`
* spdlog/fmt/ostr.h

enum class EventType
====================
Contents
--------
* None = 0* WindowClose* WindowResize* Start (to be implemented)* Update (to be implemented)* PanelOpened (to be implemented)* PanelClosed (to be implemented)* KeyPressed* KeyReleased* MouseButtonPressed* MouseButtonReleased* MouseMoved* MouseScrolled* Prebuild (to be implemented)* BuildStart (to be implemented)* Postbuild (to be implemented)enum EventCategory==================Contents--------* None = 0b00000000
* EventCategoryApplication = 0b00000001
* EventCategoryInput = 0b00000010
* EventCategoryKeyboard = 0b00000100
* EventCategoryMouse = 0b00001000
* EventCategoryMouseButton = 0b00010000
* EventCategoryBuild = 0b00100000

Macros
======
EVENT_CLASS_TYPE(type)
----------------------
Defines some of the event's virtual functions

Defined as
^^^^^^^^^^
static EventType GetStaticType() { return EventType::##type; }
virtual EventType GetEventType() const override { return GetStaticType(); }
virtual const char* GetName() const override { return #type; }

EVENT_CLASS_CATEGORY(category)
------------------------------
Defines the event's category flags

Defined as
^^^^^^^^^^
virtual int GetCategoryFlags() const override { return category; }

BIND_EVENT_FN(x)
----------------
Defined as 
^^^^^^^^^^
std::bind(&x, this, std::placeholders::_1)



Event class
===========
Base class for all events. EventDispatcher is a friend class. 

Functions
---------
virtual EventType GetEventType()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns the event type

virtual const char* GetName()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns the event's name

virtual int GetCategoryFlags()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Returns the event's category flags

virtual std::string ToString()
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Converts the event to a string

bool Handled()
^^^^^^^^^^^^^^
Is the event handled?

bool IsInCategory(EventCategory Category)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Does this event have that category flag?

std::ostream& operator<<(std::ostream& os, const Event& e)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Calls the event's ToString method and prints it to the console

Fields
------
bool m_Handled
^^^^^^^^^^^^^^
Has the event been handled? If it has, it will not be propagated to the other layers



EventDispatcher class
=====================
Defines EventFn as a template of std::function<bool(T&)>

Functions
---------
Constructor(Event& evnt)
^^^^^^^^^^^^^^^^^^^^^^^^
Creates the event dispatcher and sets the event

template<typename T> bool Dispatch(EventFn<T> func)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Dispatches the event to the function. If the function returns true, the event will be set as handles and won't be propagated further

Fields
------
Event& m_Event
^^^^^^^^^^^^^^
The event that's being dispatched