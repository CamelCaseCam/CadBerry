#include "RNA.h"

/*
This is a wrapper for the RNAstructure library for use in CadBerry. That's why I'm including the cpp files
*/

#undef __cpp_lib_byte
//Libraries
#include "src/common_utils.cpp"
#include "src/substructure.cpp"

#include <RNA_class/RNA.cpp>
#include "src/rna_library.cpp"
#include "src/structure.cpp"
#include "src/DynProgArray.cpp"
#include "src/forceclass.cpp"
#include "src/pfunction.cpp"
#include "src/outputconstraints.cpp"
#include "RNA_class/design.cpp"
#include "RNA_class/Dynalign_object.cpp"
#include "RNA_class/HybridRNA.cpp"
#include "RNA_class/Multilign_object.cpp"
#include "RNA_class/Oligowalk_object.cpp"
#include "RNA_class/ProbScan.cpp"
#include "RNA_class/RsampleData.cpp"
#include "RNA_class/thermodynamics.cpp"
#include "RNA_class/TwoRNA.cpp"
#include "src/algorithm.cpp"
#include "src/alltrace.cpp"
#include "src/alltrace_intermolecular.cpp"
#include "src/bimol.cpp"
#include "src/configfile.cpp"
#include "src/dotarray.cpp"
#include "src/DotPlotHandler.cpp"
#include "src/draw.cpp"
#include "src/dynalignarray.cpp"
#include "src/dynalign.cpp"
#include "src/dynalignheap.cpp"
#include "src/dynalignstackclass.cpp"
#include "src/extended_double.cpp"
#include "src/intermolecular.cpp"
#include "src/loop_utils.cpp"
#include "src/MaxExpect.cpp"
#include "src/MaxExpectStack.cpp"
#include "src/observable.cpp"
#include "src/observer.cpp"
#include "src/OligoScreenCalc.cpp"
#include "src/pclass.cpp"
#include "src/phmm/aln_env_utils.cpp"
#include "src/phmm/p_alignment.cpp"
#include "src/phmm/phmm_aln.cpp"
#include "src/phmm/phmm_array.cpp"
#include "src/phmm/phmm.cpp"
#include "src/phmm/phmm_ml_loops.cpp"
#include "src/phmm/phmm_pp_loops.cpp"
#include "src/phmm/structure/folding_constraints.cpp"
#include "src/phmm/structure/structure_object.cpp"
#include "src/phmm/utils/ansi_string/ansi_string.cpp"
#include "src/phmm/utils/file/utils.cpp"
#include "src/phmm/utils/xmath/linear/linear_math.cpp"
#include "src/phmm/utils/xmath/log/xlog_math.cpp"
#include "src/phmm/utils/xmath/matrix/matrix.cpp"
#include "src/probknot.cpp"
#include "src/random.cpp"
#include "src/siRNAfilter.cpp"
#include "src/stackclass.cpp"
#include "src/stackstruct.cpp"
#include "src/stochastic.cpp"
#include "src/StructureImageHandler.cpp"
#include "src/thermo.cpp"
#include "src/TProgressDialog.cpp"
#include "src/varray.cpp"
#include "src/wendarray.cpp"

#define __cpp_lib_byte