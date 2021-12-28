Comments in GIL
============
GIL supports both single-line and multiline comments. GIL comments are exactly the same as C# comments. 

Single-line comment\:

.. code-block:: none

   //Your comment goes here

Multiline comment:

.. code-block:: none

   /*
   One line of comments
   Another line of comments
   */

**Warning: the GIL lexer treats comments like a space. As such, this is not valid:**

.. code-block:: none

   Amino/*A comment right in the middle*/Sequence
   {
       //stuff goes here
   }
