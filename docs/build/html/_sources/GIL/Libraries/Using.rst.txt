Linking to outside dlls
#######################
GIL supports linking to outside C++ dlls using the using keyword. You can import a file by name or by path (excluding the file extension)


Writing outside dlls
====================
To create a GIL module, write a class that inherits from GIL::GILModule and overrides the GetOperation and GetSequence functions. Then, write an 
``extern "C"`` function that exposes the module to GIL with the following signature: ``__declspec(dllexport) GIL::GILModule* __stdcall GetModule()``

The GetOperation and GetSequence functions return a pointer to any special operation and sequences that you create based on their names. If you 
get an operation that your module doesn't add, you can just return nullptr. 

Creating sequences and operations
---------------------------------
Operations
^^^^^^^^^^
Create a class that inherits from GIL::Operation. Override the Save and Load functions with an empty function (because these operation subclasses 
have to be loaded from the dll, not from a CGIL file). Finally, override the Get function. This function is called when your operation is called. 

Example:

.. code-block:: c++
   
   class SetTRAPzymeTarget : public GIL::Operation
   {
   public:
		virtual std::pair<std::vector<GIL::Parser::Region>, std::string> Get(std::vector<GIL::Lexer::Token*> InnerTokens, GIL::Parser::Project* Proj) override;
		virtual void Save(std::ofstream& OutputFile) override {}
		virtual void Load(std::ifstream& InputFile) override {}

		static GIL::Operation* self;
		static GIL::Operation* GetPtr();
   };

Sequences
^^^^^^^^^
Exactly the same as with operations, but instead of having a Get function, it has GetRegions and GetCode. In the future, I will update this to use 
``std::pair`` instead of two functions. 

Example:

.. code-block:: c++
   
   class Sequence    //Base sequence class that is inherited by different sequence types
   {
   public:
		virtual ~Sequence() {}
		virtual std::vector<Parser::Region>* GetRegions(Parser::Project* Proj) = 0;
		virtual std::string* GetCode(Parser::Project* Proj) = 0;

		virtual void Save(std::ofstream& OutputFile) = 0;
		virtual void Load(std::ifstream& InputFile) = 0;
   };