Target organisms
================
Currently, GIL supports Saccharomyces Cerevisiae (baker's yeast), Pichia, E.coli, and Humans as compilation 
targets. 

To specify a target, use ``#target Organism``. GIL also supports using multiple names for the same organism. To add a name for an organism, 
add the organism's name.gilEncoding to the CompilationTargets folder. Add ``goto`` the name of the file with the full codon encodings for the organism. 

Example:

.. code-block:: none

   goto S.cerevisiae


Organism names
==============

Saccharomyces Cerevisiae
------------------------
Saccharomyces cerevisiae, S.cerevisiae, Yeast

Pichia
------
Pichia pastoris, Pichia

E.Coli
------
E.coli, Escherichia coli

Human
-----
Human


Adding support for other organisms
==================================
GIL stores codon encodings in .gilEncoding files in the CompilationTargets folder. To add support for another organism, add the file 
organism name.gilEncoding to the CompilationTargets folder. In this file, add the lower case single letter abreviation of the amino acid 
and its associated codons in order of codon availability, seperated by a colon. 

That was hard to describe, so here's an example for baker's yeast:

.. code-block:: none

   g:GGT GGA GGC GGG