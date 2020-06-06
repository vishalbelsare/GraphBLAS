function C = tanh (G)
%TANH hyperbolic tangent.
% C = tanh (G) computes the hyperbolic tangent of each entry of a GraphBLAS
% matrix G.
%
% See also GrB/tan, GrB/atan, GrB/atanh, GrB/atan2.

% SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights
% Reserved. http://suitesparse.com.  See GraphBLAS/Doc/License.txt.

G = G.opaque ;

if (~gb_isfloat (gbtype (G)))
    op = 'sin.double' ;
else
    op = 'sin' ;
end

C = GrB (gbapply (op, G)) ;

