utils
=====
A collection of helpful operations

utils.MutProt
-------------
Using the Mutations parameter, you can specify one or more protein mutations separated by /. This will 
mutate the amino acid at that location (starting at 1) to the specified amino acid. 

Example:

.. code-block:: none

   using utils
   .MutProt
   {
        Mutations:Q3N/P4A
        MAQPX
   }