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
#include "buffer_handling.h"

static void init_decode(TWKB_PARSE_STATE *ts,TWKB_PARSE_STATE *old_ts);
static int decode_point(TWKB_PARSE_STATE *ts);
static int decode_line(TWKB_PARSE_STATE *ts);
static int decode_polygon(TWKB_PARSE_STATE *ts);
static int decode_multi(TWKB_PARSE_STATE *ts);
static int read_pointarray(TWKB_PARSE_STATE *ts, uint32_t npoints);
int* decode_id_list(TWKB_PARSE_STATE *ts, int ngeoms);

static inline long int getReadPos(TWKB_BUF *tb)
{
    return tb->BufOffsetFromBof + tb->read_pos - tb->start_pos;
}


int
decode_twkb(TWKB_PARSE_STATE *old_ts)
{
    TWKB_PARSE_STATE ts;
    init_decode(&ts, old_ts);
    read_header (&ts);
    switch (ts.thi->type)
    {
    case POINTTYPE:
        return decode_point(&ts);//, res_buf);
        break;
    case LINETYPE:
        return decode_line(&ts);//, res_buf);
        break;
    case POLYGONTYPE:
        return decode_polygon(&ts);//, res_buf);
        break;
    case MULTIPOINTTYPE:
    case MULTILINETYPE:
    case MULTIPOLYGONTYPE:
    case COLLECTIONTYPE:
        return decode_multi(&ts);//, res_buf);
        break;
    default:
        fprintf(stderr,"Error: Unknown type number %d\n",ts.thi->type);
        exit(EXIT_FAILURE);
    }
    return 0;
}


static void
init_decode(TWKB_PARSE_STATE *ts,TWKB_PARSE_STATE *old_ts )
{
    int i;

    ts->tb = old_ts->tb;
    ts->styleID = old_ts->styleID;
    ts->theLayer = old_ts->theLayer;
    ts->thi = old_ts->thi;
    ts->thi->has_bbox=0;
    ts->thi->has_size=0;
    ts->thi->has_idlist=0;
    ts->thi->has_z=0;
    ts->thi->has_m=0;
    ts->thi->is_empty=0;
    ts->thi->type=0;

    for (i=0; i<TWKB_IN_MAXCOORDS; i++)
    {
        ts->thi->factors[i]=0;
        ts->thi->coords[i]=0;
    }
    ts->thi->ndims=0;

    return;
}


int
read_header (TWKB_PARSE_STATE *ts)
{
    uint8_t flag;
    int8_t precision;
    uint8_t has_ext_dims;
    ts->thi->ndims=2;
    unsigned int i;
    /*Here comes a byte containing type info and precission*/
    flag = buffer_read_byte(ts->tb);
    ts->thi->type=flag&0x0F;
    precision= (int8_t) unzigzag64((flag&0xF0)>>4);
    ts->thi->factors[0]=ts->thi->factors[1]= pow(10, (double)precision);
    ts->thi->n_decimals[0]=ts->thi->n_decimals[1]= precision>0?precision:0; /*We save number of decimals too, to get it right in text based formats in a simple way*/

    //Flags for options

    flag = buffer_read_byte(ts->tb);
    ts->thi->has_bbox   =  flag & 0x01;
    ts->thi->has_size   = (flag & 0x02) >> 1;
    ts->thi->has_idlist = (flag & 0x04) >> 2;
    has_ext_dims = (flag & 0x08) >> 3;

    if ( has_ext_dims )
    {
        flag = buffer_read_byte(ts->tb);

        ts->thi->has_z    = (flag & 0x01);
        ts->thi->has_m    = (flag & 0x02) >> 1;
        /* If Z*/
        if(ts->thi->has_z)
        {
            ts->thi->ndims++;
            precision = (flag & 0x1C) >> 2;
            ts->thi->factors[2]= pow(10, (double)precision);
            ts->thi->n_decimals[2]=precision>0?precision:0;
        }

        /* If M*/
        if(ts->thi->has_m)
        {
            ts->thi->ndims++;
            precision = (flag & 0xE0) >> 5;
            ts->thi->factors[2+ts->thi->has_z]= pow(10, (double)precision);
            ts->thi->n_decimals[2+ts->thi->has_z]=precision>0?precision:0;
        }
    }

    if(ts->thi->has_size)
    {
        /*We need to first read the value and then add the position of the cursor in the file.
        The size we get from the twkb-file is the size from after the size-info to the end of the twkb*/
        ts->thi->next_offset = (size_t) buffer_read_uvarint(ts->tb);
        ts->thi->next_offset+= getReadPos(ts->tb);
    }


    if(ts->thi->has_bbox)
    {
        for (i=0; i<ts->thi->ndims; i++)
        {
            ts->thi->bbox->bbox_min[i] = (GLfloat) (buffer_read_svarint(ts->tb)/ts->thi->factors[i]);
            ts->thi->bbox->bbox_max[i] = (GLfloat) (buffer_read_svarint(ts->tb)/ts->thi->factors[i] + ts->thi->bbox->bbox_min[i]);
        }
    }


    return 0;
}


