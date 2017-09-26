

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
} TLM_LAYERS_INFO;

typedef struct
{
    char *name;
    char *type;
} TLM_LAYER_INFO_FIELD;
    
    

/*************** Init and close *******************/
extern int TLM_init(char *f, char *dir);
extern void TLM_start();
extern void TLM_close();


/*************** Get info about layers *******************/
char** TLM_get_layerlist();
char** TLM_get_active_layerlist();
