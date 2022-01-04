Operation
=========
A reusable bit of GIL code wraped around another piece of code. 


Calling Operations
------------------
An Operation is called with .OperationName with the code that goes inside the Operation inside curly 
brackets

Example:

.. code-block:: none

   .OperationName
   {
       @MFPMAX@
   }


Creating Operations
-------------------
An Operation is created with operation OperationName and the operation's code in curly brackets. 

Example:

.. code-block:: none

   operation OperationName
   {

   }

Use $InnerCode wherever you want the code that the operation is called on to appear. 

Example:

.. code-block:: none

   operation OperationName
   {
        //Some code
        $InnerCode
        //Some more code
        $InnerCode
        //Even more code
   }