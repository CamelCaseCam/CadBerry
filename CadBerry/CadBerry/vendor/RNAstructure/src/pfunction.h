#pragma once 

#include "defines.h"
#include <iostream>
#include "pfunction_math.h"

#ifdef EXTENDED_DOUBLE
	#include "extended_double.h" //inlcude code for extended double if needed
#endif//defined EXTENDED_DOUBLE

#ifdef _WINDOWS_GUI
	#include "../Windows_interface_deprecated/interface.h"
#endif //_WINDOWS
#include "structure.h"
#include "forceclass.h"
#include "TProgressDialog.h"
#include "DynProgArray.h"

typedef std::vector<std::vector<std::vector<std::vector<PFPRECISION> > > > pVector4D;
typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<PFPRECISION> > > > > > pVector6D;
typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<PFPRECISION> > > > > > > pVector7D;
typedef std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<std::vector<PFPRECISION> > > > > > > > pVector8D;
//this structure contains all the info read from thermodynamic
//data files
class pfdatatable {
  public:
	vector<PFPRECISION> poppen;
	PFPRECISION maxpen;
	vector<PFPRECISION> eparam;
	vector<PFPRECISION> inter;
	vector<PFPRECISION> bulge;
	vector<PFPRECISION> hairpin;
	PFPRECISION auend,gubonus,cint,cslope,c3,efn2a,efn2b,efn2c,init,mlasym,strain,singlecbulge;
	PFPRECISION scaling;
	bool AUappliestoGU;//tracks whether the AU end penalty also applies to GU ends.

	pVector4D dangle;

	vector<PFPRECISION> tloop;
	vector<PFPRECISION> triloop;
	vector<PFPRECISION> hexaloop;
	vector<int> itloop;
	vector<int> itriloop;
	vector<int> ihexaloop;
	int numoftriloops,numoftloops,numofhexaloops;
	int maxintloopsize;

	pVector4D stack;
	pVector4D tstkh;
	pVector4D tstki;
	pVector4D coax;
	pVector4D tstackcoax;
	pVector4D coaxstack;
	pVector4D tstack;
	pVector4D tstkm;
	pVector4D tstki23;
	pVector4D tstki1n;

	pVector6D iloop11;
	pVector7D iloop21;
	pVector8D iloop22;

	PFPRECISION prelog;

	PFPRECISION **penalties;

	//Temp is the temperature in absolute scale
	pfdatatable(datatable *indata, const PFPRECISION& initialScaling, const double& Temp=310.15);
	pfdatatable();
	~pfdatatable();

	double pftemp; //The temperature -- in absolute scale 

	void rescaledatatable(const PFPRECISION& rescalefactor); //rescale the entries in datatable

	// declare variables for specification.dat data. alphabet, paring, notparing.
    // declare alphabet vector, storing alphabets, 1st dimension size is the ALPHABET_SIZE.
    // paring info 2D bool vector paring    
    // notparing info 1D char vector single
    std::vector<std::vector<char> > alphabet;
    std::vector<std::vector<bool> > pairing;  
	std::vector<char> not_pairing; // Single bases
    std::vector<char> non_interacting; // non-interacting bases
    std::vector<char> linker; // linker
    void allocate_data_tables(const int sz=6);

	int baseU, baseA;
	
	
};


PFPRECISION pfchecknp(bool lfce1,bool lfce2);
PFPRECISION erg1(int i,int j,int ip,int jp,structure *ct,pfdatatable *data);
		//calculates equilibrium constant of stacked base pairs
PFPRECISION erg2(int i,int j,int ip,int jp,structure *ct,pfdatatable *data,char a,
	char b);
		//calculates equlibrium constant of a bulge/internal loop
PFPRECISION erg2in(int i,int j,int ip,int jp,structure *ct, pfdatatable *data,char a=0,
	char b=0);
		//calculates the equilibrium constant of an internal portion of an internal/bulge loop 
PFPRECISION erg2ex(int i,int j,int size, structure *ct, pfdatatable *data);
		//calculates the equlibrium constant of an exterior fragment of am internal/bulge loop
PFPRECISION erg3(int i,int j,structure *ct,pfdatatable *data,char dbl);
		//calculates equlibrium constant of a hairpin loop
PFPRECISION erg4(int i,int j,int ip,int jp,structure *ct,pfdatatable *data,
	bool lfce);
		//calculates equlibrium constant of a dangling base
//inline PFPRECISION penalty(int i,int j,structure *ct,pfdatatable *data);

inline PFPRECISION penalty(int i,int j,structure* ct, pfdatatable *data) {	
	return data->penalties[ct->numseq[i]][ct->numseq[j]];
}


	//calculates end of helix penalty
PFPRECISION penalty2(int i, int j, pfdatatable *data);

PFPRECISION ergcoaxflushbases(int i, int j, int ip, int jp, structure *ct, pfdatatable *data);
PFPRECISION ergcoaxinterbases1(int i, int j, int ip, int jp, structure *ct, pfdatatable *data);
PFPRECISION ergcoaxinterbases2(int i, int j, int ip, int jp, structure *ct, pfdatatable *data);


