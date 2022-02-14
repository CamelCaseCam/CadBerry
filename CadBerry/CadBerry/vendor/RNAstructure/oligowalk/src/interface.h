
#if !defined (__INTERFACE)
#define __INTERFACE
#include <iostream>
<<<<<<< HEAD
using namespace std;
=======
//
using namespace std;
//
>>>>>>> 361492b0f6e9a29bb88098eeab4d8ec72d2d1807



///////////////////////////////////////////////////////////////////////////
////////TProgressDialog is opened to notify the user of the progress of
////////the folding algorithm

class TProgressDialog 
{
	public:

   	void update(int frac) {
		cout << frac <<"%"<<flush;
	}


};


/////////////////////////////////////////////////////////////////////////
/////////TRnaDialog is the main program interface


#endif
