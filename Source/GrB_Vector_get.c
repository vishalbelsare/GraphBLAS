//------------------------------------------------------------------------------
// GrB_Vector_get_*: get a field in a matrix
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_get_set.h"

//------------------------------------------------------------------------------
// GrB_Vector_get_Scalar
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_get_Scalar
(
    GrB_Vector v,
    GrB_Scalar value,
    GrB_Field field
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GrB_Vector_get_Scalar (v, value, field)") ;
    GB_RETURN_IF_NULL_OR_FAULTY (v) ;
    GB_RETURN_IF_NULL_OR_FAULTY (value) ;
    ASSERT_VECTOR_OK (v, "v to get option", GB0) ;

    //--------------------------------------------------------------------------
    // get the field
    //--------------------------------------------------------------------------

    int i ;
    GrB_Info info = GB_matvec_get ((GrB_Matrix) v, &i, field) ;
    if (info == GrB_SUCCESS)
    { 
        // field specifies an int: assign it to the scalar
        info = GB_setElement ((GrB_Matrix) value, NULL, &i, 0, 0,
            GB_INT32_code, Werk) ;
    }
    else
    {
        float x ;
        switch ((int) field)
        {
            case GxB_BITMAP_SWITCH : 
                x = v->bitmap_switch ;
                break ;

            default : 
                return (GrB_INVALID_VALUE) ;
        }
        // field specifies a float: assign it to the scalar
        info = GB_setElement ((GrB_Matrix) value, NULL, &x, 0, 0,
            GB_FP32_code, Werk) ;
    }

    return (info) ;
}

//------------------------------------------------------------------------------
// GrB_Vector_get_String
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_get_String
(
    GrB_Vector v,
    char * value,
    GrB_Field field
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GrB_Vector_get_String (v, value, field)") ;
    GB_RETURN_IF_NULL_OR_FAULTY (v) ;
    GB_RETURN_IF_NULL (value) ;
    ASSERT_VECTOR_OK (v, "v to get option", GB0) ;

    //--------------------------------------------------------------------------
    // get the field
    //--------------------------------------------------------------------------

    return (GB_name_get ((GrB_Matrix) v, value, field)) ;
}

//------------------------------------------------------------------------------
// GrB_Vector_get_ENUM
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_get_ENUM
(
    GrB_Vector v,
    int * value,
    GrB_Field field
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GrB_Vector_get_ENUM (v, value, field)") ;
    GB_RETURN_IF_NULL_OR_FAULTY (v) ;
    GB_RETURN_IF_NULL (value) ;
    ASSERT_VECTOR_OK (v, "v to get option", GB0) ;

    //--------------------------------------------------------------------------
    // get the field
    //--------------------------------------------------------------------------

    return (GB_matvec_get ((GrB_Matrix) v, value, field)) ;
}

//------------------------------------------------------------------------------
// GrB_Vector_get_SIZE
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_get_SIZE
(
    GrB_Vector v,
    size_t * value,
    GrB_Field field
)
{ 

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    GB_WHERE1 ("GrB_Vector_get_SIZE (v, value, field)") ;
    GB_RETURN_IF_NULL_OR_FAULTY (v) ;
    GB_RETURN_IF_NULL (value) ;
    ASSERT_VECTOR_OK (v, "v to get option", GB0) ;

    //--------------------------------------------------------------------------
    // get the field
    //--------------------------------------------------------------------------

    return (GB_name_size_get (value, field)) ;
}

//------------------------------------------------------------------------------
// GrB_Vector_get_VOID
//------------------------------------------------------------------------------

GrB_Info GrB_Vector_get_VOID
(
    GrB_Vector v,
    void * value,
    GrB_Field field
)
{ 
    return (GrB_NOT_IMPLEMENTED) ;
}

