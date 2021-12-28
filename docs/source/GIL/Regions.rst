Regions
=======
GIL files are compiled to GenBank .gb files. To specify a feature in the output file, use
``#region RegionName``. To end a region, use ``#endRegion``. 

Example:

.. code-block:: none

   #region Chicken Ovalbumin
   AminoSequence
   {    //The indentation isn't necessary, but it makes this more readable
       MGSIGAASMEFCFDVFKELKVHHANENIFYCPIAIMSALAMVYLGAKDSTRTQINKVVRF
       DKLPGFGDSIEAQCGTSVNVHSSLRDILNQITKPNDVYSFSLASRLYAEERYPILPEYLQ
       CVKELYRGGLEPINFQTAADQARELINSWVESQTNGIIRNVLQPSSVDSQTAMVLVNAIV
       FKGLWEKAFKDEDTQAMPFRVTEQESKPVQMMYQIGLFRVASMASEKMKILELPFASGTM
       SMLVLLPDEVSGLEQLESIINFEKLTEWTSSNVMEERKIKVYLPRMKMEEKYNLTSVLMA
       MGITDVFSSSANLSGISSAESLKISQAVHAAHAEINEAGREVVGSAEAGVDAASVSEEFR
       ADHPFLFCIKHIATNAVLFFGRCVSP
   }
   #EndRegion

Capitalization
--------------
GIL recognizes #region, #Region, #endRegion and #EndRegion. 

Nesting
-------
As of GIL 0.3, nested regions are supported (Though they aren't as efficient). #EndRegion ends the last 
region created unless otherwise specified with #EndRegion RegionName