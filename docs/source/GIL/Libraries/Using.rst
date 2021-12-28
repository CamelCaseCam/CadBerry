Linking to outside dlls
=======================
GIL supports linking to outside C# dlls using the using keyword. 

You can import a file by name or by path (excluding the file extension). However, the dll and the manifest 
**must** be in the same folder. 


Writing outside dlls
--------------------
To create an outside dll, create a C# class library project with a Main class inside the project namespace. 
Inside this class, reference GIL.dll and add a Dictionary<string, Func<List<Token>, Compiler, RelativeFeature>>
called ``Operations``. This Dictionary will contain any functions that you want to expose to users. The 
functions in this Dictionary will appear as operations. Finally, add the names of all the operations to the 
manifest file (A file with the same name as the dll but no file extension). 

Each of the operations inside this Dictionary must return a RelativeFeature. If you don't want to return 
anything, return RelativeFeature.Empty. 

The function HelperFunctions.GetParams(Token[] or List<Token>) returns the inside tokens excluding the 
parameters and a Params object which contains any parameters specified with ParameterName:Value