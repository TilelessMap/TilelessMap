/**********************************************************************
 *
 * TileLess
 *
 * TileLess is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * TileLess is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TileLess.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2017 Nicklas Avén
 *
 **********************************************************************/


#include "theclient.h"

static double k0 = 0;
static double a;
static double f;
static double b;
static double e;
static double esq;
static double e0sq;
static double e1;
static double drad;
static double g1;
static double g2;
static double g3;



static void init_reproj()
{

    k0 = 0.9996;
    a = 6378137.0;
    f = 1/298.2572236;
    b = a*(1-f);
    e = sqrt(1 - (b*b)/(a*a));
    esq = (1.0 - (b/a)*(b/a));
    e0sq = e*e/(1.0-e*e);
    e1 = (1 - sqrt(1 - e*e))/(1 + sqrt(1 - e*e));
    drad = M_PI/180.0;
    g1 = (1.0 - esq*(1.0/4.0 + esq*(3.0/64.0 + 5.0*esq/256.0)));
    g2 = (3.0/2.0 - 27.0*e1*e1/32.0);
    g3 = e1*e1*(21.0/16.0 -55.0*e1*e1/32.0);
    return;
}


void reproject(GLfloat *points,uint8_t utm_in,uint8_t utm_out, uint8_t hemi_in, uint8_t hemi_out)
{

    if(utm_in == utm_out && hemi_in == hemi_out)
        return;
    
    double N,T,C,A, AA,M, phi,lng, lngd, x, y;
    int zcm;
    int south;
    double  mu, phi1, C1, T1, N1, R1, D, DD;


    if(!k0)
    {
        init_reproj();
    }

    if(utm_in > 0)
    {
        x = points[0];
        y = points[1];
        zcm = 3 + 6*(utm_in-1) - 180;
        M = y/k0;

        mu = M/(a*g1);

        phi1 = mu + e1*g2*sin(2.0*mu) + g3*sin(4.0*mu);//Footprint Latitude

        phi1 = phi1 + e1*e1*e1*(sin(6.0*mu)*151.0/96.0 + e1*sin(8.0*mu)*1097.0/512.0);




        C1 = e0sq*pow(cos(phi1),2);
        T1 = pow(tan(phi1),2);
        N1 = a/sqrt(1-pow(e*sin(phi1),2));
        R1 = N1*(1-e*e)/(1-pow(e*sin(phi1),2));
        D = (x-500000.0)/(N1*k0);
        DD = D*D;

        phi = (DD)*(1.0/2.0 - DD*(5.0 + 3.0*T1 + 10.0*C1 - 4.0*C1*C1 - 9.0*e0sq)/24.0);
        phi = phi + pow(D,6)*(61.0 + 90.0*T1 + 298.0*C1 + 45.0*T1*T1 -252.0*e0sq - 3.0*C1*C1)/720.0;
        phi = phi1 - (N1*tan(phi1)/R1)*phi;


        //lat = floor(1000000.0*phi/drad)/1000000.0;

        //Longitude
        lng = D*(1.0 + DD*((-1.0 -2.0*T1 -C1)/6.0 + DD*(5.0 - 2.0*C1 + 28.0*T1 - 3.0*C1*C1 +8.0*e0sq + 24.0*T1*T1)/120.0))/cos(phi1);
        lngd = zcm+lng/drad;
    }
    else
    {

        phi = points[0]*drad;//Convert latitude to radians
        lngd = points[1];
        lng = lngd*drad;//Convert longitude to radians
    }
//Output Longitude




    /*      lngd = 12.808597;
      latd = 59.922851;
     phi = latd*drad;//Convert latitude to radians
     */
    if(utm_out > 0)
    {
        zcm = 3 + 6*(utm_out-1) - 180;//Central meridian of zone

        N = a/sqrt(1.0-pow(e*sin(phi),2.0));
        T = pow(tan(phi),2.0);
        C = e0sq*pow(cos(phi),2.0);
        A = (lngd-zcm)*drad*cos(phi);
        AA = A*A;




        M = phi*(1 - esq*(1.0/4.0 + esq*(3.0/64.0 + 5.0*esq/256.0)));
        M = M - sin(2*phi)*(esq*(3.0/8.0 + esq*(3.0/32.0 + 45.0*esq/1024.0)));
        M = M + sin(4*phi)*(esq*esq*(15.0/256.0 + esq*45.0/1024.0));
        M = M - sin(6*phi)*(esq*esq*esq*(35.0/3072.0));
        M = M*a;//Arc length along standard meridian


        x = k0*N*A*(1.0 + AA*((1.0-T+C)/6.0 + AA*(5.0 - 18.0*T + T*T + 72.0*C -58.0*e0sq)/120.0));//Easting relative to CM
        x+=500000.0;//Easting standard
        y = k0*(M + N*tan(phi)*(AA*(1.0/2.0 + AA*((5.0 - T + 9.0*C + 4.0*C*C)/24.0 + AA*(61.0 - 58.0*T + T*T + 600.0*C - 330.0*e0sq)/720.0))));//Northing from equator

        /*
        printf("M = %.12lf\n", M);
        printf("k0 = %.12lf\n", k0);
        printf("N = %.12lf\n", N);
        printf("A = %.12lf\n", A);
        printf("C = %.12lf\n", C);
        printf("T = %.12lf\n", T);
        printf("y = %.12lf\n", y);

          */

        if (y < 0.0) {
            y = 10000000.0+y;
        }
        //Output into UTM Boxes
        /*	if (phi<0.0)
                south=1;
        	else
                south=0;
          */
        /*
        printf("%f, %f\n",x,y);
          */
        points[0] = x;
        points[1] = y;
    }
    else
    {

        points[0] = phi/drad;

        points[1] = lngd = zcm+lng/drad;

//Output Longitude



    }
    return;
}//End UTM to Geog



