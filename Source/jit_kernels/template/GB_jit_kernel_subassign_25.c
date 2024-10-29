//------------------------------------------------------------------------------
// GB_jit_kernel_subassign_25.c: C(:,:)<M,s> = A ; M struct, A bitmap/full
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

// Method 25: C(:,:)<M,s> = A ; C is empty, M structural, A bitmap/as-if-full

// M:           present
// Mask_comp:   false
// Mask_struct: true
// C_replace:   effectively false (not relevant since C is empty)
// accum:       NULL
// A:           matrix
// S:           none
// I:           NULL
// J:           NULL

// C and M are sparse or hypersparse.  A can have any sparsity structure, even
// bitmap, but it must either be bitmap, or as-if-full.  M may be jumbled.  If
// so, C is constructed as jumbled.  C is reconstructed with the same structure
// as M and can have any sparsity structure on input.  The only constraint on C
// is nnz(C) is zero on input.

// C is iso if A is iso

GB_JIT_GLOBAL GB_JIT_KERNEL_SUBASSIGN_PROTO (GB_jit_kernel) ;
GB_JIT_GLOBAL GB_JIT_KERNEL_SUBASSIGN_PROTO (GB_jit_kernel)
{
    // get callback functions
    GB_GET_CALLBACK (GB_free_memory) ;
    GB_GET_CALLBACK (GB_malloc_memory) ;
    GB_GET_CALLBACK (GB_ek_slice) ;
    GB_GET_CALLBACK (GB_werk_pop) ;
    GB_GET_CALLBACK (GB_werk_push) ;

    #include "template/GB_subassign_25_template.c"
    return (GrB_SUCCESS) ;
}

