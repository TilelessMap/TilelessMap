

#include "interface/interface.h"
#include "info.h"
#include "log.h"

typedef struct
{
    char    *name;
    int     id;
    int     visible;
    int     info_active;
    int     type;
    char    *db_alias;
} TLM_LAYER_INFO;

typedef struct
{
    TLM_LAYER_INFO *layers;
    int nlayers;
} TLM_LAYER_LIST;

typedef struct
{
    char *name;
    char *type;
} TLM_FIELD;
    
typedef struct
{
    TLM_LAYER_INFO layers;
    int nlayers;
} TLM_FIELDS;


/*************** Init and close *******************/
extern int TLM_init(char *f, char *dir);
extern void TLM_start();
extern void TLM_close();


/*************** Get info about layers *******************/
TLM_LAYER_LIST *TLM_get_layerlist();
int    TLM_destroy_layerlist();

TLM_FIELDS *TLM_get_info_fields;
TLM_FIELDS *TLM_get_fields;



/*************** UI-tools *******************/

/* This is a work in progress but the goal is this:
 * 
 * All this shall be done between TLM_init and TLM_start
 * 
 * There are 3 ways to get buttons and menues:
 * 1)   Init TilelessMap native UI with default controls. 
 *      Gives a simple Layers button and some other generic
 *      buttons and menues as they get developed
 * 2)   Define custom menues and buttons with TilelessMaps UI-tools defined below
 * 3)   Create UI outside of TilelessMap and just communicate with TilelessMap 
 *      to get and set values and states. With this approach the menues can for example be Android native. 
 */


/* By initializing controls with the right approach we tell TilelessMap what to initialize
 * CUSTOM approach inits no UI functionality at all
 * */

#define NATIVE 1
#define CUSTOM 2

int TLM_init_controls(int approach);

/*If we have choosen TILELESS_NATIVE approach we are done. There is nothing that can be customized
 * But if we choose TILELESS_CUSTOM we need the functions below to build the interface
 * You can copy and paste from UI.c to get started and customize from that*/