static int
decode_point(TWKB_PARSE_STATE *ts)
{
    read_pointarray(ts, 1);//, res_buf);
    return 0;
}


static int
decode_line(TWKB_PARSE_STATE *ts)
{
    int npoints;

    npoints = (int) buffer_read_uvarint(ts->tb);
    read_pointarray(ts, npoints);//, res_buf);

    return 0;
}

static int
decode_polygon(TWKB_PARSE_STATE *ts)
{
    int npoints, i, nrings;

    nrings = (int) buffer_read_uvarint(ts->tb);


    for (i=0; i<nrings; i++)
    {
        npoints = (int) buffer_read_uvarint(ts->tb);

        read_pointarray(ts, npoints);//, res_buf);
    }

    return 0;
}


static int
decode_multi(TWKB_PARSE_STATE *ts)
{
    int i,  *idlist, ngeoms;

    if(ts->theLayer->type & 6)
        add2gluint_list(ts->theLayer->polygons->polygon_start_indexes, ts->theLayer->polygons->vertex_array->used);

    
    parseFunctions_p pf;
    ngeoms = (int) buffer_read_uvarint(ts->tb);
    if(ts->thi->has_idlist)
        idlist = decode_id_list(ts, ngeoms);
    else
        idlist = NULL;


    switch (ts->thi->type)
    {
    case MULTIPOINTTYPE:
        pf=decode_point;
        break;
    case MULTILINETYPE:
        pf=decode_line;
        break;
    case MULTIPOLYGONTYPE:
        pf=decode_polygon;
        break;
    case COLLECTIONTYPE:
        pf=decode_twkb;
        break;
    default:
        pf=NULL;
    }
    for (i=0; i<ngeoms; i++)
    {
        pf(ts);
    }

    return 0;
}


