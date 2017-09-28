
#include "read_sld.h"
#include "ext/mxml/mxml.h"
#include <limits.h>
#include "mem.h"
#include "buffer_handling.h"
#include "fonts.h"
static GLfloat* read_color(const char* str, GLfloat *c)
{

    char s[3];
    int i;
    s[2] = '\0';


    for (i=0; i<3; i++)
    {
        s[0] = str[1+i*2];
        s[1] = str[2+i*2];

        c[i] = (GLfloat) (strtol(s,NULL,16)/256.0);
    }
    return 0;

}


static int  check_and_add_style(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, struct STYLES **a)
{
    struct STYLES *s = NULL;
    if(key_type == STRING_TYPE)
    {
        const char *key = mxmlGetOpaque(mxmlFindElement(node, tree, "ogc:Literal",  NULL, NULL,  MXML_DESCEND));
        if(!key)
            key = "-1";

        HASH_FIND_STR( oneLayer->styles, key, s);
        if(!s)
        {
            s = st_malloc(sizeof(struct STYLES));
            s->key_type = STRING_TYPE;
            s->point_styles = NULL;
            s->line_styles = NULL;
            s->polygon_styles = NULL;
            s->text_styles = NULL;
            s->string_key = st_malloc(strlen(key) + 1);
            strcpy(s->string_key, key);
            HASH_ADD_KEYPTR( hh, oneLayer->styles, s->string_key, strlen(s->string_key), s );
            log_this(10, "layer %s har style %p for val %s\n", oneLayer->name, s, key);
        }
    }
    else if (key_type == INT_TYPE)
    {
        const char *key_txt = mxmlGetOpaque(mxmlFindElement(node, tree, "ogc:Literal",  NULL, NULL,  MXML_DESCEND));
        if(!key_txt)
            key_txt = "-1";

        long int key = strtol(key_txt, NULL, 10);
        HASH_FIND_INT( oneLayer->styles, &key, s);
        if(!s)
        {
            s = st_malloc(sizeof(struct STYLES));
            s->key_type = INT_TYPE;
            s->point_styles = NULL;
            s->line_styles = NULL;
            s->polygon_styles = NULL;
            s->text_styles = NULL;
            s->int_key = (int) key;
            HASH_ADD_INT(oneLayer->styles, int_key, s);
            log_this(10, "layer %s har style %p for val %ld\n", oneLayer->name, s, key);
        }


    }
    *a = s;
    return 0;
}

static int parse_pointstyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, GLfloat z)
{

    struct STYLES *s = NULL;
    check_and_add_style(oneLayer, tree, node, key_type, &s);
    mxml_node_t *n, *symbolizer;

    const char *opacity = NULL;

    if( ! s->point_styles)
    {
        s->point_styles =  st_malloc(sizeof(POINT_STYLE));
        s->point_styles->nsyms = 0;
        s->point_styles->symbol = init_uint8_list();
        s->point_styles->color = init_glfloat_list();
        s->point_styles->size = init_glfloat_list();
        s->point_styles->z = init_glfloat_list();
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
        GLfloat c[4];

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
    //    const char *size = mxmlGetOpaque(mxmlFindPath(symbolizer, "se:PointSymbolizer/se:Graphic/se:Size"));
        const char *size = mxmlGetOpaque(mxmlFindElement(symbolizer, symbolizer, "se:Size",  NULL, NULL,  MXML_DESCEND));
        if(size)
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
      /*      else if (!strcmp(attr, "stroke"))
            {
                const char *stroke_color = mxmlGetOpaque(n);

            }
            else if (!strcmp(attr, "stroke-width"))
            {
                const char *width = mxmlGetOpaque(n);
            }*/
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
        add2glfloat_list(s->point_styles->z,z);

        s->point_styles->nsyms++;


    }
    //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;
}






