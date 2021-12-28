CadBerry/Utils/memory.h
#######################
At this point, I'm going to have to explain myself. I'm fairly new to C++ (I used to be a C# programmer), so I don't have much experience with 
manual memory management. While raw pointers are bad, I started using C++ for the performance benefits, so I REALLY don't want to use something 
like shared_ptr if I can avoid it. 

Because of this, I decided to create my own modified version of unique_ptr called scoped_ptr. scoped_ptr has a few improvements over unique_ptr 
that I like for my purposes:

1. You can use = to set the pointer. Instead of calling std::make_unique, you can just say ptr = some pointer. If the pointer already points to an object, the object will be deleted. 
2. You can use ptr.raw() to get a copy of the raw pointer. I'm using a memory management system where one object owns a pointer, but can lend it to other objects. 
3. You can use ptr.forget() to remove the object's ownership of the pointer without deleting it. This is the most dangerous function, because the memory will be leaked if you don't keep a copy of the pointer. 
4. You can use == to test if the pointer owned by the object equals another pointer. For example, ptr == nullptr works

It's possible that unique_ptr has all of this and I created this for nothing, but I didn't know about it and I prefer to work with something I 
understand. Also, be aware of the fact that this class is new, so I might change it and I haven't changed all the raw pointers to smart pointers 
yet. 