static int
read_pointarray(TWKB_PARSE_STATE *ts, uint32_t npoints)
{
//TODO, handle more than 2 coordinates. Now they are just read into the buffer which will give failur in opengl since it doesn't get that info
    uint32_t i, j;
    uint32_t ndims = ts->thi->ndims;
    int64_t val;
    GLFLOAT_LIST *vertex_list, *wide_line;
    float new_val;
    GLfloat start_x, start_y;
    int c=0;
    int reprpject = 0;
    uint8_t utm_in, hemi_in;
    uint8_t close_ring = 0;
    LAYER_RUNTIME *theLayer = ts->theLayer;
    uint type = theLayer->type;

    //TODO: This will be overwritten for each geometry. This should be per geometry or a better way to register per data set.
    theLayer->n_dims = ndims;

    if((ts->theLayer->utm_zone != curr_utm) || (ts->theLayer->hemisphere != curr_hemi))
    {
        reprpject = 1;
        utm_in = ts->theLayer->utm_zone;
        hemi_in = ts->theLayer->hemisphere;
    }

    vertex_list = get_coord_list(theLayer, ts->styleID);

    if(type & 8)
    {
        t_vec2 last_normal;
        POINT_CIRCLE p[3];
        p->next = (POINT_CIRCLE*) p+1;
        (p+1)->next = (POINT_CIRCLE*) p+2;
        (p+2)->next = (POINT_CIRCLE*) p;
        
        if(type & 6)
            close_ring = 1;
        POINT_CIRCLE *p_akt = p;

        wide_line = get_wide_line_list(theLayer, ts->styleID);

        for( i = 0; i < npoints; i++ )
        {

            for( j = 0; j < ndims; j++ )
            {
                val = buffer_read_svarint(ts->tb);
                ts->thi->coords[j] += val;
                new_val = (float) (ts->thi->coords[j] / ts->thi->factors[j]);
                p_akt->coord[j] = new_val;
            }
            if(reprpject)
                reproject(p_akt->coord,utm_in,curr_utm,hemi_in,  curr_hemi);

            if(type & 4)
                addbatch2glfloat_list(vertex_list, ndims, p_akt->coord);

            if(i==1)
            {
                if(close_ring)
                {
                    start_x = p->coord[0];
                    start_y = p->coord[1];
                }

                calc_start(p, wide_line, &c, &last_normal);
            }


            if(i>1)
            {
                calc_join(p_akt, wide_line, &c,&last_normal);
            }

            if(i==npoints-1 && !(close_ring))
                calc_end(p_akt->next, wide_line, &c,&last_normal);



            p_akt = p_akt->next; //take a step in the ring

        }
        if(close_ring)
        {
            p_akt->coord[0] = start_x;
            p_akt->coord[1] = start_y;

            calc_join(p_akt, wide_line, &c,&last_normal);

            calc_end(p_akt->next, wide_line, &c,&last_normal);
        }
    }
    else
    {
        GLfloat coords[4];
        for( i = 0; i < npoints; i++ )
        {
            for( j = 0; j < ndims; j++ )
            {
                val = buffer_read_svarint(ts->tb);
                ts->thi->coords[j] += val;
                new_val = (GLfloat) (ts->thi->coords[j] / ts->thi->factors[j]);

                coords[j] = new_val;
            }

            if(reprpject)
                reproject(coords,utm_in,curr_utm,hemi_in,  curr_hemi);
            addbatch2glfloat_list(vertex_list, ndims, coords);
        }


    }
    pa_end(theLayer);
    return 0;
}


int* decode_id_list(TWKB_PARSE_STATE *ts, int ngeoms)
{

    buffer_jump_varint(ts->tb,ngeoms);
    //~ int i;
    //~ int64_t *idlist= get_space(ngeoms*sizeof(int64_t));

    //~ for (i=0; i<ngeoms; i++)
    //~ {
    //~ idlist[i]=buffer_read_svarint(ts->tb);
    //~ }
    //~ return idlist;
    return NULL;
}

/*this is a special case where we abuse the line type in twkb for storing
OpenGL Element Array. Maybe we will extend the twkb format to get a special
type for this. It have to get a special decoding since the result is to be loaded in int array.
We can also skip reading the header, since we we know wat it shall contain.
If that is not right we will find errors when decoding, hopefully catching before crashing*/

int* decode_element_array(TWKB_PARSE_STATE *old_ts)
{
    TWKB_PARSE_STATE ts;
    init_decode(&ts, old_ts);

    uint32_t npoints;

    uint32_t i, j;
    int64_t val;

    GLushort val_list[3];
//jump over header
    buffer_read_byte(ts.tb);
    buffer_read_byte(ts.tb);
//jump over2nd header byte since more than 2 dims
    buffer_read_byte(ts.tb);

    LAYER_RUNTIME *theLayer = old_ts->theLayer;

    GLUSHORT_LIST *element_list = theLayer->polygons->element_array;


    npoints = (uint32_t) buffer_read_uvarint(ts.tb);
    add2gluint_list(theLayer->polygons->area_style_id, old_ts->styleID);

    for( i = 0; i < npoints; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            val = buffer_read_svarint(ts.tb);
            ts.thi->coords[j] += val;
            val_list[j] = (GLushort) ts.thi->coords[j];
        }
        addbatch2glushort_list(element_list,3, val_list);
    }
    add2gluint_list(theLayer->polygons->element_start_indexes,element_list->used);

    return 0;
}
