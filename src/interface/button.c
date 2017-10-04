#include "interface.h"




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
    float txt_width, txt_height;
    ATLAS *font = loadatlas("freesans",BOLD_TYPE, font_size);
    ATLAS *font2 = loadatlas("freesans",BOLD_TYPE, 40);
    
    GLshort box[4];
    clone_box(box, box_in);
    
  //  calc_text_widthandheight(txt, font,&txt_width, &txt_height);
    
    GLshort box_width = box[2]-box[0];
    GLshort box_height = box[3]-box[1];
    
    
    
    GLfloat fontcolor[] = {150,255,0,255};
    int z = spatial_parent->z + 1;
    TEXTBLOCK *txt_block = init_textblock();
    append_2_textblock(txt_block,txt, font, fontcolor,1000,H_CENTER_ALIGNMENT | V_CENTER_ALIGNMENT , NEW_STRING);
    
    GLfloat fontcolor2[] = {0,0,0,255};
    
  //  append_2_textblock(txt_block,"flera texter", font2, fontcolor2,200,H_CENTER_ALIGNMENT | V_CENTER_ALIGNMENT , APPENDING_STRING);
   // append_2_textblock(txt_block,"NU", font, fontcolor,200,H_CENTER_ALIGNMENT | V_CENTER_ALIGNMENT , APPENDING_STRING);
    txt_width = txt_block->dims->widths->list[0];
    txt_height = txt_block->dims->heights->list[0];
    
    if(box_width - txt_width < 2*margin_x)
        box[2] += 2*margin_x - (box_width - txt_width);    
    if(box_height - txt_height < 2*margin_y)
        box[3] += 2*margin_y - (box_height - txt_height);    
    
    
    return register_control(BUTTON,spatial_parent,caller,click_func, val, NULL,box,color, txt_block,txt_margin, default_active, z);

    return NULL;
    
}


    