static int parse_linestyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, GLfloat z)
{

    struct STYLES *s = NULL;
    check_and_add_style(oneLayer, tree, node, key_type, &s);
    mxml_node_t *n, *symbolizer;

    const char *opacity = NULL;

    if( ! s->line_styles)
    {
        s->line_styles =  st_malloc(sizeof(LINE_STYLE));
        s->line_styles->nsyms = 0;
        s->line_styles->color = init_glfloat_list();
        s->line_styles->width = init_glfloat_list();
        s->line_styles->z = init_glfloat_list();
        s->line_styles->units = init_glushort_list();
    }

    for (symbolizer = mxmlFindElement(node, node,"se:LineSymbolizer",  NULL, NULL, MXML_DESCEND);
            symbolizer != NULL;
            symbolizer = mxmlFindElement(symbolizer, node,"se:LineSymbolizer",NULL, NULL,MXML_DESCEND))
    {
        GLfloat c[4];

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
            }
            else if (!strcmp(attr, "stroke-width"))
            {
                width = mxmlGetOpaque(n);
                add2glfloat_list(s->line_styles->width, (GLfloat) (strtof(width, NULL)*0.5)); //We multiply by 0.5 since the number will be used in both directions in the shader
            }
            else if (!strcmp(attr, "stroke-opacity"))
            {
                opacity = mxmlGetOpaque(n);
                //   printf("opacity = %s\n",opacity);
            }
        }

        if(opacity)
            c[3] = strtof(opacity, NULL);
        else
            c[3] = 1;


        addbatch2glfloat_list(s->line_styles->color, 4, c);

        /*To get the layers rendered in the right order
        * we use the reversed symbol order in the sld as z-value*/
        add2glfloat_list(s->line_styles->z,z);

        s->line_styles->nsyms++;
    }
    //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;
}
static int parse_polygonstyle(LAYER_RUNTIME *oneLayer, mxml_node_t *tree, mxml_node_t *node, int key_type, GLfloat z)
{

    struct STYLES *s = NULL;
    check_and_add_style(oneLayer, tree, node, key_type, &s);
    mxml_node_t *n=NULL, *symbolizer;

    const char *opacity = NULL;

    if( ! s->polygon_styles)
    {
        s->polygon_styles =  st_malloc(sizeof(POLYGON_STYLE));
        s->polygon_styles->nsyms = 0;
        s->polygon_styles->color = init_glfloat_list();
        s->polygon_styles->z = init_glfloat_list();
        s->polygon_styles->units = init_glushort_list();
    }

    if( ! s->line_styles)
    {
        s->line_styles =  st_malloc(sizeof(LINE_STYLE));
        s->line_styles->nsyms = 0;
        s->line_styles->color = init_glfloat_list();
        s->line_styles->width = init_glfloat_list();
        s->line_styles->z = init_glfloat_list();
        s->line_styles->units = init_glushort_list();
    }


    for (symbolizer = mxmlFindElement(node, node,"se:PolygonSymbolizer",  NULL, NULL, MXML_DESCEND);
            symbolizer != NULL;
            symbolizer = mxmlFindElement(symbolizer, node,"se:PolygonSymbolizer",NULL, NULL,MXML_DESCEND))
    {

        GLfloat c[4];
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
        add2glushort_list(s->line_styles->units,unit); 

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
        add2glfloat_list(s->polygon_styles->z,z);

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
        add2glfloat_list(s->line_styles->z,z);

        s->line_styles->nsyms++;
    }
    //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;
}
static int parse_textstyle(LAYER_RUNTIME *oneLayer,char **parsed_text_attr, mxml_node_t *tree, mxml_node_t *node, int key_type, GLfloat z)
{

    *parsed_text_attr = NULL;

    struct STYLES *s = NULL;
    check_and_add_style(oneLayer, tree, node, key_type, &s);
    mxml_node_t *n, *symbolizer;
	const char *font_family = NULL;
    const char* font_weight_txt;
    int size;
    int font_weight = 0;
    if( ! s->text_styles)
    {
        s->text_styles =  st_malloc(sizeof(LINE_STYLE));
        s->text_styles->nsyms = 0;
        s->text_styles->color = init_glfloat_list();
        s->text_styles->size = init_glfloat_list();
        s->text_styles->z = init_glfloat_list();
        s->text_styles->a = init_pointer_list();
    }

    for (symbolizer = mxmlFindElement(node, node,"se:TextSymbolizer",  NULL, NULL, MXML_DESCEND);
            symbolizer != NULL;
            symbolizer = mxmlFindElement(symbolizer, node,"se:TextSymbolizer",NULL, NULL,MXML_DESCEND))
    {

        const char *PropertyName = mxmlGetOpaque(mxmlFindPath(symbolizer, "se:Label/ogc:PropertyName"));
        if(PropertyName)
        {
            *parsed_text_attr = st_malloc(strlen(PropertyName)+1);
            strcpy(*parsed_text_attr, PropertyName);
        }
        GLfloat c[4];



        const char *size_txt = NULL;
        /*This is ugly. We have to iterate SvgParameter tags and check what attribute value they have
         * to know what type of value it holds. Seems to be a waeknes of mini xml*/
        mxml_node_t *Font = mxmlFindElement(symbolizer, symbolizer, "se:Font", NULL, NULL,  MXML_DESCEND);
        for (n = mxmlFindElement(Font, Font,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
                n != NULL;
                n = mxmlFindElement(n, Font,"se:SvgParameter",  "name", NULL,MXML_DESCEND))
        {
            const char *attr = mxmlElementGetAttr(n, "name");

            if(!strcmp(attr, "font-size"))
            {
                size_txt = mxmlGetOpaque(n);
            }
            else if(!strcmp(attr, "font-family"))
            {
                font_family = mxmlGetOpaque(n);
            }
            else if(!strcmp(attr, "font-weight"))
            {
                font_weight_txt = mxmlGetOpaque(n);
                if(!strcmp(font_weight_txt,"normal"))
                    font_weight = NORMAL_TYPE;
                else if(!strcmp(font_weight_txt,"bold"))
                    font_weight = BOLD_TYPE;
                else if(!strcmp(font_weight_txt,"italic"))
                    font_weight = ITALIC_TYPE;
            }

        }
        /*   if(size)

                   add2glfloat_list(s->text_styles->size, strtof(size, NULL));
           else

                   add2glfloat_list(s->text_styles->size,12);*/
        add2glfloat_list(s->text_styles->size,2);

        mxml_node_t *Fill = mxmlFindElement(symbolizer, symbolizer, "se:Fill", NULL, NULL,  MXML_DESCEND);
        for (n = mxmlFindElement(Fill, Fill,"se:SvgParameter",  "name", NULL, MXML_DESCEND);
                n != NULL;
                n = mxmlFindElement(n, Fill,"se:SvgParameter",  "name", NULL,MXML_DESCEND))
        {
            const char *attr = mxmlElementGetAttr(n, "name");

            if(!strcmp(attr, "fill"))
            {

                const char *stroke_color = mxmlGetOpaque(n);
                read_color(stroke_color,c);
            }
        }
        if(size_txt)

            size = (int) ceil(strtof(size_txt, NULL));
        else

            size = 0;




        ATLAS *a = loadatlas(font_family,font_weight,size);

        add2pointer_list(s->text_styles->a, a);

        c[3] = 1;

        addbatch2glfloat_list(s->text_styles->color, 4, c);


        /*To get the layers rendered in the right order
        * we use the reversed symbol order in the sld as z-value*/
        add2glfloat_list(s->text_styles->z,z);

        s->text_styles->nsyms++;
    }
    //  const char *symbol = mxmlGetText(mxmlFindPath(node, "se:Description/se:Title"), 0);
    return 0;


}









char* load_sld(LAYER_RUNTIME *oneLayer,char *sld, char** text_attr)
{

    if(!sld || strlen(sld) ==0)
        return 0;
    *text_attr = NULL;
    char *parsed_text_attr = NULL;
    int nvals=0;
    long int val;
    char *last_propname = NULL;
    mxml_node_t *tree;
    int key_type = INT_TYPE;
    char *checknum;

    tree = mxmlLoadString(NULL, sld, MXML_OPAQUE_CALLBACK);

    mxml_node_t *rule;

    //  node = mxmlFindElement(tree, tree, "se:SvgParameter",  NULL, NULL,  MXML_DESCEND);
  //  if(!strcmp(oneLayer->name, "norge_1000_punkter"))
    //    printf("%s\n",oneLayer->name);

    for (rule = mxmlFindElement(tree, tree,"se:Rule",  NULL, NULL, MXML_DESCEND);
            rule != NULL;
            rule = mxmlFindElement(rule, tree,"se:Rule",NULL, NULL,MXML_DESCEND))
    {

        const char *propname = mxmlGetOpaque(mxmlFindElement(rule, tree, "ogc:PropertyName",  NULL, NULL,  MXML_DESCEND));

        if(last_propname && propname && strcmp(propname, last_propname))
        {
            log_this(100, "TilelessMap only supports 1 property_name\n");
            return NULL;
        }
        if(!last_propname && propname)
        {
            last_propname = st_malloc(strlen(propname) + 1);
            strcpy(last_propname, propname);
        }
        if(key_type == INT_TYPE)
        {
            const char *val_txt = mxmlGetOpaque(mxmlFindElement(rule,tree,"ogc:Literal", NULL,NULL,MXML_DESCEND));
            if(val_txt)
            {
                val = strtol(val_txt,&checknum,10);
                if(val == 0 && val_txt == checknum) //ok, there where no numbers so we switch to text type
                    key_type = STRING_TYPE;
            }
        }
        nvals++;
    }





    nvals = 0;

    for (rule = mxmlFindElement(tree, tree,"se:Rule",  NULL, NULL, MXML_DESCEND);
            rule != NULL;
            rule = mxmlFindElement(rule, tree,"se:Rule",NULL, NULL,MXML_DESCEND))
    {

        GLfloat z = (GLfloat) (nvals*0.002); //this value is set to support 500 symbols in a layer before getting a z-value over 1(which will prevent rendering
        if(z>1)
            z=1;
        if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:PolygonSymbolizer",  NULL, NULL,  MXML_DESCEND)))
            parse_polygonstyle(oneLayer,tree, rule, key_type, z);

        if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:PointSymbolizer",  NULL, NULL,  MXML_DESCEND)))
            parse_pointstyle(oneLayer,tree, rule, key_type, z);

        if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:LineSymbolizer",  NULL, NULL,  MXML_DESCEND)))
            parse_linestyle(oneLayer,tree, rule, key_type, z);

        if( mxmlGetOpaque(mxmlFindElement(rule, tree, "se:TextSymbolizer",  NULL, NULL,  MXML_DESCEND)))
        {
            parse_textstyle(oneLayer,&parsed_text_attr, tree, rule, key_type, z);
            if(*text_attr && parsed_text_attr)
            {
                if(strcmp(parsed_text_attr, *text_attr))
                    log_this(100, "Only one text attribute is supported per layer not both %s and %s. only %s will be used\n",*parsed_text_attr, *text_attr, *text_attr);
            }
            else
            {
                if(parsed_text_attr)
                {
                    *text_attr = st_malloc(strlen(parsed_text_attr)+1);
                    strcpy(*text_attr,parsed_text_attr);
                }

            }

        }
        if((parsed_text_attr))
            free(parsed_text_attr);
        nvals++;
        z--;

    }
    oneLayer->style_key_type = key_type;

    mxmlDelete(tree);
//   printf("returning propname %s\n",last_propname);
    return last_propname;

}

int add_system_default_style()
{

    //Styling when style is missing in layer
    struct STYLES *s;
    GLfloat z = 0.5;
    GLfloat color[]= {1,1,1,0.5};
    s = st_malloc(sizeof(struct STYLES));
    s->int_key = 0;
    s->string_key = NULL;

    //point
    s->point_styles =  st_malloc(sizeof(POINT_STYLE));
    s->point_styles->symbol = init_uint8_list();
    s->point_styles->color = init_glfloat_list();
    s->point_styles->size = init_glfloat_list();
    s->point_styles->z = init_glfloat_list();
    s->point_styles->units = init_glushort_list();

    add2uint8_list(s->point_styles->symbol,CIRCLE_SYMBOL);
    addbatch2glfloat_list(s->point_styles->color,4,color);
    add2glfloat_list(s->point_styles->size,7);
    add2glfloat_list(s->point_styles->z,z);
    add2glushort_list(s->point_styles->units,PIXEL_UNIT);
    s->point_styles->nsyms=1;

    //line
    s->line_styles =  st_malloc(sizeof(LINE_STYLE));
    s->line_styles->color = init_glfloat_list();
    s->line_styles->width = init_glfloat_list();
    s->line_styles->z = init_glfloat_list();
    s->line_styles->units = init_glushort_list();

    addbatch2glfloat_list(s->line_styles->color,4,color);
    add2glfloat_list(s->line_styles->width,1);
    add2glfloat_list(s->line_styles->z,z);
    add2glushort_list(s->line_styles->units,PIXEL_UNIT);
    s->line_styles->nsyms=1;

    //polygon
    s->polygon_styles =  st_malloc(sizeof(POLYGON_STYLE));
    s->polygon_styles->color = init_glfloat_list();
    s->polygon_styles->z = init_glfloat_list();
    s->polygon_styles->units = init_glushort_list();

    addbatch2glfloat_list(s->polygon_styles->color,4,color);
    add2glfloat_list(s->polygon_styles->z,z);
    add2glushort_list(s->polygon_styles->units,PIXEL_UNIT);
    s->polygon_styles->nsyms=1;
    system_default_style = s;




    //style used for info-lookup

    //Styling when style is missing in layer
    struct STYLES *info_s;
    GLfloat info_z = 0.5;
    GLfloat info_color[]= {1,0,0,0.5};
    info_s = st_malloc(sizeof(struct STYLES));
    info_s->int_key = 0;
    info_s->string_key = NULL;

    //point
    info_s->point_styles =  st_malloc(sizeof(POINT_STYLE));
    info_s->point_styles->symbol = init_uint8_list();
    info_s->point_styles->color = init_glfloat_list();
    info_s->point_styles->size = init_glfloat_list();
    info_s->point_styles->z = init_glfloat_list();
    info_s->point_styles->units = init_glushort_list();

    add2uint8_list(info_s->point_styles->symbol,CIRCLE_SYMBOL);
    addbatch2glfloat_list(info_s->point_styles->color,4,info_color);
    add2glfloat_list(info_s->point_styles->size,7);
    add2glfloat_list(info_s->point_styles->z,info_z);
    add2glushort_list(info_s->point_styles->units,PIXEL_UNIT);
    info_s->point_styles->nsyms=1;

    //line
    info_s->line_styles =  st_malloc(sizeof(LINE_STYLE));
    info_s->line_styles->color = init_glfloat_list();
    info_s->line_styles->width = init_glfloat_list();
    info_s->line_styles->z = init_glfloat_list();
    info_s->line_styles->units = init_glushort_list();

    addbatch2glfloat_list(info_s->line_styles->color,4,info_color);
    add2glfloat_list(info_s->line_styles->width,1);
    add2glfloat_list(info_s->line_styles->z,info_z);
    add2glushort_list(info_s->line_styles->units,PIXEL_UNIT);
    info_s->line_styles->nsyms=1;

    //polygon
    info_s->polygon_styles =  st_malloc(sizeof(POLYGON_STYLE));
    info_s->polygon_styles->color = init_glfloat_list();
    info_s->polygon_styles->z = init_glfloat_list();
    info_s->polygon_styles->units = init_glushort_list();

    addbatch2glfloat_list(info_s->polygon_styles->color,4,info_color);
    add2glfloat_list(info_s->polygon_styles->z,info_z);
    add2glushort_list(info_s->polygon_styles->units,PIXEL_UNIT);
    info_s->polygon_styles->nsyms=1;
    system_default_info_style = info_s;

    return 0;

}


