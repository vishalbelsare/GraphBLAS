//------------------------------------------------------------------------------
// GrB_init: initialize GraphBLAS
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2022, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// GrB_init (or GxB_init) must called before any other GraphBLAS operation.
// GrB_finalize must be called as the last GraphBLAS operation.

#include "GB.h"
#include "GB_init.h"

GrB_Info GrB_init           // start up GraphBLAS
(
    GrB_Mode mode           // blocking or non-blocking mode
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_CONTEXT ("GrB_init (mode)") ;

    //--------------------------------------------------------------------------
    // initialize GraphBLAS
    //--------------------------------------------------------------------------

    // default:  use the ANSI C11 malloc memory manager, which is thread-safe 

#if defined ( GBCUDA )
    if (mode == GxB_BLOCKING_GPU || mode == GxB_NONBLOCKING_GPU)
    {
        return (GB_init (mode,              // blocking or non-blocking mode
            // RMM C memory management functions
            rmm_wrap_malloc, rmm_wrap_calloc, rmm_wrap_realloc, rmm_wrap_free,
            Context)) ;
    }
#endif

    return (GB_init (mode,              // blocking or non-blocking mode
        malloc, calloc, realloc, free,  // ANSI C memory management functions
        Context)) ;
}

