GIL.HelperFunctions
===================
The HelperFunctions class contains a number of functions that you'll probably need to write GIL C# 
libraries. Below is a list of everything it contains. 

HelperFunctions.WriteWarning(string)
------------------------------------
Writes the message to the console in yellow. 

HelperFunctions.WriteError(string)
----------------------------------
Writes the message to the console in red and exits the program

HelperFunctions.GetEnd(Token[] or List<Token>, int)
---------------------------------------------------
Returns the index of the closing curly bracket corresponding to the current one

HelperFunctions.GetArgs(string)
-------------------------------
Parses the string to command line arguments

HelperFunctions.GetComplement(string)
-------------------------------------
Returns the complement of a sequence of RNA or DNA

HelperFunctions.GetParams(Token[] or List<Token>)
-------------------------------------------------
Takes a list or array of Tokens and returns any non-parameter Tokens and a Params object as a tuple

HelperFunctions.GetAminos(Token[])
----------------------------------
Extracts all amino acid characters from a Token array. If there's non amino acid characters and you 
didn't pass Unsafe:true as a parameter, it will throw an error (using HelperFunctions.WriteError) with all 
the illegal characters. If Unsafe is true, it'll return non amino acid characters, so only do this if you 
know the input's safe

HelperFunctions.VerifyAminos(string)
------------------------------------
Makes sure that all characters in the string are valid amino acids

HelperFunctions.AminoLetters
----------------------------
A list of all the valid amino acid letter codes




Params class
============
A collection of parameters returned by HelperFunctions.GetParams()

Params.Add(string Name, string Value)
-------------------------------------
Adds a parameter with the name ``Name`` and value ``Value`` to the object if that parameter doesn't exist, 
otherwise sets parameter ``Name`` to ``Value``

Params.Get(Name, ThrowOnFail = false, ErrorMessage = "")
--------------------------------------------------------
If a parameter with name ``Name`` exists, returns the parameter's value. Otherwise, it throws the error 
message ``ErrorMessage`` if ThrowOnFail is true or returns an empty string if it is false. If ThrowOnFail 
is an empty string, it throws the error message ``You must provide a value for parameter "{Param}"``