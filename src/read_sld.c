
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
        const char *key = mxmlGetText(mxmlFindElement(node, tree, "ogc:Literal",  NULL, NULL,  MXML_DESCEND), 0);
         printf("key = %s\n",key);
        if(!key)
            key = "default";
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
   mxml_node_t *n;
   GLfloat c[4];
    
    const char *opacity = NULL
    ;
    if( ! s->point_styles)
    {
     s->point_styles =  st_malloc(sizeof(POINT_STYLE));
     s->point_styles->symbol = init_uint8_list();
     s->point_styles->color = init_glfloat_list();
     s->point_styles->size = init_glfloat_list();
     s->point_styles->z = init_glushort_list();
    }
    
    
   mxml_node_t *Mark = mxmlFindElement(node, tree, "se:Mark", NULL, NULL,  MXML_DESCEND);
   const char *symbol = mxmlGetText(mxmlFindPath(Mark, "se:WellKnownName"), 0);
    const char *color = mxmlGetText(mxmlFindElement(Mark, tree, "se:SvgParameter",  "name", NULL,  MXML_DESCEND), 0);
   
          for (n = mxmlFindElement(Mark, Mark,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
         n != NULL;
         n = mxmlFindElement(n, Mark,"se:SvgParameter",  "name", NULL,MXML_DESCEND))
       {
        const char *attr = mxmlElementGetAttr(n, "name");
           if(!strcmp(attr, "fill"))
           {
            const char *color = mxmlGetText(n, 0);
            read_color(color,c);
           }
           else if (!strcmp(attr, "stroke"))
           {               
            const char *stroke_color = mxmlGetText(n, 0);
            printf("color = %s\n",stroke_color);
            
           }
           else if (!strcmp(attr, "stroke-width"))
           {               
            const char *width = mxmlGetText(n, 0);
            printf("color = %s\n",width);
           }
            else if (!strcmp(attr, "fill-opacity"))
           {               
            opacity = mxmlGetText(n, 0);
           }
           
           
       }
        if(opacity)            
            c[3] = strtof(opacity, NULL);
        else
            c[3] = 1;
        
        addbatch2glfloat_list(s->point_styles->color, 4, c);
   //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    printf("symbol = %s color = %s\n",symbol, color);
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
    
    tree = mxmlLoadString(NULL, sld, MXML_TEXT_CALLBACK);
    
    mxml_node_t *rule, propname, propval;

  //  node = mxmlFindElement(tree, tree, "se:SvgParameter",  NULL, NULL,  MXML_DESCEND);
    
    
       for (rule = mxmlFindElement(tree, tree,"se:Rule",  NULL, NULL, MXML_DESCEND);
         rule != NULL;
         rule = mxmlFindElement(rule, tree,"se:Rule",NULL, NULL,MXML_DESCEND))
       {
           
        const char *propname = mxmlGetText(mxmlFindElement(rule, tree, "ogc:PropertyName",  NULL, NULL,  MXML_DESCEND), 0);
           
        if(last_propname && strcmp(propname, last_propname))
        {
            log_this(100, "TilelessMap only supports 1 property_name\n");
            return NULL;
        }
        else
        {
         last_propname = st_malloc(strlen(propname + 1));
         strcpy(last_propname, propname);
        }
        if(key_type == INT_TYPE)
        {
            const char *val_txt = mxmlGetText(mxmlFindElement(rule,tree,"ogc:Literal", NULL,NULL,MXML_DESCEND),0);
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
           
        if( mxmlGetText(mxmlFindElement(rule, tree, "se:PolygonSymbolizer",  NULL, NULL,  MXML_DESCEND), 0))
           parse_polygonstyle(oneLayer,tree, rule, key_type, z);
        
       if( mxmlGetText(mxmlFindElement(rule, tree, "se:PointSymbolizer",  NULL, NULL,  MXML_DESCEND), 0))
           parse_pointstyle(oneLayer,tree, rule, key_type, z);
        
       if( mxmlGetText(mxmlFindElement(rule, tree, "se:LineSymbolizer",  NULL, NULL,  MXML_DESCEND), 0))
           parse_linestyle(oneLayer,tree, rule, key_type, z);
        
        nvals++;
       z--;
        printf("nvals = %d\n",nvals);
    
       }     
        
        
        
    mxmlDelete(tree);
    return last_propname;
    
}




