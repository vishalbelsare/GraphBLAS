//------------------------------------------------------------------------------
// GB_add_template:  phase1 and phase2 for C=A+B, C<M>=A+B, C<!M>=A+B
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2021, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// Computes C=A+B, C<M>=A+B, or C<!M>=A+B.

// M can have any sparsity structure:

//      If M is not present, bitmap, or full, then A and B are sparse or
//      hypersparse.  They are not bitmap or full, since in those cases,
//      C will not be sparse/hypersparse, and this method is not used.

//      Otherwise, if M is present and sparse/hypersparse, then A and B can
//      have any sparsity pattern (hyper, sparse, bitmap, or full).

// phase1: does not compute C itself, but just counts the # of entries in each
// vector of C.  Fine tasks compute the # of entries in their slice of a
// single vector of C, and the results are cumsum'd.

// phase2: computes C, using the counts computed by phase1.

#undef  GB_FREE_WORK
#define GB_FREE_WORK                        \
{                                           \
    GB_WERK_POP (B_ek_slicing, int64_t) ;   \
    GB_WERK_POP (A_ek_slicing, int64_t) ;   \
    GB_WERK_POP (M_ek_slicing, int64_t) ;   \
}

#undef  GB_FREE_ALL
#define GB_FREE_ALL                 \
{                                   \
    GB_FREE_WORK ;                  \
    GB_phbix_free (C) ;             \
}

{

    //--------------------------------------------------------------------------
    // get A, B, M, and C
    //--------------------------------------------------------------------------

    int taskid ;

    const int64_t *restrict Ap = A->p ;
    const int64_t *restrict Ah = A->h ;
    const int8_t  *restrict Ab = A->b ;
    const int64_t *restrict Ai = A->i ;
    const int64_t vlen = A->vlen ;
    const bool A_is_hyper = GB_IS_HYPERSPARSE (A) ;
    const bool A_is_sparse = GB_IS_SPARSE (A) ;
    const bool A_is_bitmap = GB_IS_BITMAP (A) ;
    const bool A_is_full = GB_as_if_full (A) ;
    int A_nthreads, A_ntasks ;

    const int64_t *restrict Bp = B->p ;
    const int64_t *restrict Bh = B->h ;
    const int8_t  *restrict Bb = B->b ;
    const int64_t *restrict Bi = B->i ;
    const bool B_is_hyper = GB_IS_HYPERSPARSE (B) ;
    const bool B_is_sparse = GB_IS_SPARSE (B) ;
    const bool B_is_bitmap = GB_IS_BITMAP (B) ;
    const bool B_is_full = GB_as_if_full (B) ;
    int B_nthreads, B_ntasks ;

    const int64_t *restrict Mp = NULL ;
    const int64_t *restrict Mh = NULL ;
    const int8_t  *restrict Mb = NULL ;
    const int64_t *restrict Mi = NULL ;
    const GB_void *restrict Mx = NULL ;
    const bool M_is_hyper = GB_IS_HYPERSPARSE (M) ;
    const bool M_is_sparse = GB_IS_SPARSE (M) ;
    const bool M_is_bitmap = GB_IS_BITMAP (M) ;
    const bool M_is_full = GB_as_if_full (M) ;
    const bool M_is_sparse_or_hyper = M_is_sparse || M_is_hyper ;
    int M_nthreads, M_ntasks ;
    size_t msize = 0 ;
    if (M != NULL)
    { 
        Mp = M->p ;
        Mh = M->h ;
        Mb = M->b ;
        Mi = M->i ;
        Mx = (GB_void *) (Mask_struct ? NULL : (M->x)) ;
        msize = M->type->size ;
    }

    #if defined ( GB_PHASE_2_OF_2 )
    const GB_ATYPE *restrict Ax = (GB_ATYPE *) A->x ;
    const GB_BTYPE *restrict Bx = (GB_BTYPE *) B->x ;
    const int64_t  *restrict Cp = C->p ;
    const int64_t  *restrict Ch = C->h ;
          int8_t   *restrict Cb = C->b ;
          int64_t  *restrict Ci = C->i ;
          GB_CTYPE *restrict Cx = (GB_CTYPE *) C->x ;
    // when C is bitmap or full:
    const int64_t cnz = GB_NNZ_HELD (C) ;
    GB_GET_NTHREADS_MAX (nthreads_max, chunk, Context) ;
    #endif

    //--------------------------------------------------------------------------
    // C=A+B, C<M>=A+B, or C<!M>=A+B: 3 cases for the sparsity of C
    //--------------------------------------------------------------------------

    #if defined ( GB_PHASE_1_OF_2 )

        // phase1: symbolic phase
        // C is sparse or hypersparse (never bitmap or full)
        // Werk allocated: none
        #include "GB_sparse_add_template.c"

    #else

        // phase2: numerical phase
        if (C_sparsity == GxB_SPARSE || C_sparsity == GxB_HYPERSPARSE)
        { 
            // C is sparse or hypersparse
            // Werk allocated: none
            #include "GB_sparse_add_template.c"
        }
        else if (C_sparsity == GxB_BITMAP)
        { 
            // C is bitmap (phase2 only)
            // Werk: slice M and A, M and B, just A, or just B, or none
            #include "GB_bitmap_add_template.c"
        }
        else
        { 
            // C is full (phase2 only)
            ASSERT (C_sparsity == GxB_FULL) ;
            // Werk: slice just A, just B, or none
            #include "GB_full_add_template.c"
        }

    #endif
}

