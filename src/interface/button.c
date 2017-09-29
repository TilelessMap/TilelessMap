#include "interface.h"


static inline int32_t max_i(int a, int b)
{
    if (b > a)
        return b;
    else
        return a;
}

static inline float max_f(float a, float b)
{
    if (b > a)
        return b;
    else
        return a;
}

int calc_text_widthandheight(const char *txt, ATLAS *font, int *width, int *height)
{
    int w=0, h=0,current_row_height=0, current_row_width=0,pw, ph; 
    int len, i;
    
    //using tmp_unicode_txt here makes it not thread-safe.
    //But it saves a lot of malloc
    
    reset_wc_txt(tmp_unicode_txt);
    add_utf8_2_wc_txt(tmp_unicode_txt,txt);
    
    
    len = strlen(txt);
    uint8_t p;
    for(i=0;i<len;i++)
    {
        p = tmp_unicode_txt->txt[i];
        
        if(p=='\n')
        {
            h+=current_row_height;
            current_row_height = 0;
            w = max_i(w, current_row_width);
            current_row_width = 0;
        }
        else
        {
            ph = font->ch;
            pw = font->metrics[p].ax;
            
            current_row_height = max_i(current_row_height, ph);
            current_row_width+=pw;
        }                    
    }    
    *(height) = h+=current_row_height;
    *(width) = max_i(w, current_row_width);
    

    return 0;
    
}


CTRL* add_button(struct CTRL* caller, struct CTRL* spatial_parent, GLshort box_in[],const char *txt, tileless_event_function click_func,void *val, GLfloat* color,int font_size,short *txt_margin, int default_active)
{
    GLshort margin_x, margin_y;
    if(txt_margin)
    {
        margin_x = txt_margin[0];
        margin_y = txt_margin[1];
    }
    else
    {
        margin_x = DEFAULT_TXT_MARGIN;
        margin_y = DEFAULT_TXT_MARGIN;
    }
    int MIN_MARGIN = 2;
    int txt_width, txt_height;
    ATLAS *font = loadatlas("freesans",BOLD_TYPE, font_size);
    
    GLshort text_margin[2];
    GLshort box[4];
    clone_box(box, box_in);
    
    calc_text_widthandheight(txt, font,&txt_width, &txt_height);
    
    GLshort box_width = box[2]-box[0];
    GLshort box_height = box[3]-box[1];
    
    
    if(box_width - txt_width < 2*margin_x)
        box[2] += 2*margin_x - (box_width - txt_width);    
    if(box_height - txt_height < 2*margin_y)
        box[3] += 2*margin_y - (box_height - txt_height);    
    
    int z = spatial_parent->z + 1;
    TEXTBLOCK *txt_block = init_textblock(strlen(txt));
    append_2_textblock(txt_block,txt, font);
    
    return register_control(BUTTON,spatial_parent,caller,click_func, val, NULL,box,color, txt_block,txt_margin, default_active, z);

    
}


    
