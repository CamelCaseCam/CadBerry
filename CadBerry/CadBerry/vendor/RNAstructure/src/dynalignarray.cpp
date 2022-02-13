/*
 * Dynalign
 *  by David Mathews, copyright 2002, 2003,2004,2005, 2006
 *
 * Contributors: Chris Connett and Andrew Yohn, 2006
 */

#include "dynalignarray.h"

#include "defines.h"
#include "dynalign.h"

dynalignarray::dynalignarray() {}

dynalignarray::dynalignarray(short n1, short n2, short *lowlimit, short *highlimit, bool Optimalonly) {
  allocate(n1,n2,lowlimit,highlimit,Optimalonly);
}

void dynalignarray::allocate(short n1, short n2, short *lowlimit, short *highlimit, bool Optimalonly) {
  short i,j,k,l,I;

  N = n1;
  N2 = n2;
  Lowlimit = lowlimit;
  Highlimit = highlimit;
  Ndiff = N-N2;

  optimalonly = Optimalonly;

  infinite = DYNALIGN_INFINITY;

  //allocate the array:
	
  //First allocate the i dimension:
  array = new integersize ***[N+1];
	
  for (i=0;i<=N;++i) {

    //now allocate the j dimension:
    if (optimalonly) array[i]=new integersize **[N-i+1];
    else array[i] = new integersize **[N+1];

    //now shift the j dimension pointers so that the array positions are indexed in the way they are needed
    //for all i and j, j>i
    array[i]=array[i]-i;

    //now allocate the k dimension:
    if (optimalonly) I = N;
    else I = i+N-1;//changed by DHM on 11/27/06 by adding -1
    for (j=i;j<=I;j++) {
      //allocate the k dimension:
      array[i][j] = new integersize *[highlimit[i]-lowlimit[i]+1/*2*M+2*/];

      // Now shift the k dimension pointers so that the array
      // positions are indexed in the way that they are needed
      //|i*N2/N - k | <= M
      array[i][j]=array[i][j]-lowlimit[i]/*(i,M,N,N2)*/;

      for (k=lowlimit[i]/*(i,M,N,N2)*/;k<=highlimit[i]/*(i,M,N,N2)*/;k++) {
        //allocate the l dimension:
        array[i][j][k]=new integersize[highlimit[j]-lowlimit[j]+1/*2*M+2*/];

        // now shift the l dimension pointers so that the array
        // positions are indexed in the way that they are needed
        // |j*N2/N - l | <= M
        array[i][j][k]=array[i][j][k]-lowlimit[j]/*(j,M,N,N2)*/;

        for (l=lowlimit[j]/*(j,M,N,N2)*/;l<=highlimit[j]/*(j,M,N,N2)*/;l++) {

          //make the initial assignment:
          array[i][j][k][l]=DYNALIGN_INFINITY;
        }
      }
    }
  }
}
		
dynalignarray::~dynalignarray() {
  short i,j,k,I;

  //delete the array
  for (i=0;i<=N;++i) {
    //now delete the k dimension:
    if (optimalonly) I = N;
    else I = i+N-1;
    for (j=i;j<=I;j++) {
      for (k=Lowlimit[i]/*(i,M,N,N2)*/;k<=Highlimit[i]/*(i,M,N,N2)*/;k++) {
        //now shift the l dimension pointers back
        array[i][j][k]=array[i][j][k]+Lowlimit[j]/*(j,M,N,N2)*/;
				
        //delete the l dimension:
        delete[] array[i][j][k];
      }
      //now shift the k dimension pointers back
      array[i][j]=array[i][j]+Lowlimit[i]/*(i,M,N,N2)*/;

      //delete the k dimension:
      delete[] array[i][j];
    }

    //now shift the j pointers back
    array[i]=array[i]+i;

    //now delete the j dimension:
    delete[] array[i];
  }

  //delete the i dimension:
  delete[] array;
}
