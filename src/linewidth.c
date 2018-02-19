/**********************************************************************
 *
 * TilelessMap
 *
 * TilelessMap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * TilelessMap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TilelessMap.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * Copyright (C) 2016-2018 Nicklas Avén
 *
 ***********************************************************************/


#include "buffer_handling.h"
#include "theclient.h"




static inline double vec2Length(t_vec2 v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}


static inline t_vec2 normal(GLfloat Ax, GLfloat Ay, GLfloat Bx, GLfloat By)
{
    double len;
    t_vec2 v;
    double deltax = Bx-Ax;
    double deltay = By-Ay;

    v.x = -deltay;
    v.y = deltax;

    len = vec2Length(v);
    v.x /= len;
    v.y /= len;

    return v;
}



static inline t_vec2 add_vec(t_vec2 *v1, t_vec2 *v2)
{
    t_vec2 v;
    v.x=v1->x+v2->x;
    v.y=v1->y+v2->y;
    return v;
}

void
calc_start(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal)
{
//    log_this(10, "Entering %s\n",__func__);
    int n=0;
    GLfloat p1x, p1y, p2x, p2y;

    p1x = p[0].coord[0];
    p1y = p[0].coord[1];
    p2x = p[1].coord[0];
    p2y = p[1].coord[1];

    t_vec2 norm = normal(p1x,p1y, p2x,p2y);


    add2glfloat_list(ut, p1x);
    add2glfloat_list(ut, p1y);
    add2glfloat_list(ut, (GLfloat) norm.x);
    add2glfloat_list(ut, (GLfloat) norm.y);
    add2glfloat_list(ut, p1x);
    add2glfloat_list(ut, p1y);
    add2glfloat_list(ut, (GLfloat) -norm.x);
    add2glfloat_list(ut, (GLfloat) -norm.y);

    /* *(ut + n++) = p1x;
     *(ut + n++) = p1y;
     *(ut + n++) = norm.x;
     *(ut + n++) = norm.y;
     *(ut + n++) = p1x;
     *(ut + n++) = p1y;
     *(ut + n++) = -norm.x;
     *(ut + n++) = -norm.y;*/


    *c = n;
    *last_normal = norm;
//log_this(5, "Leaving %s\n",__func__);
    return;
}

