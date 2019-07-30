//------------------------------------------------------------------------------
// gbprint: print a GraphBLAS matrix struct
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#include "gb_matlab.h"

void mexFunction
(
    int nargout,
    mxArray *pargout [ ],
    int nargin,
    const mxArray *pargin [ ]
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    gb_usage (nargin <= 2 && nargout == 0, "usage: gbprint (X,level)") ;

    int level = 3 ;
    if (nargin > 1)
    {
        CHECK_ERROR (!IS_SCALAR (pargin [1]), "level must be a scalar") ;
        level = (int) mxGetScalar (pargin [1]) ;
    }

    //--------------------------------------------------------------------------
    // print the GraphBLAS matrix
    //--------------------------------------------------------------------------

    GrB_Matrix X = gb_get_shallow (pargin [0]) ;
    GxB_Matrix_fprint (X, "", level, stdout) ;
    gb_free_shallow (&X) ;
}