//pfunction performs the partition function calculation and saves the results to disk.
void pfunction(structure* ct,pfdatatable* data, ProgressHandler* update, char* save, bool quickQ=false, PFPRECISION *Q=NULL);

//writepfsave writes a save file with partition function data.
void writepfsave(char *filename, structure *ct, 
			 PFPRECISION *w5, PFPRECISION *w3, 
			 DynProgArray<PFPRECISION> *v, DynProgArray<PFPRECISION> *w, DynProgArray<PFPRECISION> *wmb, DynProgArray<PFPRECISION> *wl, DynProgArray<PFPRECISION> *wlc, DynProgArray<PFPRECISION> *wmbl, DynProgArray<PFPRECISION> *wcoax,
			 forceclass *fce, bool *mod, bool *lfce, pfdatatable *data);

//readpfsave reads a save file with partition function data.
//Datatable2 is needed for storing alphabet information.  It is only populated with alphabet information, and not with the free energy changes.
void readpfsave(const char *filename, structure *ct, 
			 PFPRECISION *w5, PFPRECISION *w3, 
			 DynProgArray<PFPRECISION> *v, DynProgArray<PFPRECISION> *w, DynProgArray<PFPRECISION> *wmb, DynProgArray<PFPRECISION> *wl, DynProgArray<PFPRECISION> *wlc, DynProgArray<PFPRECISION> *wmbl, DynProgArray<PFPRECISION> *wcoax,
			 forceclass *fce, PFPRECISION *scaling, bool *mod, bool *lfce, pfdatatable *data, datatable *data2);
double calculateprobability(int i, int j, DynProgArray<PFPRECISION> *v, PFPRECISION *w5, structure *ct, pfdatatable *data, bool *lfce, bool *mod, PFPRECISION scaling, forceclass *fce);

void rescale(int currenth,structure *ct, pfdatatable *data, DynProgArray<PFPRECISION> *v, DynProgArray<PFPRECISION> *w, DynProgArray<PFPRECISION> *wl, DynProgArray<PFPRECISION> *wcoax,
			 DynProgArray<PFPRECISION> *wmb,DynProgArray<PFPRECISION> *wmbl, PFPRECISION *w5, PFPRECISION *w3, PFPRECISION **wca, PFPRECISION **curE, PFPRECISION **prevE, PFPRECISION rescalefactor); //function to rescale all arrays when partition function calculation is headed out
void rescale(int currenth,structure *ct, pfdatatable *data, DynProgArray<PFPRECISION> *v, DynProgArray<PFPRECISION> *w, DynProgArray<PFPRECISION> *wl, DynProgArray<PFPRECISION> *wcoax,
			 DynProgArray<PFPRECISION> *wmb,DynProgArray<PFPRECISION> *wmbl, PFPRECISION *w5, PFPRECISION *w3, PFPRECISION **wca, PFPRECISION rescalefactor); //function to rescale all arrays when partition function calculation is headed out

															//of bounds
//void rescaleatw3(int ii,structure *ct, pfdatatable *data, DynProgArray<PFPRECISION> *v, DynProgArray<PFPRECISION> *w, DynProgArray<PFPRECISION> *wl, DynProgArray<PFPRECISION> *wcoax,
//			 DynProgArray<PFPRECISION> *wmb,DynProgArray<PFPRECISION> *wmbl, PFPRECISION *w5, PFPRECISION *w3, double rescalefactor); //function to rescale all arrays when partition function calculation is headed out
															//of bounds when claculating w3
//void rescaleatw5(int jj,structure *ct, pfdatatable *data, DynProgArray<PFPRECISION> *v, DynProgArray<PFPRECISION> *w, DynProgArray<PFPRECISION> *wl, DynProgArray<PFPRECISION> *wcoax,
//			 DynProgArray<PFPRECISION> *wmb,DynProgArray<PFPRECISION> *wmbl, PFPRECISION *w5, PFPRECISION *w3, double rescalefactor); //function to rescale all arrays when partition function calculation is headed out
															//of bounds when claculating w5
void thresh_structure(structure *ct, char *pfsfile, double thresh); //determine a structure of probable base pairs (greater than thresh) and deposit it in ct.

//calculate a the partition function, given that the arrays have been allocated
void calculatepfunction(structure* ct,pfdatatable* data, ProgressHandler* update, char* save, bool quickQ, PFPRECISION *Q,
	DynProgArray<PFPRECISION> *w, DynProgArray<PFPRECISION> *v, DynProgArray<PFPRECISION> *wmb, DynProgArray<PFPRECISION> *wl, DynProgArray<PFPRECISION> *wlc, DynProgArray<PFPRECISION> *wmbl,
	DynProgArray<PFPRECISION> *wcoax, forceclass *fce,PFPRECISION *w5,PFPRECISION *w3,bool *mod, bool *lfce, bool disablecoax=false);


// This copies code from pclass.cpp
//A function to convert free energies to equilibrium constants.
//inline PFPRECISION boltzman(short i, PFPRECISION temp) {
//
//	if (i>=INFINITE_ENERGY) return 0;
//	else return exp((-((PFPRECISION) i)/((PFPRECISION)conversionfactor))/(RKC*temp));
//
//}
