GIL booleans will work in the following way:

1. Bools are converted (at parse-time) into a graph
```
bool b1 = ...
bool b2 = ...

if (b1 & b2)
{
	DoSomething()
}
```
becomes
```
	b1    b2
	 \____/
	   |
	  AND
	   |
  DoSomething
```
2. At link time, other bool graphs are imported and linked via placeholders. Because we only know at link time how many times a sequence will be called, local sequence graphs are expanded and merged into the global graph. This mangles the bool names (although they likely could be accessed using a custom operation)
```
bool b1 = ...
bool b2 = ...

sequence seq(b1, out)
{
	bool l_b1 = ...
	out = b1 & l_b1
}

bool o1
bool o2

seq(b1, o1)
seq(b2, o2)

```
becomes
```
	b1   seq@l_b1@r1   b1   seq@l_b1@r2
	|______|           |______|
		|                  |
	   AND                AND
	    |                  |
	    o1	               o2
```
It is necessary to mangle the local variable names because they are actually global variables. Each instance of a sequence exists in parallel
3. Once we have a bool graph, we pass it to the allocator, which picks implementations for each bool and adds cast nodes to the graph


# Unsafe operations to be added
* GetBoolByExactName - allows you to access *any* bool based on its name. This is *very* dangerous because you could access a bool in a specific 
run of a sequence
* AllocWithType - Adds a global bool to the graph with a specific name and implementation type. Terminates compilation if the type cannot be 
created
* SetFalse - Sets a bool to false. This will only work with an implementation that supports it, and it's dangerous because it can't be reversed. 
Usually this would be implemented using a protease or ribozyme, so if it's active the bool will be false even if it's set to true elsewhere. 
