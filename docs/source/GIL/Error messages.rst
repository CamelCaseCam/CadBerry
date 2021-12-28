Error messages
==============
This page contains all the errors and in which situations they apply. Each one also has an associated
error code in the format "error GILxx" where x is the error number. For example, error GIL01

Errors:
=======
* Error GIL01: No region to end

You've either tried to end a region (#EndRegion) before any were created, or you tried to end a region that 
is closed by name later in your project (#EndRegion region name).

* Error GIL02: Region "Region name" does not exist

This error is caused when you try to end a region by name that does not exist. For example, you might have 
created a region with the name "TestRegion", but you tried to close the region "TesRegion"

* Error GIL03: No GIL project (.gil) in current directory

Gil compile couldn't find a .gil file

* Error GIL04: Project template "Template" not found
* Error GIL05: End token ("}") expected
* Error GIL06: The name "Name" does not exist

You referenced a sequence or operation that does not exist. 

* Error GIL07: Illegal character 'char' in complement

GIL tried to find the complement of a string containing letters other than a, t, c, g, and u. If you're developing a library, this might have been caused by calling HelperFunctions.GetComplement() on a string that isn't DNA or RNA.

* Error GIL08: Attribute "attribute" requires value of type "type"

You tried to set an attribute to an unsupported type. For example, you tried to set RNAI_Len to 'a' or to 1.1

* Error GIL09: Attribute "attribute" does not exist

You used "#SetAttribute Attribute:Value" on an attribute that does not exist. Check your spelling and if the attribute is spelled right, add an issue on the GitHub (I might have forgot to add the attribute)

* Error GIL10: Unable to find siRNA matching given params

GIL was unable to find a siRNA that satisfies all the paramaters in Block

* Error GIL11: Sequence contained the following non amino acid characters:

GIL (or a library you're using/writing) was expecting a sequence of amino acids, but one of the characters wasn't an amino acid. For example, MQPF_R wouldn't work because the _ isn't an amino acid