

#include "interface/interface.h"
#include "info.h"
#include "log.h"

typedef struct
{
    char    *name;
    char    *id;
    int     active;
    int     type;
    char    *db_alias;
    char    *db_filename;
} TLM_LAYER_INFO;

typedef struct
{
    TLM_LAYER_INFO layers;
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

