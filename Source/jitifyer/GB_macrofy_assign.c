//------------------------------------------------------------------------------
// GB_macrofy_assign: construct all macros for assign methods
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2023, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#define GB_DEBUG    /* HACK FIXME */

#include "GB.h"
#include "jitifyer/GB_stringify.h"

void GB_macrofy_assign          // construct all macros for GrB_assign
(
    // output:
    FILE *fp,                   // target file to write, already open
    // input:
    uint64_t scode,
    GrB_BinaryOp accum,         // accum operator to macrofy
    GrB_Type ctype,
    GrB_Type atype              // matrix or scalar type
)
{

    //--------------------------------------------------------------------------
    // extract the assign scode
    //--------------------------------------------------------------------------

    // S sparsity (2 bits, 1 hex digit)
    int ssparsity   = GB_RSHIFT (scode, 48, 2) ;

    // assign_kind, Ikind, Jkind, and S present (2 hex digits)
    int S_present   = GB_RSHIFT (scode, 47, 1) ;
    int C_replace   = GB_RSHIFT (scode, 46, 1) ;
    int assign_kind = GB_RSHIFT (scode, 44, 2) ;
    int Ikind       = GB_RSHIFT (scode, 42, 2) ;
    int Jkind       = GB_RSHIFT (scode, 40, 2) ;

    // binary operator (5 hex digits)
    int accum_ecode = GB_RSHIFT (scode, 32, 8) ;
//  int zcode       = GB_RSHIFT (scode, 28, 4) ;
    int xcode       = GB_RSHIFT (scode, 24, 4) ;
//  int ycode       = GB_RSHIFT (scode, 20, 4) ;

    // mask (one hex digit)
    int mask_ecode  = GB_RSHIFT (scode, 16, 4) ;

    // types of C and A (or scalar type) (2 hex digits)
    int ccode       = GB_RSHIFT (scode, 12, 4) ;   // if 0: C is iso
    int acode       = GB_RSHIFT (scode,  8, 4) ;

    bool C_iso = (ccode == 0) ;

    // sparsity structures of C, M, and A (2 hex digits),
    // iso status of A and scalar assignment
    int csparsity   = GB_RSHIFT (scode,  6, 2) ;
    int msparsity   = GB_RSHIFT (scode,  4, 2) ;
    bool s_assign   = GB_RSHIFT (scode,  3, 1) ;
    int A_iso       = GB_RSHIFT (scode,  2, 1) ;
    int asparsity   = GB_RSHIFT (scode,  0, 2) ;

    //--------------------------------------------------------------------------
    // describe the assignment
    //--------------------------------------------------------------------------

    #define SLEN 512
    char description [SLEN] ;
    bool Mask_comp = (mask_ecode % 2 == 1) ;
    bool Mask_struct = (mask_ecode <= 3) ;
    bool M_is_null = (mask_ecode == 0) ;
    int M_sparsity ;
    switch (msparsity)
    {
        default :
        case 0 : M_sparsity = GxB_HYPERSPARSE ; break ;
        case 1 : M_sparsity = GxB_SPARSE      ; break ;
        case 2 : M_sparsity = GxB_BITMAP      ; break ;
        case 3 : M_sparsity = GxB_FULL        ; break ;
    }

    switch (assign_kind)
    {
        case GB_ASSIGN     : fprintf (fp, "// assign: "     ) ; break ;
        case GB_SUBASSIGN  : fprintf (fp, "// subassign: "  ) ; break ;
        case GB_ROW_ASSIGN : fprintf (fp, "// row assign: " ) ; break ;
        case GB_COL_ASSIGN : fprintf (fp, "// col assign: " ) ; break ;
        default:;
    }
    GB_assign_describe (description, SLEN, C_replace, Ikind, Jkind,
        M_is_null, M_sparsity, Mask_comp, Mask_struct, accum, s_assign,
        assign_kind) ;
    fprintf (fp, "%s\n", description) ;

    fprintf (fp, "#define GB_ASSIGN_KIND ") ;
    switch (assign_kind)
    {
        case GB_ASSIGN     : fprintf (fp, "GB_ASSIGN\n"     ) ; break ;
        case GB_SUBASSIGN  : fprintf (fp, "GB_SUBASSIGN\n"  ) ; break ;
        case GB_ROW_ASSIGN : fprintf (fp, "GB_ROW_ASSIGN\n" ) ; break ;
        case GB_COL_ASSIGN : fprintf (fp, "GB_COL_ASSIGN\n" ) ; break ;
        default:;
    }

    fprintf (fp, "#define GB_SCALAR_ASSIGN %d\n", s_assign ? 1 : 0) ;

    fprintf (fp, "#define GB_I_KIND ") ;
    switch (Ikind)
    {
        case GB_ALL    : fprintf (fp, "GB_ALL\n"    ) ; break ;
        case GB_RANGE  : fprintf (fp, "GB_RANGE\n"  ) ; break ;
        case GB_STRIDE : fprintf (fp, "GB_STRIDE\n" ) ; break ;
        case GB_LIST   : fprintf (fp, "GB_LIST\n"   ) ; break ;
        default:;
    }

    fprintf (fp, "#define GB_J_KIND ") ;
    switch (Jkind)
    {
        case GB_ALL    : fprintf (fp, "GB_ALL\n"    ) ; break ;
        case GB_RANGE  : fprintf (fp, "GB_RANGE\n"  ) ; break ;
        case GB_STRIDE : fprintf (fp, "GB_STRIDE\n" ) ; break ;
        case GB_LIST   : fprintf (fp, "GB_LIST\n"   ) ; break ;
        default:;
    }

    fprintf (fp, "#define GB_C_REPLACE %d\n", C_replace) ;

    //--------------------------------------------------------------------------
    // describe the accum operator
    //--------------------------------------------------------------------------

    GrB_Type xtype, ytype, ztype ;
    const char *xtype_name, *ytype_name, *ztype_name ;

    fprintf (fp, "\n// accum: ") ;
    if (accum == NULL)
    { 
        // accum operator is not present
        xtype_name = "GB_void" ;
        ytype_name = "GB_void" ;
        ztype_name = "GB_void" ;
        xtype = NULL ;
        ytype = NULL ;
        ztype = NULL ;
        fprintf (fp, "not present\n\n") ;
    }
    else
    { 
        // accum operator is present
        xtype = accum->xtype ;
        ytype = accum->ytype ;
        ztype = accum->ztype ;
        xtype_name = xtype->name ;
        ytype_name = ytype->name ;
        ztype_name = ztype->name ;
        if (accum->hash == 0)
        { 
            // builtin operator
            fprintf (fp, "(%s, %s)\n\n", accum->name, xtype_name) ;
        }
        else
        { 
            // user-defined operator
            fprintf (fp,
                "%s, ztype: %s, xtype: %s, ytype: %s\n\n",
                accum->name, ztype_name, xtype_name, ytype_name) ;
        }
    }

    //--------------------------------------------------------------------------
    // construct the typedefs
    //--------------------------------------------------------------------------

    GB_macrofy_typedefs (fp, ctype, atype, NULL, xtype, ytype, ztype) ;

    if (accum != NULL)
    { 
        fprintf (fp, "// accum operator types:\n") ;
        GB_macrofy_type (fp, "Z", "_", ztype_name) ;
        GB_macrofy_type (fp, "X", "_", xtype_name) ;
        GB_macrofy_type (fp, "Y", "_", ytype_name) ;
        fprintf (fp, "#define GB_DECLAREZ(zwork) %s zwork\n", ztype_name) ;
        fprintf (fp, "#define GB_DECLAREX(xwork) %s xwork\n", xtype_name) ;
        fprintf (fp, "#define GB_DECLAREY(ywork) %s ywork\n", ytype_name) ;
    }

    //--------------------------------------------------------------------------
    // construct macros for the accum operator
    //--------------------------------------------------------------------------

    bool did_accum_scalar = false ;
    bool did_accum_aij = false ;
    bool need_copy_c_to_xwork = false ;

    if (accum != NULL)
    {
        fprintf (fp, "\n// accum operator:\n") ;

        GB_Opcode accum_opcode = accum->opcode ;
        if (xcode == GB_BOOL_code)  // && (ycode == GB_BOOL_code)
        { 
            // rename the operator
            accum_opcode = GB_boolean_rename (accum_opcode) ;
        }
        int accum_ecode2 ;
        GB_enumify_binop (&accum_ecode2, accum_opcode, xcode, false, false) ;
        ASSERT (accum_ecode2 == accum_ecode) ;

        GB_macrofy_binop (fp, "GB_ACCUM_OP", false, false, true, false, false,
            accum_ecode, C_iso, accum, NULL, NULL, NULL) ;

        char *yname = "ywork" ;

        if (s_assign)
        {
            did_accum_scalar = true ;
            fprintf (fp, "#define GB_ACCUMULATE_scalar(Cx,pC,ywork,C_iso)") ;
            if (C_iso)
            { 
                fprintf (fp, "\n") ;
            }
            else
            { 
                fprintf (fp, " \\\n"
                    "{                                          \\\n") ;
            }
            // the scalar has already been typecasted into ywork
        }
        else
        {
            did_accum_aij = true ;
            fprintf (fp,
                "#define GB_ACCUMULATE_aij(Cx,pC,Ax,pA,A_iso,ywork,C_iso)") ;
            if (C_iso)
            { 
                fprintf (fp, "\n") ;
            }
            else
            { 
                fprintf (fp, " \\\n"
                    "{                                          \\\n") ;
                // if A is iso, its iso value is already typecasted into ywork
                if (!A_iso)
                {
                    if (atype == ytype)
                    { 
                        // use Ax [pA] directly instead of ywork
                        yname = "Ax [pA]" ;
                    }
                    else
                    { 
                        // ywork = (ytype) Ax [pA]
                        fprintf (fp,
                        "    GB_DECLAREY (ywork) ;                  \\\n"
                        "    GB_GETA (ywork, Ax, pA, ) ;            \\\n") ;
                    }
                }
            }
        }

        if (!C_iso)
        {
            char *xname ;
            if (xtype == ctype)
            { 
                // use Cx [pC] directly
                xname = "Cx [pC]" ;
            }
            else
            { 
                // xwork = (xtype) Cx [pC]
                need_copy_c_to_xwork = true ;
                xname = "xwork" ;
                fprintf (fp,
                    "    GB_DECLAREX (xwork) ;                  \\\n"
                    "    GB_COPY_C_to_xwork (xwork, Cx, pC) ;   \\\n") ;
            }
            if (ztype == ctype)
            {
                // write directly in Cx [pC], no need for zwork
                if (xtype == ctype)
                { 
                    // use the update method: Cx [pC] += y
                    fprintf (fp,
                    "    GB_UPDATE (Cx [pC], %s) ;          \\\n"
                    "}\n", yname) ;
                }
                else
                { 
                    // Cx [pC] = f (x,y)
                    fprintf (fp,
                    "    GB_ACCUM_OP (Cx [pC], %s, %s) ;          \\\n"
                    "}\n", xname, yname) ;
                }
            }
            else
            { 
                // zwork = f (x,y)
                // Cx [pC] = (ctype) zwork
                fprintf (fp,
                "    GB_DECLAREZ (zwork) ;                  \\\n"
                "    GB_ACCUM_OP (zwork, %s, %s) ;          \\\n"
                "    GB_PUTC (zwork, Cx, pC) ;              \\\n"
                "}\n", xname, yname) ;
            }
        }
    }

    if (!did_accum_scalar)
    { 
        fprintf (fp, "#define GB_ACCUMULATE_scalar(Cx,pC,ywork,C_iso)"
            " /* unused */\n") ;
    }

    if (!did_accum_aij)
    { 
        fprintf (fp, "#define GB_ACCUMULATE_aij(Cx,pC,Ax,pA,A_iso,ywork,C_iso)"
            " /* unused */\n") ;
    }

    //--------------------------------------------------------------------------
    // macros for the C matrix
    //--------------------------------------------------------------------------

    if (accum == NULL)
    { 
        // C(i,j) = (ctype) cwork, no typecasting
        GB_macrofy_output (fp, "cwork", "C", "C", ctype, ctype, csparsity,
            C_iso, C_iso) ;
    }
    else
    { 
        // C(i,j) = (ctype) zwork, with possible typecasting
        GB_macrofy_output (fp, "zwork", "C", "C", ctype, ztype, csparsity,
            C_iso, C_iso) ;
    }

    fprintf (fp, "#define GB_DECLAREC(cwork) %s cwork\n", ctype->name) ;

    if (s_assign)
    { 
        // cwork = (ctype) scalar
        GB_macrofy_cast_input (fp, "GB_COPY_scalar_to_cwork", "cwork",
            "scalar", "(*((GB_A_TYPE *) scalar))", ctype, atype) ;
        // C(i,j) = (ctype) scalar, already typecasted to cwork
        fprintf (fp, "#define GB_COPY_cwork_to_C(Cx,pC,cwork,C_iso)%s",
            C_iso ? "\n" : " Cx [pC] = cwork\n") ;
        // no copy of A(i,j) to cwork
        fprintf (fp, "#define GB_COPY_aij_to_cwork(cwork,Ax,pA,A_iso)"
            " /* unused */\n") ;
        // no copy of A(i,j) to C(i,j)
        fprintf (fp, "#define GB_COPY_aij_to_C(Cx,pC,Ax,pA,A_iso,cwork,C_iso)"
            " /* unused */\n") ;
    }
    else
    {
        // C(i,j) = (ctype) A(i,j)
        GB_macrofy_cast_copy (fp, "C", "A", (C_iso) ? NULL : ctype, atype,
            A_iso) ;
        fprintf (fp, "#define GB_COPY_aij_to_C(Cx,pC,Ax,pA,A_iso,cwork,C_iso)");
        if (C_iso)
        { 
            fprintf (fp, "\n");
        }
        else if (A_iso)
        { 
            // cwork = (ctype) Ax [0] already done
            fprintf (fp, " Cx [pC] = cwork\n") ;
        }
        else
        { 
            // general case
            fprintf (fp, " \\\n    GB_COPY_A_to_C (Cx, pC, Ax, pA, A_iso)\n") ;
        }
        // cwork = (ctype) A(i,j)
        GB_macrofy_cast_input (fp, "GB_COPY_aij_to_cwork", "cwork",
            "Ax,p,A_iso", A_iso ? "Ax [0]" : "Ax [p]", ctype, atype) ;
        // no copy of cwork to C
        fprintf (fp, "#define GB_COPY_cwork_to_C(Cx,pC,cwork,C_iso)"
            " /* unused */\n") ;
        // no copy of scalar to cwork
        fprintf (fp, "#define GB_COPY_scalar_to_cwork(cwork,scalar)"
            " /* unused */\n") ;
    }

    // xwork = (xtype) C(i,j), if needed
    if (need_copy_c_to_xwork)
    { 
        ASSERT (accum != NULL) ;
        ASSERT (!C_iso) ;
        ASSERT (xtype != ctype) ;
        GB_macrofy_cast_input (fp, "GB_COPY_C_to_xwork", "xwork",
            "Cx,p", "Cx [p]", xtype, ctype) ;
    }

    //--------------------------------------------------------------------------
    // construct the macros to access the mask (if any), and its name
    //--------------------------------------------------------------------------

    GB_macrofy_mask (fp, mask_ecode, "M", msparsity) ;

    //--------------------------------------------------------------------------
    // construct the macros for A or the scalar, including typecast to Y type
    //--------------------------------------------------------------------------

    bool did_scalar_to_ywork = false ;
    bool did_aij_to_ywork = false ;

    if (s_assign)
    { 
        // scalar assignment
        fprintf (fp, "\n// scalar:\n") ;
        GB_macrofy_type (fp, "A", "_", atype->name) ;
        if (accum != NULL)
        { 
            // accum is present
            // ywork = (ytype) scalar
            GB_macrofy_cast_input (fp, "GB_COPY_scalar_to_ywork", "ywork",
                "scalar", "(*((GB_A_TYPE *) scalar))", ytype, atype) ;
            did_scalar_to_ywork = true ;
        }
        GB_macrofy_sparsity (fp, "A", -1) ; // unused macros
        fprintf (fp, "#define GB_A_NVALS(e) int64_t e = 1 ; /* unused */\n") ;
        fprintf (fp, "#define GB_A_NHELD(e) int64_t e = 1 ; /* unused */\n") ;
    }
    else
    {
        // matrix assignment
        GB_macrofy_input (fp, "a", "A", "A", true, ytype, atype, asparsity,
            acode, A_iso, -1) ;
        if (accum != NULL)
        { 
            // accum is present
            // ywork = (ytype) A(i,j)
            fprintf (fp, "#define GB_COPY_aij_to_ywork(ywork,Ax,pA,A_iso) "
                "GB_GETA (ywork, Ax, pA, A_iso)\n") ;
            did_aij_to_ywork = true ;
        }
    }

    if (!did_scalar_to_ywork)
    {
        fprintf (fp, "#define GB_COPY_scalar_to_ywork(ywork,scalar)"
            " /* unused */\n") ;
    }

    if (!did_aij_to_ywork)
    {
        fprintf (fp, "#define GB_COPY_aij_to_ywork(ywork,Ax,pA,A_iso)"
            " /* unused */\n") ;
    }

    //--------------------------------------------------------------------------
    // construct the macros for S
    //--------------------------------------------------------------------------

    if (S_present)
    {
        GB_macrofy_sparsity (fp, "S", ssparsity) ;
        fprintf (fp, "#define GB_S_CONSTRUCTED 1\n") ;
    }
    else
    {
        fprintf (fp, "\n// S matrix: not constructed\n")  ;
        fprintf (fp, "#define GB_S_CONSTRUCTED 0\n") ;
    }

    //--------------------------------------------------------------------------
    // include the final default definitions
    //--------------------------------------------------------------------------

    fprintf (fp, "\n#include \"include/GB_assign_shared_definitions.h\"\n") ;
}

