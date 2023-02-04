//------------------------------------------------------------------------------
// GB_op_name_and_defn: construct name and defn of a user-defined op
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2022, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// This method constructs the name and defn of a user-defined op (unary,
// binary, or indexunary).  It also constructs the op->hash for the jit.

#include "GB.h"
#include <ctype.h>
#include "GB_jitifyer.h"

GrB_Info GB_op_name_and_defn
(
    // output
    char *operator_name,        // op->name of the GrB operator struct
    int32_t *operator_name_len, // op->name_len
    uint64_t *operator_hash,    // op->hash
    char **operator_defn,       // op->defn
    size_t *operator_defn_size, // op->defn_size
    // input
    const char *input_name,     // user-provided name, may be NULL
    const char *input_defn,     // user-provided name, may be NULL
    const char *typecast_name,  // typecast name for function pointer
    size_t typecast_len         // length of typecast_name
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    ASSERT (operator_name != NULL) ;
    ASSERT (operator_defn != NULL) ;
    ASSERT (typecast_name != NULL) ;
    ASSERT (operator_defn_size != NULL) ;
    (*operator_defn) = NULL ;
    (*operator_defn_size) = 0 ;

    //--------------------------------------------------------------------------
    // get the name of the operator
    //--------------------------------------------------------------------------

    memset (operator_name, 0, GxB_MAX_NAME_LEN) ;
    if (input_name != NULL)
    {
        // copy the input_name into the working name
        char working [GxB_MAX_NAME_LEN] ;
        memset (working, 0, GxB_MAX_NAME_LEN) ;
        strncpy (working, input_name, GxB_MAX_NAME_LEN-1) ;
        // see if the typecast appears in the input_name
        char *p = NULL ;
        p = strstr (working, typecast_name) ;
        if (p != NULL)
        { 
            // skip past the typecast, the left parenthesis, and any whitespace
            p += typecast_len ;
            while (isspace (*p)) p++ ;
            if (*p == ')') p++ ;
            while (isspace (*p)) p++ ;
            // p now contains the final name, copy it to the output name
            strncpy (operator_name, p, GxB_MAX_NAME_LEN-1) ;
        }
        else
        { 
            // no typcast appears; copy the entire operator_name as-is
            memcpy (operator_name, working, GxB_MAX_NAME_LEN) ;
        }
    }
    else
    { 
        // no operator_name, so give it a generic name
        snprintf (operator_name, GxB_MAX_NAME_LEN-1, "user_op") ;
    }

    // ensure operator_name is null-terminated
    operator_name [GxB_MAX_NAME_LEN-1] = '\0' ;

    // get the operator name length and hash the name
    (*operator_name_len)= strlen (operator_name) ;
    (*operator_hash) = GB_jitifyer_hash (operator_name, (*operator_name_len)) ;

    //--------------------------------------------------------------------------
    // get the definition of the operator, if present
    //--------------------------------------------------------------------------

    char *defn = NULL ;
    size_t defn_size = 0 ;
    if (input_defn != NULL)
    { 
        // determine the string length of the definition
        size_t defn_len = strlen (input_defn) ;

        // allocate space for the definition
        defn = GB_MALLOC (defn_len+1, char, &defn_size) ;
        if (defn == NULL)
        { 
            // out of memory
            return (GrB_OUT_OF_MEMORY) ;
        }

        // copy the defintion into the new operator
        memcpy (defn, input_defn, defn_len+1) ;
    }

    //--------------------------------------------------------------------------
    // return result
    //--------------------------------------------------------------------------

    (*operator_defn) = defn ;
    (*operator_defn_size) = defn_size ;
    return (GrB_SUCCESS) ;
}

