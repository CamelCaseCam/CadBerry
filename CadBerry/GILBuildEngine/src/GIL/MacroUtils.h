#pragma once

#define split { auto SplitPair_o = 
#define to(x, y) ;x = SplitPair_o.first; y = SplitPair_o.second; }

#define AddVectors(destination, add) (destination).reserve((destination).size() + (add).size());\
for (int vecaddi = 0; vecaddi < add.size(); ++vecaddi) { (destination).push_back((add)[vecaddi]); }

//The built-in for loop can use iterators, which can be invalidated by push_back. I don't like this, since there are many situations where you'd
//want to keep iterating. This implementation uses an index under the hood to make things easier

#define foreach(type, elem, collection, num) type elem;\
for (int i_  ## num  = 0; i_ ## num < collection.size(); ++i_ ## num)\
for (elem = collection[i_foreach]; false; ) assert(false & "This is disabled")
//I feel proud of that hack ^^

#define foreach(type, elem, collection) type elem; for (int i_foreach  = 0; i_foreach < collection.size(); ++i_foreach)\
for (elem = collection[i_foreach]; false; ) assert(false & "This is disabled")