sensing
#######
Contains operations to help the cell sense its environment

Creating TRAPzymes
==================
The sensing library contains two operations that can be used together to generate a TRAPzyme in a couple of seconds. Something important to 
remember is that these operations will always create *something* if possible. Even if the only option is a TRAPzyme that probably won't work, 
these operations are designed to make something rather than nothing. 

Because of this, it's important to check the console for warnings if you're using these operations. It's fine if you get a warning about 
attenuator/antiattenuator self-binding (though you should aim for a chunk size of less than 4), but any warnings about attenuator-ribozyme binding 
are a bad sign. This warning means that your attenuator is binding to parts of the ribozyme that aren't the catalytic core. 

sensing: .SetTRAPzymeTarget
---------------------------
This operation sets the TRAPzyme's substrate. When using this operation, use the biggest possible sequence that will be present, because this gives 
the optimizer the most freedom to minimize attenuator-ribozyme binding. You must call this operation before calling GenTZFromAttenuator, but the 
target is preserved, so you only have to call it when you want to change the target. 

Example:

.. code-block:: none

   using "sensing"
   
   sensing: .SetTRAPzymeTarget
   {
		ATGAATCTTCCGTCTTTCGTCCAACGTCTCTCCACAGCAAGCAGTCGCAGTATAGCGATTACTTGCGTAGTTGTCCTTGCCTCTGCAATCGCCTTTCCCTTCATCCGCAGAGACTACCAGACCTTCCTGGAAGTGGGACCCTCGTACGCCCCGCAGAACTTTAGAGGATACATCATCGTCTGTGTCCTCTCGTTGTTCCGCCAAGAACAAAAAGGACTCGAAATCTACGATCGGCTCCCAGAGAAACGAAGGTGGTTGTCCGACCTTCCCTTTCGTGACGGGCCCAGACCCAGCATCACAAGCCATATCATTCAACGACAGCGTACCCAACTAGTTGATCCGGACTTCGCTACCCAGGAGCTCATAGGCAAAGTCATCCCTCGTGTGCAAGCACGACACACCGACAAAACATTCCTCAGCACCTCCAAATTCGAATTTCACGCAAAAGCCATATTCCTCCTGCCTTCCATCCCAATCAACGACCCTCTGAACGTTCCAAGCCACGACACTGTCCGACGAACGAAGCGCGAGATCGCGCATATGCATGATTATCATGATTGCACTCTTCACATCGCTCTCGCTGCTCAGGACGGAAAGGAGGTTTTGAAGAAGGGATGGGGGCAACGACACCCACTCGCTGGACCTGGAGTGCCCGGCCCACCGACGGAGTGGACGTTTCTCTATGCGCCTCGAAACGAAGAAGAGGTTCGAGTTGTGGAGATGATTATTGAGGCTGCCATAGGTTACATGACGAATGATCCGGCAGGAAAAGTTGTAGAAGCCACTGGAAAG
   }

sensing: .GenTZFromAttenuator
-----------------------------
Generates a TRAPzyme from the attenuator sequence. This operation does the following things:

1. Finds the attenuator with the least self-binding and (if it has to decide between multiple equal attenuators) highest or lowest GC content
    1. Finds an attenuator candidate. If this candidate doesn't self-bind, it picks it and returns
	2. Recursively calls itself to try to find a better attenuator later on in the sequence. 
	3. If it can't find a better option later in the sequence, returns this candidate
	4. If it finds an equal option later in the sequence, returns the candidate with the higher or lower GC content (calculated using (GC - 0.5f)^2 )
	5. If the candidate it returned exibits self-binding, prints a warning to the console. 
2. If it couldn't find a valid attenuator at the last step, prints an error and returns
3. Find the TRAPzyme body with the least self-binding
	1. Finds a valid target site in the substrate
	2. If the ribozyme generated from this target site doesn't bind with the attenuator (in chunks of 7 or more, since it's supposed to bind with the catalytic core), it returns it
	3. Otherwise, recursively calls itself to try to find a better target site
	4. If no perfect target site was found, prints an error to the console. 

As you can see, this algorithm works best if you provide bigger target and attenuator sequences. 