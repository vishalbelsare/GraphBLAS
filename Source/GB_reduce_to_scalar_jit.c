//------------------------------------------------------------------------------
// GB_reduce_to_scalar_jit: reduce a matrix to a scalar, via the JIT
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2022, All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

//------------------------------------------------------------------------------

#include "GB_reduce.h"
#include "GB_binop.h"
#include "GB_stringify.h"
#include "GB_jitifyer.h"

typedef GrB_Info (*GB_reduce_function)
    (void *, const GrB_Matrix, void *, bool *, int, int) ;

GrB_Info GB_reduce_to_scalar_jit    // z = reduce_to_scalar (A) via the JIT
(
    void *z,                    // result
    const GrB_Monoid monoid,    // monoid to do the reduction
    const GrB_Matrix A,         // matrix to reduce
    GB_void *restrict W,        // workspace
    bool *restrict F,           // workspace
    int ntasks,                 // # of tasks to use
    int nthreads                // # of threads to use
)
{

    //------------------------------------------------------------------
    // enumify the reduce problem and look it up in the jit hash
    //------------------------------------------------------------------

    GBURBLE ("(jit) \n") ;
    GB_jit_encoding encoding ;
    char suffix [8 + GxB_MAX_NAME_LEN] ;
    uint64_t hash = GB_encodify_reduce (&encoding, suffix, monoid, A) ;
    void *dl_function = GB_jitifyer_lookup (hash, &encoding, suffix) ;

    //------------------------------------------------------------------
    // load it and compile it if not found
    //------------------------------------------------------------------

    if (dl_function == NULL)
    { 

        //--------------------------------------------------------------
        // first time this kernel has been seen since GrB_init
        //--------------------------------------------------------------

        // namify the reduce problem
        #define RLEN (256 + GxB_MAX_NAME_LEN)
        char reduce_name [RLEN] ;
        uint64_t rcode = encoding.code ;
        snprintf (reduce_name, RLEN-1, "GB_jit_reduce_%0*" PRIx64 "%s",
            7, rcode, suffix) ;

        char lib_filename [2048] ;

        //==============================================================
        // FIXME: make this a helper function for all kernels
        // FIXME: create this at GrB_init time, or by GxB_set
        char lib_folder [2048] ;
        snprintf (lib_folder, 2047,
            "/home/faculty/d/davis/.SuiteSparse/GraphBLAS/v%d.%d.%d"
            #ifdef GBRENAME
            "_matlab"
            #endif
            ,
            GxB_IMPLEMENTATION_MAJOR,
            GxB_IMPLEMENTATION_MINOR,
            GxB_IMPLEMENTATION_SUB) ;
        // try to load the libkernelname.so from the user's
        // .SuiteSparse/GraphBLAS folder (if already compiled)
        snprintf (lib_filename, 2048, "%s/lib%s.so", lib_folder, reduce_name) ;

        char command [4096] ;
        sprintf (command, "ldd %s\n", lib_filename) ;
        int res = system (command) ;
        printf ("result: %d\n", res) ;
        sprintf (command, "readelf -d %s\n", lib_filename) ;
        res = system (command) ;
        printf ("result: %d\n", res) ;

        void *dl_handle = dlopen (lib_filename, RTLD_LAZY) ;

        bool need_to_compile = (dl_handle == NULL) ;
        bool builtin = (encoding.suffix_len == 0) ;

        if (!need_to_compile && !builtin)
        {
            // not loaded but already compiled; make sure the defn are OK
            void *dl_query = dlsym (dl_handle, "GB_jit_query_defn") ;
            // compare the monoid and A->type definitions
            need_to_compile =
                !GB_jitifyer_match_defn (dl_query, 0, monoid->op->defn) ||
                !GB_jitifyer_match_defn (dl_query, 2, A->type->defn) ||
                !GB_jitifyer_match_idterm (dl_handle, monoid) ;
            if (need_to_compile)
            {
                // library is loaded but needs to change, so close it
                dlclose (dl_handle) ;
            }
        }

        //--------------------------------------------------------------
        // compile the jit kernel, if not found or if op/type changed
        //--------------------------------------------------------------

        if (need_to_compile)
        {

            //----------------------------------------------------------
            // construct a new jit kernel for this instance
            //----------------------------------------------------------

            GBURBLE ("(compiling) ") ;
            char source_filename [2048] ;
            snprintf (source_filename, 2048, "%s/%s.c",
                lib_folder, reduce_name) ;
            FILE *fp = fopen (source_filename, "w") ;
            if (fp == NULL)
            {
                // unable to open source file: punt to generic
                printf ("failed to write to *.c file!\n") ;
                return (GrB_PANIC) ;
            }
            fprintf (fp,
                "//--------------------------------------"
                "----------------------------------------\n") ;
            fprintf (fp, "// %s.h\n"
                "#include \"GB_jit_kernel_reduce.h\"\n", reduce_name) ;
            GB_macrofy_reduce (fp, rcode, monoid, A->type) ;
            fprintf (fp,
                "\n// reduction kernel\n"
                "#include \"GB_jit_kernel_reduce.c\"\n") ;

            if (!builtin)
            {
                // either the monoid or A->type is not builtin, or both;
                // create qeury_defn function
                GB_macrofy_query_defn (fp,
                    (GB_Operator) monoid->op, NULL,
                    A->type, NULL, NULL, NULL, NULL, NULL) ;
            }
            // create query_monoid function if the monoid is not builtin
            GB_macrofy_query_monoid (fp, monoid) ;
            fclose (fp) ;

            //----------------------------------------------------------
            // compile the *.c file to create the lib*.so file
            //----------------------------------------------------------

            GB_jitifyer_compile (reduce_name) ;
            dl_handle = dlopen (lib_filename, RTLD_LAZY) ;
            if (dl_handle == NULL)
            {
                // unable to open lib*.so file: punt to generic
                printf ("failed to load lib*.so file!\n") ;
                return (GrB_PANIC) ;
            }
        }

        // get the jit_kernel_function pointer
        dl_function = dlsym (dl_handle, "GB_jit_kernel") ;
        if (dl_function == NULL)
        {
            // unable to find GB_jit_kernel: punt to generic
            printf ("failed to load dl_function!\n") ;
            dlclose (dl_handle) ; 
            return (GrB_PANIC) ;
        }

        // insert the new kernel into the hash table
        if (!GB_jitifyer_insert (hash, &encoding, suffix, dl_handle,
            dl_function))
        {
            // unable to add kernel to hash table: punt to generic
            printf ("failed to add to hash table!\n") ;
            dlclose (dl_handle) ; 
            return (GrB_PANIC) ;
        }
    }

    //------------------------------------------------------------------
    // call the jit kernel and return result
    //------------------------------------------------------------------

    GB_reduce_function redfunc = (GB_reduce_function) dl_function ;
    GrB_Info info = redfunc (z, A, W, F, ntasks, nthreads) ;
    return (info) ;
}

