using namespace cooperative_groups ;

#define log2_chunk_size 10
#define chunk_size 1024

__global__ void GB_cuda_apply_unop_kernel
(
    GB_void *Cx_out,
    GB_void *thunk_value,
    GrB_Matrix A
)
{

    GB_A_NHELD (anz) ;
    GB_Y_TYPE thunk ;

    const GB_A_TYPE *__restrict__ Ax = (GB_A_TYPE *) A->x ;
    const int64_t *__restrict__ Ai = (int64_t *) A->i ;
    const int64_t *__restrict__ Ah = (int64_t *) A->h ;
    const int64_t *__restrict__ Ap = (int64_t *) A->p ;
    const int8_t *__restrict__ Ab = (int8_t *) A->b ;

    GB_C_TYPE *__restrict__ Cx = (GB_C_TYPE *) Cx_out;

    #define A_iso GB_A_ISO

    int tid = blockDim.x * blockIdx.x + threadIdx.x ;
    int nthreads = blockDim.x * gridDim.x ;

    const int64_t anvec = A->nvec ;
    const int64_t avlen = A->vlen ;

    #if ( GB_DEPENDS_ON_Y )
        // get thunk value (of type GB_Y_TYPE)
        // if there is no thunk, will remain uninitialized; ok since ignored
        // by macros
        thunk = * ((GB_Y_TYPE *) thunk_value) ;
    #endif

    #if ( GB_A_IS_BITMAP || GB_A_IS_FULL )
        // bitmap/full case
        for (int p = tid ; p < anz ; p += nthreads)
        {
            if (!GBB_A (Ab, p)) { continue ; }

            int64_t col_idx = p / avlen ;
            int64_t row_idx = p % avlen ;
            GB_UNOP (Cx, p, Ax, p, A_iso, row_idx, col_idx, thunk) ;
        }
    #else

        // sparse/hypersparse case
        #if ( GB_DEPENDS_ON_J )
            // need to do ek_slice method
            for (int64_t pfirst = blockIdx << log2_chunk_size ; 
                        pfirst < anz ;
                        pfirst += girdDim.x << log2_chunk_size )
                {
                    int64_t my_chunk_size, anvec_sub1 ;
                    float slope ;
                    int64_t kfirst = GB_cuda_ek_slice_setup (Ap, anvec, anz, pfirst,
                        chunk_size, &my_chunk_size, &anvec_sub1, &slope) ;

                    for (int64_t curr_p = threadIdx.x ; curr_p < my_chunk_size ; curr_p += blockDim.x)
                    {
                        int64_t k = GB_cuda_ek_slice_entry (curr_p, pfirst, Ap, anvec_sub1, kfirst, slope) ;
                        int64_t col_idx = GBH_A (Ah, k) ;
                        int64_t row_idx = GBI_A (Ai, pfirst + curr_p, avlen) ;
                        GB_UNOP (Cx, p, Ax, p, A_iso, row_idx, col_idx, thunk) ;
                    }
                }
        #else
            // can do normal method
            for (int p = tid ; p < anz ; p += nthreads)
            {
                int64_t row_idx = GBI_A (Ai, p, avlen) ;
                GB_UNOP (Cx, p, Ax, p, A_iso, row_idx, /* col_idx */, thunk) ;  
            }
        #endif
    #endif
}

extern "C" {
    GB_JIT_CUDA_KERNEL_APPLY_UNOP_PROTO (GB_jit_kernel) ;
}

GB_JIT_CUDA_KERNEL_APPLY_UNOP_PROTO (GB_jit_kernel)
{
    dim3 grid (gridsz) ;
    dim3 block (blocksz) ;

    GB_cuda_apply_unop_kernel <<<grid, block, 0, stream>>> (Cx, thunk, A) ;

    return (GrB_SUCCESS) ;
}