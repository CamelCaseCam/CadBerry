From keyword
============
From translates a genetic sequence from one organism to another. If your origin organism doesn't have a gilencoding file or its codon usage bias 
is unknown, you can use ``From Unoptimized`` to ignore codon use bias and pick the best codons for your target organism. This may cause problems 
with protein folding, so it should only be used as a last-resort

Example:

.. code-block:: none

   From Pichia
   {
      ATGTTTTAG
   }