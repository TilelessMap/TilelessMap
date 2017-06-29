// Copyright 2000 softSurfer, 2012 Dan Sunday
// This code may be freely used and modified for any purpose
// providing that this copyright notice is included with it.
// SoftSurfer makes no warranty for this code, and cannot be held
// liable for any real or imagined damage resulting from its use.
// Users of this code must verify correctness for their application.
 

// a Point is defined by its coordinates {int x, y;}

// Modified by Nicklas Avén 2017
//===================================================================
 
 
#include "theclient.h"
 

// isLeft(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2  on the line
//            <0 for P2  right of the line
//    See: Algorithm 1 "Area of Triangles and Polygons"
inline static int isLeft( GLfloat *P0, GLfloat *P1, GLfloat *P2 )
{
    return ( (P1[0] - P0[0]) * (P2[1] - P0[1])
            - (P2[0] -  P0[0]) * (P1[1] - P0[1]) );
}
//===================================================================

//===================================================================


// wn_PnPoly(): winding number test for a point in a polygon
//      Input:   P = a point,
//               V[] = vertex points of a polygon V[n+1] with V[n]=V[0]
//      Return:  wn = the winding number (=0 only when P is outside)
int wn_PnPoly( GLfloat *P, GLfloat *V, int n , int n_dims_in_array)
{
    int    wn = 0, i;    // the  winding number counter

    // loop through all edges of the polygon
    for (i=0; i<n-1; i++) 
    {   // edge from V[i] to  V[i+1]
     //   printf("Vi= %f, Vi+1 = %f, Vi1 = %f, Vi1 + 1= %f, X = %f, Y = %f \n",V[n_dims_in_array*i], V[n_dims_in_array*i + 1], V[n_dims_in_array*(i+1)], V[n_dims_in_array*(i+1)+1], P[0], P[1]);
        if (V[n_dims_in_array*i+1] <= P[1]) 
        {          // start y <= P.y
            if (V[n_dims_in_array*(i+1)+1]  > P[1])      // an upward crossing
                 if (isLeft( V + n_dims_in_array*i, V + n_dims_in_array*(i+1), P) > 0)  // P left of  edge
                     ++wn;            // have  a valid up intersect
        }
        else 
        {                        // start y > P.y (no test needed)
            if (V[n_dims_in_array*(i+1)+1]  <= P[1])     // a downward crossing
                 if (isLeft( V + n_dims_in_array*i, V + n_dims_in_array*(i+1), P) < 0)  // P right of  edge
                     --wn;            // have  a valid down intersect
        }
    }
    
    i = n-1;
    if(P[0] != V[n_dims_in_array*i] || P[1] != V[n_dims_in_array*i+1])
    {
              //  printf("Vi= %f, Vi+1 = %f, Vi1 = %f, Vi1 + 1= %f, X = %f, Y = %f \n",V[n_dims_in_array*i], V[n_dims_in_array*i + 1], V[0], V[1], P[0], P[1]);

        if (V[n_dims_in_array*i+1] <= P[1]) 
        {          // start y <= P.y
            if (V[1]  > P[1])      // an upward crossing
                    if (isLeft( V + n_dims_in_array*i, V, P) > 0)  // P left of  edge
                        ++wn;            // have  a valid up intersect
        }
        else 
        {                        // start y > P.y (no test needed)
            if (V[1]  <= P[1])     // a downward crossing
                    if (isLeft( V + n_dims_in_array*i, V, P) < 0)  // P right of  edge
                        --wn;            // have  a valid down intersect
            }
    }    
    // printf("wn = %d\n", wn);   
    return wn;
}
//===================================================================
