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

static void init_decode(TWKB_PARSE_STATE *ts,TWKB_PARSE_STATE *old_ts);
static int decode_point(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf);
static int decode_line(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf);
static int decode_polygon(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf);
static int decode_multi(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf);
static int read_pointarray(TWKB_PARSE_STATE *ts, uint32_t npoints, GLESSTRUCT *res_buf);
int* decode_id_list(TWKB_PARSE_STATE *ts, int ngeoms);



int
decode_twkb(TWKB_PARSE_STATE *old_ts , GLESSTRUCT *res_buf)
{   TWKB_PARSE_STATE ts;
    init_decode(&ts, old_ts);
    read_header (&ts);
    switch (ts.thi->type)
    {
    case POINTTYPE:
        return decode_point(&ts, res_buf);
        break;
    case LINETYPE:
        return decode_line(&ts, res_buf);
        break;
    case POLYGONTYPE:
        return decode_polygon(&ts, res_buf);
        break;
    case MULTIPOINTTYPE:
    case MULTILINETYPE:
    case MULTIPOLYGONTYPE:
    case COLLECTIONTYPE:
        return decode_multi(&ts, res_buf);
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
    //~ ts->rb = old_ts->rb;
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
    uint i;
    /*Here comes a byte containing type info and precission*/
    flag = buffer_read_byte(ts->tb);
    ts->thi->type=flag&0x0F;
    precision=unzigzag64((flag&0xF0)>>4);
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
        ts->thi->next_offset = buffer_read_uvarint(ts->tb);
        ts->thi->next_offset+= getReadPos(ts->tb);
    }


    if(ts->thi->has_bbox)
    {
        for (i=0; i<ts->thi->ndims; i++)
        {
            ts->thi->bbox->bbox_min[i]=buffer_read_svarint(ts->tb)/ts->thi->factors[i];
            ts->thi->bbox->bbox_max[i]=buffer_read_svarint(ts->tb)/ts->thi->factors[i] + ts->thi->bbox->bbox_min[i];
        }
    }

    return 0;
}


static int
decode_point(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf)
{
    //int type;
    //type = POINTTYPE;
    check_and_increase_max_pa(1,res_buf);
    read_pointarray(ts, 1, res_buf);
    return 0;
}


static int
decode_line(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf)
{
    int npoints;
    //int type;
    //type = LINETYPE;

    npoints = (int) buffer_read_uvarint(ts->tb);
    //   DEBUG_PRINT(("npoints;%d\n", npoints));
    check_and_increase_max_pa(1,res_buf);
    read_pointarray(ts, npoints, res_buf);

    return 0;
}

static int
decode_polygon(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf)
{
    int npoints, i, nrings;

//int type;
    //  type = POLYGONTYPE;
    nrings = (int) buffer_read_uvarint(ts->tb);

    check_and_increase_max_pa(nrings,res_buf);


    for (i=0; i<nrings; i++)
    {
        npoints = (int) buffer_read_uvarint(ts->tb);

        read_pointarray(ts, npoints, res_buf);
    }

    return 0;
}


static int
decode_multi(TWKB_PARSE_STATE *ts, GLESSTRUCT *res_buf)
{
    int i,  *idlist, ngeoms;

    check_and_increase_max_polygon(2, res_buf);// Here we add one extra since we register start offset for the comming polygon. So we need space for that

    set_end_polygon( res_buf);
    //int type;
    parseFunctions_p pf;
//    type = COLLECTIONTYPE;
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
        pf(ts, res_buf);
    }

    return 0;
}


static int
read_pointarray(TWKB_PARSE_STATE *ts, uint32_t npoints, GLESSTRUCT *res_buf)
{

    uint32_t i, j;
    uint32_t ndims = ts->thi->ndims;
    int64_t val;
    float *dlist;
    float new_val;
    double f;

    int has_m=ts->thi->has_m;

    int c = 0, filter = 10000;

//	register_vertex_array(npoints, ndims,res_buf);

    if(has_m)
    {
        ndims--;
        dlist = get_start(npoints, ndims,res_buf);
        for( i = 0; i < npoints; i++ )
        {

            for( j = 0; j < ndims; j++ )
            {
                val = buffer_read_svarint(ts->tb);
                ts->thi->coords[j] += val;
                new_val = (float) ts->thi->coords[j] / ts->thi->factors[j];

                //  printf("npoint = %d, dim = %d, new_val = %f\n",i,j, new_val);
                dlist[ndims * c + j] = new_val;
//DEBUG_PRINT(("c = %d, j= %d, val = %f\n", c, j, new_val));
            }

            val = buffer_read_svarint(ts->tb);
            ts->thi->coords[j] += val;
            f = (double) ts->thi->coords[j] / ts->thi->factors[j];
            if(f >= filter)
                c++;
        }
        /*     for (i =0; i<c; i++)
             {
         for( j = 0; j < ndims; j++ )
         {
           DEBUG_PRINT(("c = %d, j= %d, val = %f\n", i, j, dlist[ndims * i + j]));
           }

             }*/


        set_end(npoints, ndims,ts->id, ts->styleID,res_buf);
    }
    else
    {
        dlist = get_start(npoints, ndims,res_buf);
        for( i = 0; i < npoints; i++ )
        {
            for( j = 0; j < ndims; j++ )
            {
                val = buffer_read_svarint(ts->tb);
                ts->thi->coords[j] += val;
                new_val = (float) ts->thi->coords[j] / ts->thi->factors[j];

                //  printf("npoint = %d, dim = %d, new_val = %f\n",i,j, new_val);
                dlist[ndims*i + j] = new_val;
                //		printf("%ld ",dlist[ndims*i + j] );
            }
        }
        set_end(npoints, ndims,ts->id, ts->styleID,res_buf);

    }


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

int* decode_element_array(TWKB_PARSE_STATE *old_ts, ELEMENTSTRUCT *index_buf)
{
    TWKB_PARSE_STATE ts;
    init_decode(&ts, old_ts);

    uint32_t npoints;

    uint32_t i, j;
    int64_t val;
    GLushort *dlist;

//jump over header
    buffer_read_byte(ts.tb);
    buffer_read_byte(ts.tb);
//jump over2nd header byte since more than 2 dims
    buffer_read_byte(ts.tb);




    //  read_header (&ts);
    npoints = (uint32_t) buffer_read_uvarint(ts.tb);
    element_check_and_increase_max_pa(1,index_buf);


    dlist = element_get_start(npoints, 3,index_buf);
    for( i = 0; i < npoints; i++ )
    {
        for( j = 0; j < 3; j++ )
        {
            val = buffer_read_svarint(ts.tb);
            ts.thi->coords[j] += val;
            dlist[3 * i + j] = ts.thi->coords[j];
        }
    }
    element_set_end(npoints, 3, ts.styleID,index_buf);

    return 0;
}