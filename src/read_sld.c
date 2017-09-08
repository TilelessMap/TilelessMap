
#include "read_sld.h"
#include "ext/mxml/mxml.h"
#include <limits.h>
#include "mem.h"
#include "buffer_handling.h"

static GLfloat* read_color(const char* str, GLfloat *c)
{
    
    char s[3];
    int i;
    s[2] = '\0';
    
    
    for (i=0;i<3;i++)
    {
    s[0] = str[1+i*2];
    s[1] = str[2+i*2];
    
    c[i] = strtol(s,NULL,16)/256.0;
    } 
     return 0;   
    
}


static int  check_and_add_style(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, struct STYLES **a)
{
    struct STYLES *s;
    if(key_type == STRING_TYPE)
    {
        const char *key = mxmlGetOpaque(mxmlFindElement(node, tree, "ogc:Literal",  NULL, NULL,  MXML_DESCEND));
       //  printf("key = %s\n",key);
        if(!key)
            key = "default";
        
        printf("key = %s, oneLayer styles = %p\n",key, oneLayer->styles);
        HASH_FIND_STR( oneLayer->styles, key, s);
        if(!s)
        {
         s = st_malloc(sizeof(struct STYLES));   
         s->key_type = STRING_TYPE;
         s->point_styles = NULL;
         s->line_styles = NULL;
         s->polygon_styles = NULL;         
         s->string_key = st_malloc(strlen(key) + 1);
         strcpy(s->string_key, key);
         HASH_ADD_KEYPTR( hh, oneLayer->styles, s->string_key, strlen(s->string_key), s );   
        }
    }
    else if (key_type == INT_TYPE)
    {
        const char *key_txt = mxmlGetOpaque(mxmlFindElement(node, tree, "ogc:Literal",  NULL, NULL,  MXML_DESCEND));
       //  printf("key = %s\n",key);
        if(!key_txt)
            key_txt = "default";
        
        long int key = strtol(key_txt, NULL, 10);
        HASH_FIND_INT( oneLayer->styles, &key, s);
        if(!s)
        {
         s = st_malloc(sizeof(struct STYLES));   
         s->key_type = INT_TYPE;
         s->point_styles = NULL;
         s->line_styles = NULL;
         s->polygon_styles = NULL;         
         s->int_key = (int) key;
         HASH_ADD_INT(oneLayer->styles, int_key, s);  
        }

        
    }
    *a = s;
    return 0;
}

