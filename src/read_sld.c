
#include "read_sld.h"
#include <mxml.h>
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
    
        for (symbolizer = mxmlFindElement(node, node,"se:PointSymbolizer",  NULL, NULL, MXML_DESCEND);
         symbolizer != NULL;
         symbolizer = mxmlFindElement(symbolizer, node,"se:PointSymbolizer",NULL, NULL,MXML_DESCEND))
       { 
                mxml_node_t *Mark = mxmlFindElement(node, tree, "se:Mark", NULL, NULL,  MXML_DESCEND);
                const char *symbol = mxmlGetOpaque(mxmlFindPath(Mark, "se:WellKnownName"));
                    const char *size = mxmlGetOpaque(mxmlFindPath(node, "se:PointSymbolizer/se:Graphic/se:Size"));
                
                        for (n = mxmlFindElement(symbolizer, symbolizer,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
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
                        int unit = PIXEL_UNIT;
                        if(mxmlFindElement(n, Mark,"se:PointSymbolizer",  "uom", NULL,MXML_DESCEND))
                        {
                        const char *u = mxmlElementGetAttr(n, "uom");
                        if (!strcmp(u,"http://www.opengeospatial.org/se/units/metre"))
                            unit = METER_UNIT;
                        }
                        add2glushort_list(s->point_styles->units,unit);
                        
                        add2glushort_list(s->point_styles->z,z);
                        
                        add2glfloat_list(s->point_styles->size, strtof(size, NULL));
                        
                        printf("writing size %f and %zu is used\n",strtof(size, NULL), s->point_styles->size->used);
                        
                        if(!strcmp(symbol, "square") )          
                            add2uint8_list(s->point_styles->symbol,SQUARE_SYMBOL);
                        else if(!strcmp(symbol, "circle") )          
                            add2uint8_list(s->point_styles->symbol,CIRCLE_SYMBOL);
                        else if(!strcmp(symbol, "triangle") )          
                            add2uint8_list(s->point_styles->symbol,TRIANGLE_SYMBOL);
                        else if(!strcmp(symbol, "start") )          
                            add2uint8_list(s->point_styles->symbol,STAR_SYMBOL);
                        
                        s->point_styles->nsyms++;
       }
   //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;
}
static int parse_linestyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, int z)
{
    
    return 0;
}
static int parse_polygonstyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, int z)
{
    
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
    
    
        int z = nvals; 
    nvals = 0;
        
          for (rule = mxmlFindElement(tree, tree,"se:Rule",  NULL, NULL, MXML_DESCEND);
         rule != NULL;
         rule = mxmlFindElement(rule, tree,"se:Rule",NULL, NULL,MXML_DESCEND))
       {
           
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


