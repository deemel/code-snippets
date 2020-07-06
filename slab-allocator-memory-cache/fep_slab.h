#ifndef _FEP_SLAB_H
#define _FEP_SLAB_H

#include <pthread.h>
#include <string.h>
#include "fep.h"
#include "fep_types.h"
#include "fep_memory.h"
#include "fep_map.h"

#define FEP_SLAB_MAX_SLOTS   8
#define FEP_SLAB_SZ_NAME     16
#define FEP_SLAB_SZ_BLK      (65 * 1024) // 66560 bytes
#define FEP_SLAB_INIT_HDRNUM -1
#define FEP_SLAB_MAGIC_BLK   0xb10ca91c
#define FEP_SLAB_MAGIC_OBJ   0x0b93a91c

typedef struct fep_slab_slot_info fep_slab_slot_info_t;
typedef struct fep_slab_list      fep_slab_list_t;
typedef struct fep_slab_obj       fep_slab_obj_t;
typedef struct fep_slab_blk       fep_slab_blk_t;
typedef struct fep_slab_slot      fep_slab_slot_t;
typedef struct fep_slab           fep_slab_t;
typedef struct fep_id             fep_id_t;
typedef struct fep_id_gen         fep_id_gen_t;

/*
* User-provided slot info
*/
struct fep_slab_slot_info {
    size_t  data_size; // user data size
    int     n_objs;    // # objs
    char   *slot_name; // slot name; 1st column in "show memory"
    char   *type_name; // user data type name
};

/*
* Circular doubly-linked list
*/
struct fep_slab_list {
    fep_slab_list_t *prev; // prev element
    fep_slab_list_t *next; // next element
};

/*
* Object in block
*/
struct fep_slab_obj {
    fep_slab_list_t  list;      // obj list
    fep_slab_blk_t  *parent;    // obj's parent (i.e., blk)
#if defined (__TYPE_PPC__)
    int              filler[3]; // filler for 64-bit ptr
#endif
    int              hdrnum;    // obj's header #
    int              magic;     // for corruption detection
    /*
    * User data starts here; header info above
    */
};

/*
* Block in slab
*/
struct fep_slab_blk {
    fep_slab_list_t list;      // blk list
#if defined (__TYPE_PPC__)
    int             filler[2]; // filler for 64-bit ptr
#endif
    int             blkref;    // blk ref count
    int             slot;      // slot idx using this blk
    int             obj_size;  // obj size
    int             magic;     // for corruption detection
    /*
    * Object (i.e., fep_slab_obj_t) starts here; header info above
    */
};

/*
* Slot in slab
*/
struct fep_slab_slot {
    fep_slab_list_t free_objs;   // free objs list
    fep_slab_list_t used_objs;   // used objs list
    Memory          memory;      // memory
    PrivInfo        privinfo;    // private memory info
    ulong           data_size;   // user data size
    int             n_objs;      // # objs
    int             objs_free;   // # objs free
    int             objs_in_use; // # objs in use
    int             objs_max;    // # objs in use max
    int             n_allocs;    // # alloc successes
    int             n_fail;      // # alloc failures
};

/*
* FEP slab allocator, Linux-inspired object allocator
*/
struct fep_slab {
    fep_slab_list_t free_blks;                 // free blks list
    char            name[FEP_SLAB_SZ_NAME];    // slab name
    ulong           flags;                     // flags (unused)
    int             blks_free;                 // # blks free
    int             blks_in_use;               // # blks in use
    int             blks_max;                  // # blks in use max
    pthread_mutex_t mutex;                     // mutex
    int             next_hdrnum;               // next obj header #
    fep_slab_blk_t* pblks;                     // first block in store
    int             nblks;                     // # blocks in store
    int             n_slots;                   // # slots
    fep_slab_slot_t slots[FEP_SLAB_MAX_SLOTS]; // slots
};

/*
* ID
*/
struct fep_id {
    fep_slab_list_t  list;  // ID list
    ulong            id;    // ID (begins at 1)
    void            *pdata; // user data associated with ID
};

/*
* ID generator
*/
struct fep_id_gen {
    fep_slab_list_t  free_ids; // free IDs list
    fep_slab_list_t  used_ids; // used IDs list
    pthread_mutex_t  mutex;    // mutex
    ulong            n_ids;    // # IDs
    fep_id_t        *ids;      // ID array
};

/*
* User APIs in diam_lib/fep_slab.c
*/
extern int fep_slab_init(fep_slab_t *, const char *, size_t, ulong, const fep_slab_slot_info_t[], void *, size_t);
extern void *fep_slab_alloc_from_slot(fep_slab_t *, int);
extern void *fep_slab_alloc(fep_slab_t *, size_t);
extern void fep_slab_free(fep_slab_t *, void *);
extern size_t calc_zstore(int, size_t, size_t);

extern int fep_id_gen_init(fep_id_gen_t *, ulong);
extern void fep_id_gen_free(fep_id_gen_t *);
extern fep_id_t *fep_id_alloc(fep_id_gen_t *, void *);
extern void fep_id_free(fep_id_gen_t *, fep_id_t *);
extern void fep_id_free_by_id(fep_id_gen_t *, ulong);
extern void *fep_id_get_data(fep_id_gen_t *, ulong);

#endif