static int parse_pointstyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, int z)
{
    
    struct STYLES *s = NULL;
    check_and_add_style(oneLayer, tree, node, key_type, &s);
   mxml_node_t *n, *symbolizer;
   GLfloat c[4];
    
    const char *opacity = NULL;
    
    if( ! s->point_styles)
    {
     s->point_styles =  st_malloc(sizeof(POINT_STYLE));
     s->point_styles->nsyms = 0;
     s->point_styles->symbol = init_uint8_list();
     s->point_styles->color = init_glfloat_list();
     s->point_styles->size = init_glfloat_list();
     s->point_styles->z = init_glushort_list();
     s->point_styles->units = init_glushort_list();
    }
    else
    {
        printf("error");
    }
    
    
        for (symbolizer = mxmlFindElement(node, node,"se:PointSymbolizer",  NULL, NULL, MXML_DESCEND);
         symbolizer != NULL;
         symbolizer = mxmlFindElement(symbolizer, node,"se:PointSymbolizer",NULL, NULL,MXML_DESCEND))
       { 
           
                    /*This is ugly because the sld standard sometimes is ugly
                     * If a symbol is given as map units we find that from searching 
                     * for a special uri in the symbolizer tag*/
                    int unit = PIXEL_UNIT;
                    const char *u = mxmlElementGetAttr(symbolizer, "uom");
                    if(u)
                    {
                    
                    if (!strcmp(u,"http://www.opengeospatial.org/se/units/metre"))
                        unit = METER_UNIT;
                    }
                    add2glushort_list(s->point_styles->units,unit);

           //We start investigating the Mark tag
                mxml_node_t *Mark = mxmlFindElement(symbolizer, symbolizer, "se:Mark", NULL, NULL,  MXML_DESCEND);
                
                /*Symbol*/
                const char *symbol = mxmlGetOpaque(mxmlFindPath(Mark, "se:WellKnownName"));
                if(!strcmp(symbol, "square") )          
                    add2uint8_list(s->point_styles->symbol,SQUARE_SYMBOL);
                else if(!strcmp(symbol, "circle") )          
                    add2uint8_list(s->point_styles->symbol,CIRCLE_SYMBOL);
                else if(!strcmp(symbol, "triangle") )          
                    add2uint8_list(s->point_styles->symbol,TRIANGLE_SYMBOL);
                else if(!strcmp(symbol, "star") )          
                    add2uint8_list(s->point_styles->symbol,STAR_SYMBOL);
                else
                    add2uint8_list(s->point_styles->symbol,CIRCLE_SYMBOL);

                /*Symbol size*/
                    const char *size = mxmlGetOpaque(mxmlFindPath(node, "se:PointSymbolizer/se:Graphic/se:Size"));
                    add2glfloat_list(s->point_styles->size, strtof(size, NULL));
                
                /*This is ugly. We have to iterate SvgParameter tags and check what attribute value they have
                 * to know what type of value it holds. Seems to be a waeknes of mini xml*/
                    for (n = mxmlFindElement(Mark, Mark,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
                    n != NULL;
                    n = mxmlFindElement(n, Mark,"se:SvgParameter",  "name", NULL,MXML_DESCEND))
                    {
                        const char *attr = mxmlElementGetAttr(n, "name");
                        if(!strcmp(attr, "fill"))
                        {
                            const char *color = mxmlGetOpaque(n);
                            read_color(color,c);
                        }
                        else if (!strcmp(attr, "stroke"))
                        {               
                            const char *stroke_color = mxmlGetOpaque(n);
                            
                        }
                        else if (!strcmp(attr, "stroke-width"))
                        {               
                            const char *width = mxmlGetOpaque(n);
                        }
                            else if (!strcmp(attr, "fill-opacity"))
                        {               
                            opacity = mxmlGetOpaque(n);
                        }
                    }
                    
                    if(opacity)            
                        c[3] = strtof(opacity, NULL);
                    else
                        c[3] = 1;
                        
                    addbatch2glfloat_list(s->point_styles->color, 4, c);
                
                     /*To get the layers rendered in the right order
                      * we use the reversed symbol order in the sld as z-value*/
                    add2glushort_list(s->point_styles->z,z);
                    
                    s->point_styles->nsyms++;
                    
                    printf("writing size %f and %zu is used\n",strtof(size, NULL), s->point_styles->size->used);
                        
       }
   //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;
}






static int parse_linestyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, int z)
{
    
    struct STYLES *s = NULL;
    check_and_add_style(oneLayer, tree, node, key_type, &s);
   mxml_node_t *n, *symbolizer;
   GLfloat c[4];
    
    const char *opacity = NULL;
    
    if( ! s->line_styles)
    {
     s->line_styles =  st_malloc(sizeof(LINE_STYLE));
     s->line_styles->nsyms = 0;
     s->line_styles->color = init_glfloat_list();
     s->line_styles->width = init_glfloat_list();
     s->line_styles->z = init_glushort_list();
     s->line_styles->units = init_glushort_list();
    }
    
   // printf("ny style\n");
        for (symbolizer = mxmlFindElement(node, node,"se:LineSymbolizer",  NULL, NULL, MXML_DESCEND);
         symbolizer != NULL;
         symbolizer = mxmlFindElement(symbolizer, node,"se:LineSymbolizer",NULL, NULL,MXML_DESCEND))
       { 
      //     printf(" ny symbolizer %p ok\n", symbolizer);
                    /*This is ugly because the sld standard sometimes is ugly
                     * If a symbol is given as map units we find that from searching 
                     * for a special uri in the symbolizer tag*/
                     int unit = PIXEL_UNIT;
                    const char *u = mxmlElementGetAttr(symbolizer, "uom");
                    if(u)
                    {
                    
                    if (!strcmp(u,"http://www.opengeospatial.org/se/units/metre"))
                        unit = METER_UNIT;
                    }
                    add2glushort_list(s->line_styles->units,unit);

                mxml_node_t *Stroke = mxmlFindElement(symbolizer, symbolizer, "se:Stroke", NULL, NULL,  MXML_DESCEND);
                
                opacity = NULL;
                    
                const char *width;
                /*This is ugly. We have to iterate SvgParameter tags and check what attribute value they have
                 * to know what type of value it holds. Seems to be a waeknes of mini xml*/
                    for (n = mxmlFindElement(Stroke, Stroke,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
                    n != NULL;
                    n = mxmlFindElement(n, Stroke,"se:SvgParameter",  "name", NULL,MXML_DESCEND))
                    {
                        const char *attr = mxmlElementGetAttr(n, "name");
                        
                        if(!strcmp(attr, "stroke"))
                        {
                            const char *color = mxmlGetOpaque(n);                            
                            read_color(color,c);
                          //  printf("        c = %f, %f, %f, %f,color = %s\n",c[0],c[1], c[2],c[3], color);
                        }
                        else if (!strcmp(attr, "stroke-width"))
                        {               
                            width = mxmlGetOpaque(n);
                            add2glfloat_list(s->line_styles->width, strtof(width, NULL));
                        }
                        else if (!strcmp(attr, "stroke-opacity"))
                        {               
                            opacity = mxmlGetOpaque(n);
                         //   printf("opacity = %s\n",opacity);
                        }
                    }
                    
                  // if(opacity)            
                 //       c[3] = strtof(opacity, NULL);
               //     else
                        c[3] = 0.5;
                        
                    
          //  printf("        color = %f, %f, %f, %f,z=%d, unit=%d, width=%s style = %p, used colors= %d\n",c[0],c[1], c[2],c[3], z, unit, width, s->line_styles->color->list, s->line_styles->color->used);
                    addbatch2glfloat_list(s->line_styles->color, 4, c);
                
           // printf("color = %f, %f, %f, %f\n",s->line_styles->color->list[s->line_styles->color->used-4],s->line_styles->color->list[s->line_styles->color->used-3],s->line_styles->color->list[s->line_styles->color->used-2],s->line_styles->color->list[s->line_styles->color->used-1]);
                     /*To get the layers rendered in the right order
                      * we use the reversed symbol order in the sld as z-value*/
                    add2glushort_list(s->line_styles->z,z);
                    
                    s->line_styles->nsyms++;                    
       }
   //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;
}
static int parse_polygonstyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, int z)
{
    
    struct STYLES *s = NULL;
    check_and_add_style(oneLayer, tree, node, key_type, &s);
   mxml_node_t *n, *symbolizer;
   GLfloat c[4];
    
    const char *opacity = NULL;
    
    if( ! s->polygon_styles)
    {
     s->polygon_styles =  st_malloc(sizeof(POLYGON_STYLE));
     s->polygon_styles->nsyms = 0;
     s->polygon_styles->color = init_glfloat_list();
     s->polygon_styles->z = init_glushort_list();
     s->polygon_styles->units = init_glushort_list();
    }
    
    if( ! s->line_styles)
    {
     s->line_styles =  st_malloc(sizeof(LINE_STYLE));
     s->line_styles->nsyms = 0;
     s->line_styles->color = init_glfloat_list();
     s->line_styles->width = init_glfloat_list();
     s->line_styles->z = init_glushort_list();
     s->line_styles->units = init_glushort_list();
    }
    
    
        for (symbolizer = mxmlFindElement(node, node,"se:PolygonSymbolizer",  NULL, NULL, MXML_DESCEND);
         symbolizer != NULL;
         symbolizer = mxmlFindElement(symbolizer, node,"se:PolygonSymbolizer",NULL, NULL,MXML_DESCEND))
       { 
           
                    /*This is ugly because the sld standard sometimes is ugly
                     * If a symbol is given as map units we find that from searching 
                     * for a special uri in the symbolizer tag*/
                    int unit = PIXEL_UNIT;
                    if(mxmlFindElement(node, node,"se:PolygonSymbolizer",  "uom", NULL,MXML_DESCEND))
                    {
                    const char *u = mxmlElementGetAttr(n, "uom");
                    if (!strcmp(u,"http://www.opengeospatial.org/se/units/metre"))
                        unit = METER_UNIT;
                    }
                    add2glushort_list(s->polygon_styles->units,unit);

                mxml_node_t *Fill = mxmlFindElement(symbolizer, symbolizer, "se:Fill", NULL, NULL,  MXML_DESCEND);
                

                    
                
                /*This is ugly. We have to iterate SvgParameter tags and check what attribute value they have
                 * to know what type of value it holds. Seems to be a waeknes of mini xml*/
                    for (n = mxmlFindElement(symbolizer, symbolizer,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
                    n != NULL;
                    n = mxmlFindElement(n, Fill,"se:SvgParameter",  "name", NULL,MXML_DESCEND))
                    {
                        const char *attr = mxmlElementGetAttr(n, "name");
                        if(!strcmp(attr, "fill"))
                        {
                            const char *color = mxmlGetOpaque(n);
                            read_color(color,c);
                        } 
                        else if (!strcmp(attr, "fill-opacity"))
                        {               
                            opacity = mxmlGetOpaque(n);
                        }
                    }
                    
                    if(opacity)            
                        c[3] = strtof(opacity, NULL);
                    else
                        c[3] = 1;
                        
                    addbatch2glfloat_list(s->polygon_styles->color, 4, c);
                
                     /*To get the layers rendered in the right order
                      * we use the reversed symbol order in the sld as z-value*/
                    add2glushort_list(s->polygon_styles->z,z);
                    
                    s->polygon_styles->nsyms++;                    
                    
                    
                    
                    
                    
                mxml_node_t *Stroke = mxmlFindElement(node, node, "se:Stroke", NULL, NULL,  MXML_DESCEND);
                

                    
                
                /*This is ugly. We have to iterate SvgParameter tags and check what attribute value they have
                 * to know what type of value it holds. Seems to be a waeknes of mini xml*/
                
                const char *width_txt = NULL;
                    for (n = mxmlFindElement(Stroke, Stroke,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
                    n != NULL;
                    n = mxmlFindElement(n, Stroke,"se:SvgParameter",  "name", NULL,MXML_DESCEND))
                    {
                        const char *attr = mxmlElementGetAttr(n, "name");

                        if (!strcmp(attr, "stroke"))
                        {               
                            const char *stroke_color = mxmlGetOpaque(n);                            
                            read_color(stroke_color,c);
                        }
                        else if (!strcmp(attr, "stroke-width"))
                        {               
                            width_txt = mxmlGetOpaque(n);
                            
                        }
                    }
                    if(width_txt)
                        add2glfloat_list(s->line_styles->width, strtof(width_txt, NULL));
                    else
                        add2glfloat_list(s->line_styles->width, 0.0);
                    
                        c[3] = 1;
                        
                    addbatch2glfloat_list(s->line_styles->color, 4, c);
                
                     /*To get the layers rendered in the right order
                      * we use the reversed symbol order in the sld as z-value*/
                    add2glushort_list(s->line_styles->z,z);
                    
                    s->line_styles->nsyms++;                    
       }
   //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;
}
static int parse_textstyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, int z)
{
    
    return 0;
}









char* load_sld(LAYER_RUNTIME *oneLayer,const char *sld)
{
    
    if(!sld || strlen(sld) ==0)
        return 0;
    
       const char *txt;
    const char *attr;
    int max_val = 0,min_val = INT_MAX, nvals=0, n_codes;
    long int val;
    char *last_propname = NULL;
    mxml_node_t *tree;
    struct STYLES *styles;
    int key_type = INT_TYPE;
    char *checknum;
    
    tree = mxmlLoadString(NULL, sld, MXML_OPAQUE_CALLBACK);
    
    mxml_node_t *rule, propname, propval;

  //  node = mxmlFindElement(tree, tree, "se:SvgParameter",  NULL, NULL,  MXML_DESCEND);
    
    
       for (rule = mxmlFindElement(tree, tree,"se:Rule",  NULL, NULL, MXML_DESCEND);
         rule != NULL;
         rule = mxmlFindElement(rule, tree,"se:Rule",NULL, NULL,MXML_DESCEND))
       {
           
        const char *propname = mxmlGetOpaque(mxmlFindElement(rule, tree, "ogc:PropertyName",  NULL, NULL,  MXML_DESCEND));
           
        if(last_propname && strcmp(propname, last_propname))
        {
            log_this(100, "TilelessMap only supports 1 property_name\n");
            return NULL;
        }
        else
        {
         last_propname = st_malloc(strlen(propname) + 1);
         strcpy(last_propname, propname);
        }
        if(key_type == INT_TYPE)
        {
            const char *val_txt = mxmlGetOpaque(mxmlFindElement(rule,tree,"ogc:Literal", NULL,NULL,MXML_DESCEND));
            printf("val = %s\n", val_txt);
            if(val_txt)
            {
                val = strtol(val_txt,&checknum,10);
                if(val == 0 && val_txt == checknum) //ok, there where no numbers so we switch to text type
                    key_type = STRING_TYPE;
            }
        }
        nvals++;
       }
    
    printf("nvals = %d, maxval = %d\n", nvals, max_val);
    
    
 //   styles = st_malloc(nvals * sizeof(struct STYLES));
    
    
    nvals = 0;
        
          for (rule = mxmlFindElement(tree, tree,"se:Rule",  NULL, NULL, MXML_DESCEND);
         rule != NULL;
         rule = mxmlFindElement(rule, tree,"se:Rule",NULL, NULL,MXML_DESCEND))
       {
           
        int z = nvals*0.002; //this value is set to support 500 symbols in a layer before getting a z-value over 1(which will prevent rendering
        if(z>1)
            z=1;
        if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:PolygonSymbolizer",  NULL, NULL,  MXML_DESCEND)))
           parse_polygonstyle(oneLayer,tree, rule, key_type, z);
        
       if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:PointSymbolizer",  NULL, NULL,  MXML_DESCEND)))
           parse_pointstyle(oneLayer,tree, rule, key_type, z);
        
       if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:LineSymbolizer",  NULL, NULL,  MXML_DESCEND)))
           parse_linestyle(oneLayer,tree, rule, key_type, z);
       
       if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:TextSymbolizer",  NULL, NULL,  MXML_DESCEND)))
           parse_textstyle(oneLayer,tree, rule, key_type, z);
        
        nvals++;
       z--;
        printf("nvals = %d\n",nvals);
    
       }     
        oneLayer->style_key_type = key_type;
        
    mxmlDelete(tree);
    printf("returning propname %s\n",last_propname);
    return last_propname;
    
}

int add_system_default_style()
{
    struct STYLES *s;
    GLfloat color[]={1,1,1,0.5};
            s = st_malloc(sizeof(struct STYLES)); 
            s->int_key = 0;
         s->string_key = NULL;
         
         //point
    s->point_styles =  st_malloc(sizeof(POINT_STYLE));
     s->point_styles->symbol = init_uint8_list();
     s->point_styles->color = init_glfloat_list();
     s->point_styles->size = init_glfloat_list();
     s->point_styles->z = init_glushort_list();
     s->point_styles->units = init_glushort_list();
     
     add2uint8_list(s->point_styles->symbol,CIRCLE_SYMBOL);
     addbatch2glfloat_list(s->point_styles->color,4,color);
     add2glfloat_list(s->point_styles->size,7);
     add2glushort_list(s->point_styles->z,1);
     add2glushort_list(s->point_styles->units,PIXEL_UNIT);
     s->point_styles->nsyms=1;
     
         //line
    s->line_styles =  st_malloc(sizeof(LINE_STYLE));
     s->line_styles->color = init_glfloat_list();
     s->line_styles->width = init_glfloat_list();
     s->line_styles->z = init_glushort_list();
     s->line_styles->units = init_glushort_list();
     
     addbatch2glfloat_list(s->line_styles->color,4,color);
     add2glfloat_list(s->line_styles->width,1);
     add2glushort_list(s->line_styles->z,1);
     add2glushort_list(s->line_styles->units,PIXEL_UNIT);
     s->line_styles->nsyms=1;
     
         //polygon
    s->polygon_styles =  st_malloc(sizeof(POLYGON_STYLE));
     s->polygon_styles->color = init_glfloat_list();
     s->polygon_styles->z = init_glushort_list();
     s->polygon_styles->units = init_glushort_list();
     
     addbatch2glfloat_list(s->polygon_styles->color,4,color);
     add2glushort_list(s->polygon_styles->z,1);
     add2glushort_list(s->polygon_styles->units,PIXEL_UNIT);
     s->polygon_styles->nsyms=1;
     system_default_style = s;
}


