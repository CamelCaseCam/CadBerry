Regions
=======
GIL files are compiled to GenBank .gb files. To specify a feature in the output file, use
``#region RegionName``. To end a region, use ``#endRegion``. Regions exist in this version of GIL to maintain some amount of backwards 
compatibility with the C# version of GIL. Ideally, you should be using sequences instead of regions. 

Example:

.. code-block:: none

   #Region Chicken Ovalbumin
   @MGSIGAASMEFCFDVFKELKVHHANENIFYCPIAIMSALAMVYLGAKDSTRTQINKVVRFDKLPGFGDSIEAQCGTSVNVHSSLRDILNQITKPNDVYSFSLASRLYAEERYPILPEYLQCVKELYRGGLEPINFQTAADQAR
   ELINSWVESQTNGIIRNVLQPSSVDSQTAMVLVNAIVFKGLWEKAFKDEDTQAMPFRVTEQESKPVQMMYQIGLFRVASMASEKMKILELPFASGTMSMLVLLPDEVSGLEQLESIINFEKLTEWTSSNVMEERKIKVYLPRMK
   MEEKYNLTSVLMAMGITDVFSSSANLSGISSAESLKISQAVHAAHAEINEAGREVVGSAEAGVDAASVSEEFRADHPFLFCIKHIATNAVLFFGRCVSP@
   #EndRegion

Capitalization
--------------
GIL recognizes #region, #Region, #endRegion and #EndRegion. 