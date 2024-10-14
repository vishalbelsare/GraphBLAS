//------------------------------------------------------------------------------
// GB_mex_test37: index binary op tests
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2024, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_mex.h"
#include "GB_mex_errors.h"

#define GET_DEEP_COPY ;
#define FREE_DEEP_COPY ;

#define FREE_ALL                        \
{                                       \
    GrB_Scalar_free (&Theta) ;          \
    GrB_Scalar_free (&Alpha) ;          \
    GrB_Scalar_free (&Beta) ;           \
    GrB_Scalar_free (&Crud_Scalar) ;    \
    GrB_Type_free (&Crud_Type) ;        \
    GrB_Matrix_free (&A) ;              \
    GrB_Matrix_free (&M) ;              \
    GrB_Matrix_free (&A2) ;             \
    GrB_Matrix_free (&C1) ;             \
    GrB_Matrix_free (&C2) ;             \
    GrB_Matrix_free (&B1) ;             \
    GrB_Matrix_free (&B2) ;             \
    GrB_Matrix_free (&E1) ;             \
    GrB_Matrix_free (&E2) ;             \
    GrB_Matrix_free (&F1) ;             \
    GrB_Matrix_free (&F2) ;             \
    GrB_Matrix_free (&G1) ;             \
    GrB_Matrix_free (&G2) ;             \
    GrB_Matrix_free (&D) ;              \
    GrB_BinaryOp_free (&Bop) ;          \
    GzB_IndexBinaryOp_free (&Iop) ;     \
}

void test37_idxbinop (double *z,
    const double *x, GrB_Index ix, GrB_Index jx,
    const double *y, GrB_Index iy, GrB_Index jy,
    const double *theta) ;

void test37_idxbinop (double *z,
    const double *x, GrB_Index ix, GrB_Index jx,
    const double *y, GrB_Index iy, GrB_Index jy,
    const double *theta)
{
    (*z) = (*x) + 2*(*y) - 42*ix + jx + 3*iy + 1000*jy - (*theta) ;
}

#define TEST37_IDXBINOP                                                     \
"void test37_idxbinop (double *z,                                       \n" \
"    const double *x, GrB_Index ix, GrB_Index jx,                       \n" \
"    const double *y, GrB_Index iy, GrB_Index jy,                       \n" \
"    const double *theta)                                               \n" \
"{                                                                      \n" \
"    (*z) = (*x) + 2*(*y) - 42*ix + jx + 3*iy + 1000*jy - (*theta) ;    \n" \
"}                                                                      \n"

