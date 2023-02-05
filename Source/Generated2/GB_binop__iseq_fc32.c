
//------------------------------------------------------------------------------
// GB_binop:  hard-coded functions for each built-in binary operator
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2022, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// If this file is in the Generated2/ folder, do not edit it
// (it is auto-generated from Generator/*).

#include "GB.h"
#ifndef GBCUDA_DEV
#include "GB_emult.h"
#include "GB_control.h"
#include "GB_ek_slice.h"
#include "GB_dense.h"
#include "GB_bitmap_assign_methods.h"
#include "GB_binop__include.h"

// C=binop(A,B) is defined by the following types and operators:

// A+B function (eWiseAdd):         GB (_AaddB__iseq_fc32)
// A.*B function (eWiseMult):       GB (_AemultB__iseq_fc32)
// A.*B function (eWiseMult):       GB (_AemultB_02__iseq_fc32)
// A.*B function (eWiseMult):       GB (_AemultB_04__iseq_fc32)
// A.*B function (eWiseMult):       GB (_AemultB_bitmap__iseq_fc32)
// A*D function (colscale):         GB ((none))
// D*A function (rowscale):         GB ((none))
// C+=B function (dense accum):     GB (_Cdense_accumB__iseq_fc32)
// C+=b function (dense accum):     GB (_Cdense_accumb__iseq_fc32)
// C+=A+B function (dense ewise3):  GB ((none))
// C=A+B function (dense ewise3):   GB (_Cdense_ewise3_noaccum__iseq_fc32)
// C=scalar+B                       GB (_bind1st__iseq_fc32)
// C=scalar+B'                      GB (_bind1st_tran__iseq_fc32)
// C=A+scalar                       GB (_bind2nd__iseq_fc32)
// C=A'+scalar                      GB (_bind2nd_tran__iseq_fc32)

// C type:     GxB_FC32_t
// A type:     GxB_FC32_t
// A pattern?  0
// B type:     GxB_FC32_t
// B pattern?  0

// BinaryOp:   cij = GB_FC32_iseq (aij, bij)

#define GB_A_TYPE \
    GxB_FC32_t

#define GB_B_TYPE \
    GxB_FC32_t

#define GB_C_TYPE \
    GxB_FC32_t

// true if the types of A and B are identical
#define GB_ATYPE_IS_BTYPE \
    1

// true if the types of C and A are identical
#define GB_CTYPE_IS_ATYPE \
    1

// true if the types of C and B are identical
#define GB_CTYPE_IS_BTYPE \
    1

// declare aij as atype
#define GB_DECLAREA(aij) \
    GxB_FC32_t aij

// aij = Ax [pA]
#define GB_GETA(aij,Ax,pA,A_iso)  \
    aij = GBX (Ax, pA, A_iso)

// true if values of A are not used
#define GB_A_IS_PATTERN \
    0 \

// declare bij as btype
#define GB_DECLAREB(bij)  \
    GxB_FC32_t bij

// bij = Bx [pB]
#define GB_GETB(bij,Bx,pB,B_iso)  \
    bij = GBX (Bx, pB, B_iso)

// true if values of B are not used
#define GB_B_IS_PATTERN \
    0 \

// declare scalar of the same type as C
#define GB_CTYPE_SCALAR(t)  \
    GxB_FC32_t t

// cij = Ax [pA]
#define GB_COPY_A_TO_C(cij,Ax,pA,A_iso) \
    cij = GBX (Ax, pA, A_iso)

// cij = Bx [pB]
#define GB_COPY_B_TO_C(cij,Bx,pB,B_iso) \
    cij = GBX (Bx, pB, B_iso)

#define GB_CX(p) Cx [p]

// binary operator
#define GB_BINOP(z,x,y,i,j) \
    z = GB_FC32_iseq (x, y) ;

// true if the binop must be flipped
#define GB_BINOP_FLIP \
    0

// op is second
#define GB_OP_IS_SECOND \
    0

// do the numerical phases of GB_add and GB_emult
#define GB_PHASE_2_OF_2

// hard-coded loops can be vectorized
#define GB_PRAGMA_SIMD_VECTORIZE GB_PRAGMA_SIMD

