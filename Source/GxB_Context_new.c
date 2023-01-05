//------------------------------------------------------------------------------
// GxB_Context_new: create a new Context
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// Default values are set to the current GxB_CONTEXT_WORLD settings.

#include "GB.h"

GrB_Info GxB_Context_new            // create a new Context
(
    GxB_Context *Context_handle     // handle of Context to create
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GB_Context_new (&Context)") ;
    GB_RETURN_IF_NULL (Context_handle) ;
    (*Context_handle) = NULL ;
    GxB_Context Context = NULL ;

    //--------------------------------------------------------------------------
    // create the Context
    //--------------------------------------------------------------------------

    // allocate the Context
    size_t header_size ;
    Context = GB_MALLOC (1, struct GB_Context_opaque, &header_size);
    if (Context == NULL)
    { 
        // out of memory
        return (GrB_OUT_OF_MEMORY) ;
    }

    // initialize the Context
    Context->magic = GB_MAGIC ;
    Context->header_size = header_size ;
    Context->nthreads_max = GB_Context_nthreads_max_get (NULL) ;
    Context->chunk = GB_Context_chunk_get (NULL) ;
    return (GrB_SUCCESS) ;
}