void mexFunction
(
    int nargout,
    mxArray *pargout [ ],
    int nargin,
    const mxArray *pargin [ ]
)
{

    //--------------------------------------------------------------------------
    // startup GraphBLAS
    //--------------------------------------------------------------------------

    GrB_Info info ;
    bool malloc_debug = GB_mx_get_global (true) ;

    //--------------------------------------------------------------------------
    // create index binary ops and test matrices
    //--------------------------------------------------------------------------

    GrB_Type Crud_Type = NULL ;
    GrB_Scalar Theta = NULL, Alpha = NULL, Beta = NULL, Crud_Scalar ;
    GzB_IndexBinaryOp Iop = NULL, Crud_Iop = NULL ;
    GrB_BinaryOp Bop = NULL, Crud_Bop = NULL ;
    GrB_Matrix A = NULL, C1 = NULL, C2 = NULL, B1 = NULL, B2 = NULL, D = NULL,
        E1 = NULL, E2 = NULL, A2 = NULL, F1 = NULL, F2 = NULL, M = NULL,
        G1 = NULL, G2 = NULL ;

    OK (GrB_Matrix_new (&A, GrB_FP64, 10, 10)) ;

    OK (GrB_Matrix_new (&C1, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&C2, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&B1, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&B2, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&E1, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&E2, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&F1, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&F2, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&G1, GrB_FP64, 10, 10)) ;
    OK (GrB_Matrix_new (&G2, GrB_FP64, 10, 10)) ;

    // C1 and B1 always stay by column
    OK (GrB_Matrix_set_INT32 (C1, GrB_COLMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;
    OK (GrB_Matrix_set_INT32 (B1, GrB_COLMAJOR, GrB_STORAGE_ORIENTATION_HINT)) ;

    double x = 1 ;
    for (int64_t i = 0 ; i < 9 ; i++)
    {
        OK (GrB_Matrix_setElement_FP64 (A, x, i, i)) ;
        x = x*1.2 ;
        OK (GrB_Matrix_setElement_FP64 (A, x, i, i+1)) ;
        x = x*1.2 ;
        OK (GrB_Matrix_setElement_FP64 (A, x, i+1, i)) ;
        x = x*1.2 ;
    }
    OK (GrB_Matrix_setElement_FP64 (A, x, 9, 9)) ;
    x = x - 1000 ;
    OK (GrB_Matrix_setElement_FP64 (A, x, 5, 2)) ;

    OK (GrB_Scalar_new (&Theta, GrB_FP64)) ;
    OK (GrB_Scalar_setElement_FP64 (Theta, x)) ;

    OK (GzB_IndexBinaryOp_new (&Iop,
        (GzB_index_binary_function) test37_idxbinop,
        GrB_FP64, GrB_FP64, GrB_FP64, GrB_FP64,
        "test37_idxbinop", TEST37_IDXBINOP)) ;

    OK (GzB_IndexBinaryOp_set_String (Iop, "test37 idx binop", GrB_NAME)) ;
    OK (GxB_print (Iop, 5)) ;

    size_t theta_type_namelen = 0 ;
    OK (GzB_IndexBinaryOp_get_SIZE (Iop, &theta_type_namelen,
        GzB_THETA_TYPE_STRING)) ;
    printf ("theta name length: %d\n", (int) theta_type_namelen) ;
    CHECK (theta_type_namelen == strlen ("GrB_FP64") + 1) ;

    char theta_type_name [256] ;
    theta_type_name [0] = '\0' ;
    OK (GzB_IndexBinaryOp_get_String (Iop, &theta_type_name,
        GzB_THETA_TYPE_STRING)) ;
    CHECK (strcmp (theta_type_name, "GrB_FP64") == 0)  ;

    int32_t theta_type_code = -1 ;
    OK (GzB_IndexBinaryOp_get_INT32 (Iop, &theta_type_code,
        GzB_THETA_TYPE_CODE)) ;
    CHECK (theta_type_code == GrB_FP64_CODE) ;

    OK (GrB_BinaryOp_get_INT32 (GxB_FIRSTI1_INT32, &theta_type_code,
        GzB_THETA_TYPE_CODE)) ;
    CHECK (theta_type_code == GrB_INT32_CODE) ;

    OK (GrB_BinaryOp_get_INT32 (GxB_FIRSTI1_INT64, &theta_type_code,
        GzB_THETA_TYPE_CODE)) ;
    CHECK (theta_type_code == GrB_INT64_CODE) ;

    OK (GzB_BinaryOp_new_IndexOp (&Bop, Iop, Theta)) ;
    OK (GxB_print (Bop, 5)) ;

    OK (GrB_Scalar_new (&Alpha, GrB_FP64)) ;

    double y = 0 ;
    int expected = GrB_INVALID_VALUE ;
    ERR (GzB_IndexBinaryOp_get_Scalar (Iop, Alpha, GzB_THETA)) ;

    y = 0 ;
    OK (GrB_Scalar_clear (Alpha)) ;
    OK (GrB_BinaryOp_get_Scalar (Bop, Alpha, GzB_THETA)) ;
    OK (GrB_Scalar_extractElement_FP64 (&y, Alpha)) ;
    CHECK (x == y) ;

    theta_type_code = -1 ;
    OK (GrB_BinaryOp_get_INT32 (Bop, &theta_type_code,
        GzB_THETA_TYPE_CODE)) ;
    CHECK (theta_type_code == GrB_FP64_CODE) ;

    theta_type_namelen = 0 ;
    OK (GrB_BinaryOp_get_SIZE (Bop, &theta_type_namelen,
        GzB_THETA_TYPE_STRING)) ;
    CHECK (theta_type_namelen == strlen ("GrB_FP64") + 1) ;

    theta_type_name [0] = '\0' ;
    OK (GrB_BinaryOp_get_String (Bop, &theta_type_name,
        GzB_THETA_TYPE_STRING)) ;
    CHECK (strcmp (theta_type_name, "GrB_FP64") == 0)  ;

    OK (GrB_Scalar_new (&Beta, GrB_FP64)) ;
    OK (GrB_Scalar_setElement_FP64 (Alpha, (double) 3.14159)) ;
    OK (GrB_Scalar_setElement_FP64 (Beta, (double) 42)) ;

    OK (GrB_Matrix_dup (&A2, A)) ;
    OK (GrB_Matrix_dup (&M, A)) ;

    OK (GrB_Matrix_set_INT32 (M, GxB_SPARSE,
        (GrB_Field) GxB_SPARSITY_CONTROL)) ;

    //--------------------------------------------------------------------------
    // test index binary ops
    //--------------------------------------------------------------------------

    for (int a1_sparsity = 0 ; a1_sparsity <= 1 ; a1_sparsity++)
    {
        for (int a2_sparsity = 0 ; a2_sparsity <= 1 ; a2_sparsity++)
        {
            for (int a1_store = 0 ; a1_store <= 1 ; a1_store++)
            {
                for (int a2_store = 0 ; a2_store <= 1 ; a2_store++)
                {
                    for (int c2_store = 0 ; c2_store <= 1 ; c2_store++)
                    {
                        for (int b2_store = 0 ; b2_store <= 1 ; b2_store++)
                        {
                            for (int jit = 0 ; jit <= 1 ; jit++)
                            {

                                printf (".") ;

                                // turn on/off the JIT
                                OK (GrB_Global_set_INT32 (GrB_GLOBAL,
                                    jit ? GxB_JIT_ON : GxB_JIT_OFF,
                                    (GrB_Field) GxB_JIT_C_CONTROL)) ;

                                // change A sparsity
                                OK (GrB_Matrix_set_INT32 (A,
                                    a1_sparsity ? GxB_SPARSE : GxB_BITMAP,
                                    (GrB_Field) GxB_SPARSITY_CONTROL)) ;

                                // change A storage orientation
                                OK (GrB_Matrix_set_INT32 (A,
                                    a1_store ? GrB_ROWMAJOR : GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;

                                // C1 = add (A, A')
                                OK (GrB_Matrix_eWiseAdd_BinaryOp (C1,
                                    NULL, NULL, Bop, A, A, GrB_DESC_T1)) ;
                                // B1 = union (A, A')
                                OK (GxB_Matrix_eWiseUnion (B1, NULL, NULL, Bop,
                                    A, Alpha, A, Beta, GrB_DESC_T1)) ;
                                // E1 = emult (A, A')
                                OK (GrB_Matrix_eWiseMult_BinaryOp (E1,
                                    NULL, NULL, Bop, A, A, GrB_DESC_T1)) ;
                                // F1 = emult (A, A')
                                OK (GrB_Matrix_eWiseMult_BinaryOp (F1,
                                    NULL, NULL, Bop, A, A2, GrB_DESC_T1)) ;
                                // G1<M> = emult (A, A')
                                OK (GrB_Matrix_eWiseMult_BinaryOp (G1,
                                    M, NULL, Bop, A, A2, GrB_DESC_RT1)) ;

                                // change A sparsity again
                                OK (GrB_Matrix_set_INT32 (A2,
                                    a2_sparsity ? GxB_SPARSE : GxB_BITMAP,
                                    (GrB_Field) GxB_SPARSITY_CONTROL)) ;

                                // change A storage again
                                OK (GrB_Matrix_set_INT32 (A,
                                    a2_store ? GrB_ROWMAJOR : GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;

                                // change C2, etc storage
                                OK (GrB_Matrix_set_INT32 (C2,
                                    c2_store ? GrB_ROWMAJOR : GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (B2,
                                    b2_store ? GrB_ROWMAJOR : GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (E2,
                                    b2_store ? GrB_ROWMAJOR : GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (F2,
                                    b2_store ? GrB_ROWMAJOR : GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (G2,
                                    b2_store ? GrB_ROWMAJOR : GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;

                                // C2 = add (A, A')
                                OK (GrB_Matrix_eWiseAdd_BinaryOp (C2,
                                    NULL, NULL, Bop, A, A, GrB_DESC_T1)) ;
                                // B2 = union (A, A')
                                OK (GxB_Matrix_eWiseUnion (B2, NULL, NULL,
                                    Bop, A, Alpha, A, Beta, GrB_DESC_T1)) ;
                                // E2 = emult (A, A')
                                OK (GrB_Matrix_eWiseMult_BinaryOp (E2,
                                    NULL, NULL, Bop, A, A, GrB_DESC_T1)) ;
                                // F2 = emult (A, A2')
                                OK (GrB_Matrix_eWiseMult_BinaryOp (F2,
                                    NULL, NULL, Bop, A, A2, GrB_DESC_T1)) ;
                                // G2<M> = emult (A, A2')
                                OK (GrB_Matrix_eWiseMult_BinaryOp (G2,
                                    M, NULL, Bop, A, A2, GrB_DESC_RT1)) ;

                                // change C2 etc to same storage as C1 etc
                                OK (GrB_Matrix_set_INT32 (C2, GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (B2, GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (E2, GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (F2, GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;
                                OK (GrB_Matrix_set_INT32 (G2, GrB_COLMAJOR,
                                    GrB_STORAGE_ORIENTATION_HINT)) ;

                                // FIXME: check C1, etc matrices

                                OK (GrB_Matrix_new (&D, GrB_FP64, 10, 10)) ;
                                OK (GrB_Matrix_eWiseAdd_BinaryOp (D, NULL,
                                    NULL, GrB_MINUS_FP64, C1, C2, NULL)) ;
                                OK (GrB_Matrix_select_FP64 (D, NULL, NULL,
                                    GrB_VALUENE_FP64, D, (double) 0, NULL)) ;
                                int64_t nvals ;
                                OK (GrB_Matrix_nvals (&nvals, D)) ;
                                // OK (GxB_print (D, 5)) ;
                                OK (GrB_Matrix_free (&D)) ;

                                CHECK (GB_mx_isequal (C1, C2, 0)) ;
                                CHECK (GB_mx_isequal (B1, B2, 0)) ;
                                CHECK (GB_mx_isequal (E1, E2, 0)) ;
                                CHECK (GB_mx_isequal (F1, F2, 0)) ;
                                CHECK (GB_mx_isequal (F1, E2, 0)) ;
                                CHECK (GB_mx_isequal (G1, G2, 0)) ;
                            }
                        }
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------
    // error tests
    //------------------------------------------------------------------------

    // turn on the JIT
    OK (GrB_Global_set_INT32 (GrB_GLOBAL, GxB_JIT_ON,
        (GrB_Field) GxB_JIT_C_CONTROL)) ;

    int save_jit = 0, save_burble = 0 ;
    bool save_fallback = false ;
    OK (GxB_get (GxB_JIT_C_CONTROL, &save_jit)) ;
    CHECK (save_jit == GxB_JIT_ON) ;

    printf ("\nerror handling tests: JIT is %d\n", save_jit) ;

    expected = GrB_INVALID_OBJECT ;
    void *p = Bop->theta_type = NULL ;
    Bop->theta_type = NULL ;
    ERR (GB_BinaryOp_check (Bop, "Bop: bad theta_type", 5, stdout)) ;
    Bop->theta_type = p ;

    p = Iop->idxbinop_function ;
    Iop->idxbinop_function = NULL ;
    ERR (GB_IndexBinaryOp_check (Iop, "Iop: null function", 5, stdout)) ;
    Iop->idxbinop_function = p ;

    p = Iop->ztype ;
    Iop->ztype = NULL ;
    ERR (GB_IndexBinaryOp_check (Iop, "Iop: null ztype", 5, stdout)) ;
    Iop->ztype = p ;

    p = Iop->xtype ;
    Iop->xtype = NULL ;
    ERR (GB_IndexBinaryOp_check (Iop, "Iop: null xtype", 5, stdout)) ;
    Iop->xtype = p ;

    p = Iop->ytype ;
    Iop->ytype = NULL ;
    ERR (GB_IndexBinaryOp_check (Iop, "Iop: null ytype", 5, stdout)) ;
    Iop->ytype = p ;

    p = Iop->theta_type ;
    Iop->theta_type = NULL ;
    ERR (GB_IndexBinaryOp_check (Iop, "Iop: null theta_type", 5, stdout)) ;
    Iop->theta_type = p ;

    GB_Opcode code = Iop->opcode ;
    Iop->opcode = GB_PLUS_binop_code ;
    ERR (GB_IndexBinaryOp_check (Iop, "Iop: invalid opcode", 5, stdout)) ;
    Iop->opcode = code ;

    int len = Iop->name_len ;
    Iop->name_len = 3 ;
    ERR (GB_IndexBinaryOp_check (Iop, "Iop: invalid name_len", 5, stdout)) ;
    Iop->name_len = len ;

    expected = GrB_NULL_POINTER ;
    ERR (GB_IndexBinaryOp_check (NULL, "Iop: null", 5, stdout)) ;

    expected = GrB_INVALID_VALUE ;
    ERR (GzB_IndexBinaryOp_set_Scalar (Iop, Theta, GrB_NAME)) ;
    ERR (GzB_IndexBinaryOp_set_INT32 (Iop, 2, GrB_SIZE)) ;
    ERR (GzB_IndexBinaryOp_set_VOID (Iop, NULL, GrB_SIZE, 0)) ;

    expected = GrB_DOMAIN_MISMATCH ;
    OK (GrB_Type_new (&Crud_Type, 4)) ;
    OK (GrB_Scalar_new (&Crud_Scalar, Crud_Type)) ;
    ERR (GzB_BinaryOp_new_IndexOp (&Crud_Bop, Iop, Crud_Scalar)) ;
    ERR (GrB_Matrix_apply (A, NULL, NULL, Bop, A, NULL)) ;

    //------------------------------------------------------------------------
    // JIT testing
    //------------------------------------------------------------------------

    printf ("\n\n-------------- lots of compiler errors expected here:\n") ;

    #define CRUD_IDXBINOP                               \
    "void crud_idxbinop (double *z, "                   \
    " const double *x, GrB_Index ix, GrB_Index jx, "    \
    " const double *y, GrB_Index iy, GrB_Index jy, "    \
    " const double *theta) "                            \
    "{ "                                                \
    "    compiler error occurs here "                   \
    "}"

    OK (GxB_get (GxB_JIT_C_CONTROL, &save_jit)) ;
    OK (GxB_get (GxB_JIT_ERROR_FALLBACK, &save_fallback)) ;
    OK (GxB_get (GxB_BURBLE, &save_burble)) ;

    OK (GxB_set (GxB_JIT_C_CONTROL, GxB_JIT_OFF)) ;
    OK (GxB_set (GxB_JIT_C_CONTROL, GxB_JIT_ON)) ;

    printf ("-------- test JIT with error fallback:\n") ;
    OK (GxB_set (GxB_JIT_C_CONTROL, GxB_JIT_ON)) ;
    OK (GxB_set (GxB_JIT_ERROR_FALLBACK, true)) ;
    OK (GxB_set (GxB_BURBLE, true)) ;

    expected = GrB_NULL_POINTER ;
    ERR (GzB_IndexBinaryOp_new (&Crud_Iop, NULL,
        GrB_FP64, GrB_FP64, GrB_FP64, GrB_FP64,
        "crud_idxbinop", CRUD_IDXBINOP)) ;

    printf ("-------- test JIT without error fallback:\n") ;
    OK (GxB_set (GxB_JIT_C_CONTROL, GxB_JIT_ON)) ;
    OK (GxB_set (GxB_JIT_ERROR_FALLBACK, false)) ;
    bool fallback = true ;
    OK (GxB_get (GxB_JIT_ERROR_FALLBACK, &fallback)) ;
    CHECK (!fallback) ;
    printf ("fallback is now: %d\n", fallback) ;

    expected = GxB_JIT_ERROR ;
    ERR (GzB_IndexBinaryOp_new (&Crud_Iop, NULL,
        GrB_FP64, GrB_FP64, GrB_FP64, GrB_FP64,
        "crud_idxbinop", "still more errors here")) ;

    OK (GxB_set (GxB_JIT_C_CONTROL, save_jit)) ;
    OK (GxB_set (GxB_JIT_ERROR_FALLBACK, &save_fallback)) ;
    OK (GxB_set (GxB_BURBLE, save_burble)) ;

    //------------------------------------------------------------------------
    // finalize GraphBLAS
    //------------------------------------------------------------------------

    FREE_ALL ;
    GB_mx_put_global (true) ;
    printf ("\nGB_mex_test37:  all tests passed\n\n") ;
}