// disable this operator and use the generic case if these conditions hold
#define GB_DISABLE \
    (GxB_NO_ISEQ || GxB_NO_FC32 || GxB_NO_ISEQ_FC32)

//------------------------------------------------------------------------------
// C += A+B, all 3 matrices dense
//------------------------------------------------------------------------------

#if 0

// The op must be MIN, MAX, PLUS, MINUS, RMINUS, TIMES, DIV, or RDIV.

void GB ((none))
(
    GrB_Matrix C,
    const GrB_Matrix A,
    const GrB_Matrix B,
    const int nthreads
)
{ 
    #include "GB_dense_ewise3_accum_template.c"
}

#endif

//------------------------------------------------------------------------------
// C = A+B, all 3 matrices dense
//------------------------------------------------------------------------------

void GB (_Cdense_ewise3_noaccum__iseq_fc32)
(
    GrB_Matrix C,
    const GrB_Matrix A,
    const GrB_Matrix B,
    const int nthreads
)
{ 
    #include "GB_dense_ewise3_noaccum_template.c"
}

//------------------------------------------------------------------------------
// C += B, accumulate a sparse matrix into a dense matrix
//------------------------------------------------------------------------------

GrB_Info GB (_Cdense_accumB__iseq_fc32)
(
    GrB_Matrix C,
    const GrB_Matrix B,
    const int64_t *B_ek_slicing, const int B_ntasks, const int B_nthreads
)
{
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    
    { 
        #include "GB_dense_subassign_23_template.c"
    }
    
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C += b, accumulate a scalar into a dense matrix
//------------------------------------------------------------------------------

GrB_Info GB (_Cdense_accumb__iseq_fc32)
(
    GrB_Matrix C,
    const GB_void *p_bwork,
    const int nthreads
)
{
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    
    { 
        // get the scalar b for C += b, of type GxB_FC32_t
        GxB_FC32_t bwork = (*((GxB_FC32_t *) p_bwork)) ;
        #include "GB_dense_subassign_22_template.c"
        return (GrB_SUCCESS) ;
    }
    
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// C = A*D, column scale with diagonal D matrix
//------------------------------------------------------------------------------

#if 0

GrB_Info GB ((none))
(
    GrB_Matrix C,
    const GrB_Matrix A,
    const GrB_Matrix D,
    const int64_t *A_ek_slicing, const int A_ntasks, const int A_nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    GxB_FC32_t *restrict Cx = (GxB_FC32_t *) C->x ;
    #include "GB_AxB_colscale_template.c"
    return (GrB_SUCCESS) ;
    #endif
}

#endif

//------------------------------------------------------------------------------
// C = D*B, row scale with diagonal D matrix
//------------------------------------------------------------------------------

#if 0

GrB_Info GB ((none))
(
    GrB_Matrix C,
    const GrB_Matrix D,
    const GrB_Matrix B,
    int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    GxB_FC32_t *restrict Cx = (GxB_FC32_t *) C->x ;
    #include "GB_AxB_rowscale_template.c"
    return (GrB_SUCCESS) ;
    #endif
}

#endif

//------------------------------------------------------------------------------
// eWiseAdd: C=A+B, C<M>=A+B, C<!M>=A+B
//------------------------------------------------------------------------------

GrB_Info GB (_AaddB__iseq_fc32)
(
    GrB_Matrix C,
    const int C_sparsity,
    const GrB_Matrix M,
    const bool Mask_struct,
    const bool Mask_comp,
    const GrB_Matrix A,
    const GrB_Matrix B,
    const bool is_eWiseUnion,
    const GB_void *alpha_scalar_in,
    const GB_void *beta_scalar_in,
    const bool Ch_is_Mh,
    const int64_t *restrict C_to_M,
    const int64_t *restrict C_to_A,
    const int64_t *restrict C_to_B,
    const GB_task_struct *restrict TaskList,
    const int C_ntasks,
    const int C_nthreads,
    GB_Werk Werk
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    GB_WERK_DECLARE (M_ek_slicing, int64_t) ;
    GB_WERK_DECLARE (A_ek_slicing, int64_t) ;
    GB_WERK_DECLARE (B_ek_slicing, int64_t) ;
    GxB_FC32_t alpha_scalar ;
    GxB_FC32_t beta_scalar ;
    if (is_eWiseUnion)
    {
        alpha_scalar = (*((GxB_FC32_t *) alpha_scalar_in)) ;
        beta_scalar  = (*((GxB_FC32_t *) beta_scalar_in )) ;
    }
    #include "GB_add_template.c"
    GB_FREE_WORKSPACE ;
    return (GrB_SUCCESS) ;
    #endif
}

//------------------------------------------------------------------------------
// eWiseMult: C=A.*B, C<M>=A.*B, or C<M!>=A.*B where C is sparse/hyper
//------------------------------------------------------------------------------



GrB_Info GB (_AemultB__iseq_fc32)
(
    GrB_Matrix C,
    const int C_sparsity,
    const int ewise_method,
    const GrB_Matrix M,
    const bool Mask_struct,
    const bool Mask_comp,
    const GrB_Matrix A,
    const GrB_Matrix B,
    const int64_t *restrict C_to_M,
    const int64_t *restrict C_to_A,
    const int64_t *restrict C_to_B,
    const GB_task_struct *restrict TaskList,
    const int C_ntasks,
    const int C_nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_emult_meta.c"
    return (GrB_SUCCESS) ;
    #endif
}



//------------------------------------------------------------------------------
// eWiseMult: C<#> = A.*B when A is sparse/hyper and B is bitmap/full
//------------------------------------------------------------------------------



GrB_Info GB (_AemultB_02__iseq_fc32)
(
    GrB_Matrix C,
    const GrB_Matrix M,
    const bool Mask_struct,
    const bool Mask_comp,
    const GrB_Matrix A,
    const GrB_Matrix B,
    const bool flipxy,
    const int64_t *restrict Cp_kfirst,
    const int64_t *A_ek_slicing, const int A_ntasks, const int A_nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #if GB_BINOP_FLIP
        // The operator is not commutative, and does not have a flipped
        // variant.  For example z=atan2(y,x).
        if (flipxy)
        {
            // use fmult(y,x)
            #undef  GB_FLIPPED
            #define GB_FLIPPED 1
            #include "GB_emult_02_template.c"
        }
        else
        {
            // use fmult(x,y)
            #undef  GB_FLIPPED
            #define GB_FLIPPED 0
            #include "GB_emult_02_template.c"
        }
    #else
        // No need to handle the flip: the operator is either commutative, or
        // has been handled by changing z=div(y,x) to z=rdiv(x,y) for example.
        #undef  GB_FLIPPED
        #define GB_FLIPPED 0
        #include "GB_emult_02_template.c"
    #endif
    return (GrB_SUCCESS) ;
    #endif
}



//------------------------------------------------------------------------------
// eWiseMult: C<M> = A.*B, M sparse/hyper, A and B bitmap/full
//------------------------------------------------------------------------------



GrB_Info GB (_AemultB_04__iseq_fc32)
(
    GrB_Matrix C,
    const GrB_Matrix M,
    const bool Mask_struct,
    const GrB_Matrix A,
    const GrB_Matrix B,
    const int64_t *restrict Cp_kfirst,
    const int64_t *M_ek_slicing, const int M_ntasks, const int M_nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_emult_04_template.c"
    return (GrB_SUCCESS) ;
    #endif
}



//------------------------------------------------------------------------------
// eWiseMult: C=A.*B, C<M>=A.*B, C<!M>=A.*B where C is bitmap
//------------------------------------------------------------------------------



GrB_Info GB (_AemultB_bitmap__iseq_fc32)
(
    GrB_Matrix C,
    const int ewise_method,
    const GrB_Matrix M,
    const bool Mask_struct,
    const bool Mask_comp,
    const GrB_Matrix A,
    const GrB_Matrix B,
    const int64_t *M_ek_slicing, const int M_ntasks, const int M_nthreads,
    const int C_nthreads,
    GB_Werk Werk
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    #include "GB_bitmap_emult_template.c"
    return (GrB_SUCCESS) ;
    #endif
}



//------------------------------------------------------------------------------
// Cx = op (x,Bx):  apply a binary operator to a matrix with scalar bind1st
//------------------------------------------------------------------------------



GrB_Info GB (_bind1st__iseq_fc32)
(
    GB_void *Cx_output,         // Cx and Bx may be aliased
    const GB_void *x_input,
    const GB_void *Bx_input,
    const int8_t *restrict Bb,
    int64_t bnz,
    int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    GxB_FC32_t *Cx = (GxB_FC32_t *) Cx_output ;
    GxB_FC32_t   x = (*((GxB_FC32_t *) x_input)) ;
    GxB_FC32_t *Bx = (GxB_FC32_t *) Bx_input ;
    int64_t p ;
    #pragma omp parallel for num_threads(nthreads) schedule(static)
    for (p = 0 ; p < bnz ; p++)
    {
        if (!GBB (Bb, p)) continue ;
        GxB_FC32_t bij ;
        bij = GBX (Bx, p, false) ;
        Cx [p] = GB_FC32_iseq (x, bij) ;
    }
    return (GrB_SUCCESS) ;
    #endif
}



//------------------------------------------------------------------------------
// Cx = op (Ax,y):  apply a binary operator to a matrix with scalar bind2nd
//------------------------------------------------------------------------------



GrB_Info GB (_bind2nd__iseq_fc32)
(
    GB_void *Cx_output,         // Cx and Ax may be aliased
    const GB_void *Ax_input,
    const GB_void *y_input,
    const int8_t *restrict Ab,
    int64_t anz,
    int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    int64_t p ;
    GxB_FC32_t *Cx = (GxB_FC32_t *) Cx_output ;
    GxB_FC32_t *Ax = (GxB_FC32_t *) Ax_input ;
    GxB_FC32_t   y = (*((GxB_FC32_t *) y_input)) ;
    #pragma omp parallel for num_threads(nthreads) schedule(static)
    for (p = 0 ; p < anz ; p++)
    {
        if (!GBB (Ab, p)) continue ;
        GxB_FC32_t aij ;
        aij = GBX (Ax, p, false) ;
        Cx [p] = GB_FC32_iseq (aij, y) ;
    }
    return (GrB_SUCCESS) ;
    #endif
}



//------------------------------------------------------------------------------
// C = op (x, A'): transpose and apply a binary operator
//------------------------------------------------------------------------------



// cij = op (x, aij), no typecasting (in spite of the macro name)
#undef  GB_CAST_OP
#define GB_CAST_OP(pC,pA)                       \
{                                               \
    GxB_FC32_t aij ;                          \
    aij = GBX (Ax, pA, false) ;               \
    Cx [pC] = GB_FC32_iseq (x, aij) ;        \
}

GrB_Info GB (_bind1st_tran__iseq_fc32)
(
    GrB_Matrix C,
    const GB_void *x_input,
    const GrB_Matrix A,
    int64_t *restrict *Workspaces,
    const int64_t *restrict A_slice,
    int nworkspaces,
    int nthreads
)
{ 
    // GB_unop_transpose.c uses GB_A_TYPE, but A is
    // the 2nd input to binary operator z=f(x,y).
    #undef  GB_A_TYPE
    #define GB_A_TYPE \
    GxB_FC32_t
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    GxB_FC32_t x = (*((const GxB_FC32_t *) x_input)) ;
    #include "GB_unop_transpose.c"
    return (GrB_SUCCESS) ;
    #endif
    #undef  GB_A_TYPE
    #define GB_A_TYPE \
    GxB_FC32_t
}



//------------------------------------------------------------------------------
// C = op (A', y): transpose and apply a binary operator
//------------------------------------------------------------------------------



// cij = op (aij, y), no typecasting (in spite of the macro name)
#undef  GB_CAST_OP
#define GB_CAST_OP(pC,pA)                       \
{                                               \
    GxB_FC32_t aij ;                          \
    aij = GBX (Ax, pA, false) ;               \
    Cx [pC] = GB_FC32_iseq (aij, y) ;        \
}

GrB_Info GB (_bind2nd_tran__iseq_fc32)
(
    GrB_Matrix C,
    const GrB_Matrix A,
    const GB_void *y_input,
    int64_t *restrict *Workspaces,
    const int64_t *restrict A_slice,
    int nworkspaces,
    int nthreads
)
{ 
    #if GB_DISABLE
    return (GrB_NO_VALUE) ;
    #else
    GxB_FC32_t y = (*((const GxB_FC32_t *) y_input)) ;
    #include "GB_unop_transpose.c"
    return (GrB_SUCCESS) ;
    #endif
}



#endif

