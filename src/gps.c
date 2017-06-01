
#include "theclient.h"




int latlon2utm(int *zone, double latd, double lngd, double *x_res, double *y_res)
{
//Symbols as used in USGS PP 1395: Map Projections - A Working Manual
double N,T,C,A,M, phi,lng,esq,e0sq,x,y;


	double k0 = 0.9996;//scale on central meridian
	double a = 6378137.0;//equatorial radius, meters. 
	double f = 1/298.2572236;//polar flattening.
	double b = a*(1-f);//polar axis.
	double e = sqrtf(1 - (b*b)/(a*a));//eccentricity
    
	 esq = (1.0 - (b/a)*(b/a));//e squared for use in expansions
    //e = sqrtf(1 - (b/a)*(b/a));//eccentricity
    
    
	 e0sq = e*e/(1.0-e*e);// e0 squared - always even powers
     
	double drad = M_PI/180.0;//Convert degrees to radians)
    int utmz;
 //   char nos[10];
	int M0 = 0.0;//M0 is M for some origin latitude other than zero. Not needed for standard UTM


//Convert Latitude and Longitude to UTM



	if(latd <-90 || latd> 90){
		printf("Latitude must be between -90 and 90");
        return 1;
		}
	if(lngd <-180 || lngd > 180){
		printf("Latitude must be between -180 and 180");
        return 1;
		}

	 phi = latd*drad;//Convert latitude to radians
//	 lng = lngd*drad;//Convert longitude to radians
	utmz = 1 + floor((lngd+180)/6);//calculate utm zone
  //  printf("ursprunglig utmz = %d\n", utmz);
	utmz = *zone;
	int zcm = 3 + 6*(utmz-1) - 180;//Central meridian of zone
	
	
	//alert(utmz + "   " + zcm);
	//Calculate Intermediate Terms
	//alert(esq+"   "+e0sq)
	N = a/sqrtf(1.0-pow(e*sin(phi),2.0));
	//alert(1-Math.pow(e*Math.sin(phi),2));
	//alert("N=  "+N);
	T = pow(tan(phi),2.0);
	//alert("T=  "+T);
	C = e0sq*pow(cos(phi),2.0);
	//alert("C=  "+C);
	A = (lngd-zcm)*drad*cos(phi);
	//alert("A=  "+A);
	//Calculate M
//	double factor = (1 - esq*(1.0/4.0 + esq*(3.0/64.0 + 5*esq/256.0)));
    M = phi*(1 - esq*(1.0/4.0 + esq*(3.0/64.0 + 5.0*esq/256.0)));
    
	M = phi*(1 - esq*(1.0/4.0 + esq*(3.0/64.0 + 5*esq/256.0)));
 //   printf("M = %lf, phi = %lf, esq = %lf, factor = %lf\n",M,phi,esq, factor);
	M = M - sin(2*phi)*(esq*(3.0/8.0 + esq*(3.0/32.0 + 45.0*esq/1024.0)));
	M = M + sin(4*phi)*(esq*esq*(15.0/256.0 + esq*45.0/1024.0));
	M = M - sin(6*phi)*(esq*esq*esq*(35.0/3072.0));
	M = M*a;//Arc length along standard meridian
	//alert("M    ="+M);
	//Calculate UTM Values
	
	
	x = k0*N*A*(1.0 + A*A*((1.0-T+C)/6.0 + A*A*(5.0 - 18.0*T + T*T + 72.0*C -58.0*e0sq)/120.0));//Easting relative to CM
  //  printf("k0=%lf,N=%lf,A=%lf,T=%lf,C=%lf,e0sq=%lf\n", k0,N,A,T,C,e0sq);
    x=x+500000.0;//Easting standard 
	y = k0*(M - M0 + N*tan(phi)*(A*A*(1.0/2.0 + A*A*((5.0 - T + 9.0*C + 4.0*C*C)/24.0 + A*A*(61.0 - 58.0*T + T*T + 600.0*C - 330.0*e0sq)/720.0))));//Northing from equator
	
  //  printf ("  k0=%lf, M=%lf, M0=%d, N=%lf, phi=%lf, A=%lf, T=%lf, C=%lf, e0sqi=%lf, esq=%lf\n",  k0, M, M0, N, phi, A, T, C, e0sq, esq);
    if (y < 0.0){y = 10000000.0+y;}
	//Output into UTM Boxes
/*	if (phi<0.0)
        strcpy(nos,"south");
	else
        strcpy(nos,"north");*/
	//printf("UTM zone = %d, x = %lf, y = %lf, %s\n", utmz, x, y, nos);
    *x_res = x;
    *y_res = y;
    return 0;
}//close Geog to UTM



void gps_in(double latitude, double longitude, double acc)
{
    if(GPSEventType == ((Uint32)-1))
        GPSEventType = SDL_RegisterEvents(1);
    
    double x_res, y_res;
    int zone = 33;
    latlon2utm(&zone,latitude, longitude, &x_res, &y_res);
    
    
    log_this(100, "lat = %lf, lon = %lf, x = %lf, y = %lf", latitude, longitude, x_res, y_res);
    gps_point.x = x_res;
    gps_point.y = y_res;
    gps_point.s = acc;
    
    

    
    if (GPSEventType != ((Uint32)-1))
    {
        SDL_Event event;
        SDL_memset(&event, 0, sizeof(event)); /* or SDL_zero(event) */
        event.type = GPSEventType;
        event.user.code = 1;
 //       event.user.data1 = &gps_point;
        event.user.data1 = 0;
        event.user.data2 = 0;
        SDL_PushEvent(&event);
    }




}
