#ifndef _SEED_MANAGER_
#define _SEED_MANAGER_

#include <vector>
<<<<<<< HEAD
using namespace std;
=======
//
using namespace std;
//
>>>>>>> 361492b0f6e9a29bb88098eeab4d8ec72d2d1807

// Global seed manager for RNG's. 

class t_seed_manager
{
public:
	t_seed_manager();
	t_seed_manager(char* seeds_fp);
	~t_seed_manager();

	static vector<int>* seeds;
	static int i_next_seed;

	// Generte a unique seed.
	static int seed_me();

	static void dump_seeds();
};

#endif // _SEED_MANAGER_