void
calc_join(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal)
{
    //   log_this(10, "Entering %s\n",__func__);
    int n = *c;
    POINT_CIRCLE *p_akt = p->next;

    GLfloat p1x, p1y, p2x, p2y, p3x, p3y;

    double Ax, Ay, Bx, By,Cx, Cy, Dx, Dy;

    p1x = p_akt->coord[0];
    p1y = p_akt->coord[1];

    p_akt = p_akt->next;
    p2x = p_akt->coord[0];
    p2y = p_akt->coord[1];

    p_akt = p_akt->next;
    p3x = p_akt->coord[0];
    p3y = p_akt->coord[1];


    double r_top, r_bot,r, d;

    t_vec2 norm = normal(p2x, p2y, p3x, p3y);



    d=(p3x - p1x) * (p2y - p1y) - (p3y - p1y) * (p2x - p1x);

    //  printf("d = %f\n",d);


//d=(x−x1)(y2−y1)−(y−y1)(x2−x1)

    t_vec2  avg_vec = add_vec(&norm, last_normal);

    Ax = p2x;
    Ay = p2y;
    Bx = p2x + avg_vec.x;
    By = 1.0*(1.0*p2y + 1.0*avg_vec.y);


//   printf("Ay = %f, avg_vec.y = %f, By = %f, Ay+avg_vec.y = %f\n", Ay, avg_vec.y, By, Ay+avg_vec.y);

    Cx =  p1x + last_normal->x;
    Cy =  p1y + last_normal->y;
    Dx =  p2x + last_normal->x;
    Dy =  p2y + last_normal->y;



    r_top = ((Ay - Cy) * (Dx-Cx)-(Ax-Cx) * (Dy-Cy));
    r_bot = ((Bx-Ax) * (Dy-Cy)-(By-Ay) * (Dx-Cx));

    if(r_bot == 0)
        r = 1;
    else
        r = r_top/r_bot;


    //  printf("ax = %f, ay = %f, bx = %f, by = %f, cx = %f, cy=%f, dx=%f, dy=%f\n", Ax, Ay, Bx, By, Cx, Cy, Dx, Dy);

//   printf("r = %f, rtop = %f, r_bot = %f\n",r,  r_top, r_bot);
    //      r = ((l1.p1.y - l2.p1.y) * (l2.p2.x - l2.p1.x) - (l1.p1.x - l2.p1.x) * (l2.p2.y - l2.p1.y)) / ((l1.p2.x - l1.p1.x) * (l2.p2.y - l2.p1.y) - (l1.p2.y - l1.p1.y) * (l2.p2.x - l2.p1.x));

    //     printf("r: %f\n", r);

    if(fabs(r)>1.4)
    {
        //    printf("d: %f\n", d);
        if (d < 0)
        {
            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  r * avg_vec.x);
            add2glfloat_list(ut,(GLfloat)  r * avg_vec.y);
            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  -last_normal->x);
            add2glfloat_list(ut,(GLfloat)  -last_normal->y);
            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  r * avg_vec.x);
            add2glfloat_list(ut,(GLfloat)  r * avg_vec.y);
            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  -norm.x);
            add2glfloat_list(ut,(GLfloat)  -norm.y);

            /*         *(ut + n++) = p1x;
                     *(ut + n++) = p1y;
                     *(ut + n++) = r * avg_vec.x;
                     *(ut + n++) = r * avg_vec.y;

                     *(ut + n++) = p2x;
                     *(ut + n++) = p2y;

                     *(ut + n++) = -last_normal->x;
                     *(ut + n++) = -last_normal->y;

                     *(ut + n++) = p2x;
                     *(ut + n++) = p2y;

                     *(ut + n++) = r * avg_vec.x;
                     *(ut + n++) = r * avg_vec.y;

                     *(ut + n++) = p2x;
                     *(ut + n++) = p2y;

                     *(ut + n++) = -norm.x;
                     *(ut + n++) = -norm.y;*/

        }
        else
        {

            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  last_normal->x);
            add2glfloat_list(ut,(GLfloat)  last_normal->y);
            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  - r * avg_vec.x);
            add2glfloat_list(ut,(GLfloat)  - r * avg_vec.y);
            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  norm.x);
            add2glfloat_list(ut,(GLfloat)  norm.y);
            add2glfloat_list(ut, p2x);
            add2glfloat_list(ut, p2y);
            add2glfloat_list(ut,(GLfloat)  - r * avg_vec.x);
            add2glfloat_list(ut,(GLfloat)  - r * avg_vec.y);

            /*    printf("-------------:::::::.----------------\n");
                 printf("p2x = %f, p2y = %f\n",p2x, p2y);
                 printf("last_normal->x = %f, last_normal->y = %f\n",last_normal->x, last_normal->y);
                 printf("p2x = %f, p2y = %f\n",p2x, p2y);
                 printf("r = %f, avg_vec.x =  %f, avg_vec.y = %f\n",r,avg_vec.x, avg_vec.y );
                 printf("p2x = %f, p2y = %f\n",p2x, p2y);
                 printf("normal.x = %f, normal.y = %f\n",norm.x, norm.y);
            */
            /**(ut + n++) = p2x;
            *(ut + n++) = p2y;

            *(ut + n++) = last_normal->x;
            *(ut + n++) = last_normal->y;

            *(ut + n++) = p2x;
            *(ut + n++) = p2y;

            *(ut + n++) = -r * avg_vec.x;
            *(ut + n++) = -r * avg_vec.y;

            *(ut + n++) = p2x;
            *(ut + n++) = p2y;

            *(ut + n++) = norm.x;
            *(ut + n++) = norm.y;

            *(ut + n++) = p2x;
            *(ut + n++) = p2y;

            *(ut + n++) = -r * avg_vec.x;
            *(ut + n++) = -r * avg_vec.y;*/


        }
    }
    else
    {
        add2glfloat_list(ut, p2x);
        add2glfloat_list(ut, p2y);
        add2glfloat_list(ut,(GLfloat)  r * avg_vec.x);
        add2glfloat_list(ut,(GLfloat)  r * avg_vec.y);
        add2glfloat_list(ut, p2x);
        add2glfloat_list(ut, p2y);
        add2glfloat_list(ut,(GLfloat)  -r * avg_vec.x);
        add2glfloat_list(ut,(GLfloat)  -r * avg_vec.y);
        /* *(ut + n++) = p2x;
         *(ut + n++) = p2y;
         *(ut + n++) = r * avg_vec.x;
         *(ut + n++) = r * avg_vec.y;

         *(ut + n++) = p2x;
         *(ut + n++) = p2y;
         *(ut + n++) = -r * avg_vec.x;
         *(ut + n++) = -r * avg_vec.y;*/

    }


    *c = n;
    *last_normal = norm;

// log_this(5, "Leaving %s\n",__func__);
    return;
}


void
calc_end(POINT_CIRCLE *p,GLFLOAT_LIST *ut,int *c, t_vec2 *last_normal)
{
//    log_this(10, "Entering %s\n",__func__);
    int n = *c;

    double  p2x, p2y;

    POINT_CIRCLE *p_akt = p->next;

    p_akt = p_akt->next;
    p2x = p_akt->coord[0];
    p2y = p_akt->coord[1];

    add2glfloat_list(ut, p2x);
    add2glfloat_list(ut, p2y);
    add2glfloat_list(ut,(GLfloat)  last_normal->x);
    add2glfloat_list(ut,(GLfloat)  last_normal->y);
    add2glfloat_list(ut, p2x);
    add2glfloat_list(ut, p2y);
    add2glfloat_list(ut,(GLfloat)  -last_normal->x);
    add2glfloat_list(ut,(GLfloat)  -last_normal->y);

    /**(ut + n++) = p2x;
    *(ut + n++) = p2y;
    *(ut + n++) = last_normal->x;
    *(ut + n++) = last_normal->y;
    *(ut + n++) = p2x;
    *(ut + n++) = p2y;
    *(ut + n++) = -last_normal->x;
    *(ut + n++) = -last_normal->y;*/




    *c = n;
//log_this(5, "Leaving %s\n",__func__);
    return;
}
