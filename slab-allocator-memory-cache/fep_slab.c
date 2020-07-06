#include "fep_slab.h"

/*
* ======================================================================
* Functions for managing circular doubly-linked list
* ======================================================================
*/

/*
* Initialize list
*/
static inline void fep_slab_list_init(fep_slab_list_t *phead) {
    phead->next = phead->prev = phead;
}

/*
* Add element between 2 elements in list
*/
static inline void fep_slab_list_add(fep_slab_list_t *pnew, fep_slab_list_t *pprev, fep_slab_list_t *pnext) {
    pprev->next = pnew;
    pnew->prev = pprev;
    pnext->prev = pnew;
    pnew->next = pnext;
}

/*
* Add element to head of list
*/
static inline void fep_slab_list_add_head(fep_slab_list_t *pnew, fep_slab_list_t *phead) {
    fep_slab_list_add(pnew, phead, phead->next);
}

/*
* Add element to tail of list
*/
static inline void fep_slab_list_add_tail(fep_slab_list_t *pnew, fep_slab_list_t *phead) {
    fep_slab_list_add(pnew, phead->prev, phead);
}

/*
* Delete element from list
*/
static inline void fep_slab_list_del(fep_slab_list_t *pold) {
    pold->prev->next = pold->next;
    pold->next->prev = pold->prev;
    pold->prev = pold->next = NULL;
}

/*
* Pop element from head of list
*/
static inline fep_slab_list_t *fep_slab_list_pop_head(fep_slab_list_t *phead) {
    fep_slab_list_t *pbuf = phead->next;
    fep_slab_list_del(pbuf);

    return pbuf;
}

/*
* Pop element from tail of list
*/
static inline fep_slab_list_t *fep_slab_list_pop_tail(fep_slab_list_t *phead) {
    fep_slab_list_t *pbuf = phead->prev;
    fep_slab_list_del(pbuf);

    return pbuf;
}

/*
* Is list empty?
*/
static inline bool_t fep_slab_list_is_empty(fep_slab_list_t *phead) {
    return (phead->next == phead);
}

/*
* ======================================================================
* Private functions for managing slab allocator
* ======================================================================
*/

/*
* Allocate block, partitioning block into objects
*/
static int fep_slab_block_alloc(fep_slab_t *pslab, int slot) {
    if (! pslab)
        return FAILURE;

    if ((slot < 0) || (slot >= pslab->n_slots))
        return FAILURE;

    fep_slab_slot_t *pslot = &pslab->slots[slot];

    /*
    * Try to get free block from free blocks list
    */
    fep_slab_blk_t *pblk = NULL;
    if (! fep_slab_list_is_empty(&pslab->free_blks)) {
        pblk = (fep_slab_blk_t *) fep_slab_list_pop_head(&pslab->free_blks);
        --pslab->blks_free;
        ++pslab->blks_in_use;
    }
    else return FAILURE; // Empty free blocks list; alloc failure

    if (! pblk) // Free blocks list returned NULL?!
        return FAILURE;

    if (pslab->blks_in_use > pslab->blks_max)
        pslab->blks_max = pslab->blks_in_use; // Update watermark

    pblk->obj_size = sizeof(fep_slab_obj_t) + pslot->data_size;
    pblk->blkref = 0; // No object is alloc from this block yet
    pblk->slot = slot;
    pblk->magic = FEP_SLAB_MAGIC_BLK;

    int objs_per_blk = (FEP_SLAB_SZ_BLK - sizeof(fep_slab_blk_t)) / pblk->obj_size;
    fep_slab_obj_t *pobj;

    for (pobj = (fep_slab_obj_t *) (pblk + 1); objs_per_blk > 0; 
         pobj = (fep_slab_obj_t *) (((char *) pobj) + pblk->obj_size), --objs_per_blk) {
        pobj->parent = pblk;
        pobj->hdrnum = FEP_SLAB_INIT_HDRNUM;
        pobj->magic = FEP_SLAB_MAGIC_OBJ;

        fep_slab_list_add_tail(&pobj->list, &pslot->free_objs);
        ++pslot->objs_free;
    }

    return SUCCESS;
}

/*
* Free block, unpartitioning objects in block
*/
static void fep_slab_block_free(fep_slab_t *pslab, fep_slab_blk_t *pblk, fep_slab_slot_t *pslot) {
    if ((! pslab) || (! pblk) || (! pslot))
        return;

    int objs_per_blk = (FEP_SLAB_SZ_BLK - sizeof(fep_slab_blk_t)) / pblk->obj_size;
    fep_slab_obj_t *pobj;

    for (pobj = (fep_slab_obj_t *) (pblk + 1); objs_per_blk > 0; 
         pobj = (fep_slab_obj_t *) (((char *) pobj) + pblk->obj_size), --objs_per_blk) {
        fep_slab_list_del(&pobj->list);
        --pslot->objs_free;
    }

    pblk->obj_size = 0;
    pblk->slot     = UNINITIALIZED;

    fep_slab_list_add_tail(&pblk->list, &pslab->free_blks);
    ++pslab->blks_free;
    --pslab->blks_in_use;
}

/*
* ======================================================================
* Private functions for maintaining internal FEP memory behavior,
* specifically "show memory" support
* ======================================================================
*/

/*
* Maintain behavior in diam_lib/fep_memory.c:fep_create_private()
*/
static void fep_slab_fep_create_private(PrivInfo *pPrivInfo) {
    if (! pPrivInfo)
        return;

    Memory *pMemory = pPrivInfo->pMemory;

    /*
    * Populate Memory struct
    */
    pMemory->mem_address = NULL;
    pMemory->mem_count   = pPrivInfo->ulCount;
    pMemory->mem_length  = pPrivInfo->ulCount * *pPrivInfo->pulBsize;
    pMemory->mem_name    = pPrivInfo->pszPrivName;
    pMemory->mem_size    = *pPrivInfo->pulBsize;
    pMemory->mem_type    = MEMORY_PRIVATE;

    /*
    * Add Memory struct to both "show memory" linked lists
    */
    map_insert_both(pMemory);
}

/*
* Maintain behavior in diam_lib/fep_memory.c:new_private()
*/
static void *fep_slab_new_private(Memory *pMemory, void *pdata) {
    if (! pMemory)
        return NULL; // pdata should've been NULL

    if (! pdata) {
        ++pMemory->mem_fail;
        return pdata; // pdata is NULL
    }

    /*
    * Count successful allocation
    */
    MEMORY_ALLOCATED(pMemory);

    return pdata;
}

/*
* Maintain behavior in diam_lib/fep_memory.c:delete_private()
*/
static void fep_slab_delete_private(Memory *pMemory) {
    if (! pMemory)
        return;

    /*
    * Count successful deallocation
    */
    MEMORY_DEALLOCATED(pMemory);
}

/*
* ======================================================================
* Public functions for using slab allocator
* ======================================================================
*/

/*
* Initialize slab allocator
*/
int fep_slab_init(fep_slab_t *pslab, const char *slab_name, size_t obj_align, 
                  ulong flags, const fep_slab_slot_info_t pslotinfo[], 
                  void *pstore, size_t zstore) {
    char me[] = "fep_slab_init";

    if (! pslab)
        return FAILURE;

    memset(pslab, 0, sizeof(*pslab));
    strncpy(pslab->name, slab_name, (sizeof(pslab->name) - 1));
    pslab->flags = flags; // unused
    pthread_mutex_init(&pslab->mutex, NULL);

    /*
    * Use memory store passed in to set up free block list
    */
    fep_slab_list_init(&pslab->free_blks);

    char *next = (char *) pstore; // memory store
    long left = zstore; // initial memory store size

    /*
    * Align store pointer to 1k boundary
    */
    next = (char *) ((((ulong) next + 1024 - 1) / 1024) * 1024);
    left -= (((ulong) next) - ((ulong) pstore));

    xprint("%s: pstore %p, next %p, zstore %lu, left %ld\n", me, pstore, next, zstore, left);

    /*
    * Set the ptr to the first block in the store.
    */
    pslab->pblks = (fep_slab_blk_t*) next;

    /*
    * Partition memory store into blocks
    */
    while (left >= FEP_SLAB_SZ_BLK) {
        fep_slab_blk_t *pblk = (fep_slab_blk_t *) next;

        pblk->magic    = FEP_SLAB_MAGIC_BLK;
        pblk->obj_size = 0;
        pblk->slot     = UNINITIALIZED;

        fep_slab_list_add_tail(&pblk->list, &pslab->free_blks);
        ++pslab->blks_free;

        next += FEP_SLAB_SZ_BLK;
        left -= FEP_SLAB_SZ_BLK;
    }

    /*
    * Remember the # of blocks in the store (for debug only).
    */
    pslab->nblks = pslab->blks_free;

    /*
    * Use slot info array to set up size slots
    */
    int i;
    for (i = 0; i < FEP_SLAB_MAX_SLOTS; ++i, ++pslab->n_slots, ++pslotinfo) {
        if (pslotinfo->data_size <= 0 )
            break;

        fep_slab_slot_t *pslot = &pslab->slots[i];
        Memory *pMemory = &pslot->memory;
        PrivInfo *pPrivInfo = &pslot->privinfo;

        pslot->n_objs = pslotinfo->n_objs;
        pslot->data_size = ((pslotinfo->data_size + obj_align - 1) / obj_align ) * obj_align;

        fep_slab_list_init(&pslot->free_objs);
        fep_slab_list_init(&pslot->used_objs);

        /*
        * Set up internal FEP memory structs
        */
        strncpy(pMemory->mem_text, pslotinfo->slot_name, (sizeof(pMemory->mem_text) - 1));
        strncpy(pMemory->mem_struct, pslotinfo->type_name, (sizeof(pMemory->mem_struct) - 1));
        pMemory->mem_fatal = 0;

        pPrivInfo->pszPrivName = pMemory->mem_text;
        pPrivInfo->pulBsize = &pslot->data_size;
        pPrivInfo->pMemory = pMemory;
        pPrivInfo->pvStaticAlloc = NULL;

        /*
        * Calculate maximum # objects (i.e., user data) that can be created for
        * each slot, assuming ALL memory is devoted to slot
        */
        int obj_size = sizeof(fep_slab_obj_t) + pslot->data_size;
        int objs_per_blk = (FEP_SLAB_SZ_BLK - sizeof(fep_slab_blk_t)) / obj_size;

        pPrivInfo->ulCount = pslab->blks_free * objs_per_blk;
        if (pPrivInfo->ulCount > (ulong) pslot->n_objs)
            pPrivInfo->ulCount = pslot->n_objs; // Use user-provided max # objs

        /*
        * Do some (not all) "fep_create_private()" actions
        */
        fep_slab_fep_create_private(pPrivInfo);

        xprint("%s: Slot %d created, mem_text (slot name) %s, mem_struct (type name) %s, data_size %lu, ulCount (max # objs) %lu, calculated ulCount %lu\n", me, i, pMemory->mem_text, pMemory->mem_struct, pslot->data_size, pPrivInfo->ulCount, (pslab->blks_free * objs_per_blk));
    }

    xprint("%s: FEP slab allocator \"%s\" created, n_slots %d, blks_free %d\n", me, pslab->name, pslab->n_slots, pslab->blks_free);

    return SUCCESS;
}

/*
* Allocate user data type from slot
*/
void *fep_slab_alloc_from_slot(fep_slab_t *pslab, int slot) {
    if (! pslab)
        return fep_slab_new_private(NULL, NULL);

    if ((slot < 0) || (slot >= pslab->n_slots))
        return fep_slab_new_private(NULL, NULL);

    fep_slab_slot_t *pslot = &pslab->slots[slot];

    pthread_mutex_lock(&pslab->mutex);

    /*
    * If there are no free objects of this size, try to alloc some more
    */
    if (fep_slab_list_is_empty(&pslot->free_objs))
        if (fep_slab_block_alloc(pslab, slot) != SUCCESS) {
            pthread_mutex_unlock(&pslab->mutex);
            return fep_slab_new_private(&pslot->memory, NULL);
        }

    fep_slab_obj_t *pobj = (fep_slab_obj_t *) fep_slab_list_pop_head(&pslot->free_objs );

    /*
    * Free objects list returned NULL?!
    */
    if (! pobj) {
        ++pslot->n_fail;
        pthread_mutex_unlock(&pslab->mutex);

        return fep_slab_new_private(&pslot->memory, NULL);
    }

    ++pslot->n_allocs;
    ++pobj->parent->blkref;
    pobj->hdrnum = pslab->next_hdrnum;
    ++pslab->next_hdrnum;

    /*
    * Skip fep_slab_obj_t (i.e., object header)
    */
    void *pdata = (void *) (pobj + 1);

    /*
    * Put object on used list
    */
    fep_slab_list_add_head(&pobj->list, &pslot->used_objs);
    ++pslot->objs_in_use;
    --pslot->objs_free;

    if (pslot->objs_in_use > pslot->objs_max)
        pslot->objs_max = pslot->objs_in_use; // Update watermark

    pthread_mutex_unlock(&pslab->mutex);

    /*
    * Do some (not all) "new_private()" actions
    */
    return fep_slab_new_private(&pslot->memory, pdata);
}

/*
* Allocate user data type
*/
void *fep_slab_alloc(fep_slab_t *pslab, size_t size) {
    if (! pslab)
        return fep_slab_new_private(NULL, NULL);

    if (size <= 0)
        return fep_slab_new_private(NULL, NULL);

    /*
    * Choose smallest slot size that satisfies request
    */
    int i, j;
    for (i = 0, j = -1; i < pslab->n_slots; ++i)
        if (size <= pslab->slots[i].data_size)
            if ((j < 0) || (pslab->slots[i].data_size < pslab->slots[j].data_size))
                j = i;

    return ((j < 0) ? (fep_slab_new_private(NULL, NULL)) : (fep_slab_alloc_from_slot(pslab, j)));
}

/*
* Free user data type
*/
void fep_slab_free(fep_slab_t *pslab, void *pdata) {
    if ((! pslab) || (! pdata))
        return;

    fep_slab_obj_t *pobj = ((fep_slab_obj_t *) pdata) - 1;
    int slot = pobj->parent->slot;

    if ((slot < 0) || (slot >= pslab->n_slots))
        return;

    fep_slab_slot_t *pslot = &pslab->slots[slot];

    pthread_mutex_lock(&pslab->mutex);

    /*
    * Remove object from used list
    */
    fep_slab_list_del(&pobj->list);
    --pslot->objs_in_use;

    /*
    * Add removed object to free list
    */
    fep_slab_list_add_tail(&pobj->list, &pslot->free_objs);
    ++pslot->objs_free;

    pobj->hdrnum = FEP_SLAB_INIT_HDRNUM;
    --pobj->parent->blkref;

    if (pobj->parent->blkref <= 0) // Block is now empty
        fep_slab_block_free(pslab, pobj->parent, pslot);

    /*
    * Do some (not all) "delete_private()" actions
    */
    fep_slab_delete_private(&pslot->memory);

    pthread_mutex_unlock(&pslab->mutex);
}

/*
* Calculate memory store size given # user data objects, user data size,
* alignment
*/
size_t calc_zstore(int n_objs, size_t data_size, size_t obj_align) {
    data_size = ((data_size + obj_align - 1) / obj_align) * obj_align;

    ulong obj_size = sizeof(fep_slab_obj_t) + data_size;
    ulong objs_per_blk = (FEP_SLAB_SZ_BLK - sizeof(fep_slab_blk_t)) / obj_size;
    ulong n_blks = (n_objs / objs_per_blk) + 1;

    ++n_blks; // Pad extra block

    size_t zstore = n_blks * FEP_SLAB_SZ_BLK;

    return zstore;
}

/*
* ======================================================================
* Public functions for using ID generator
* ======================================================================
*/

/*
* Initialize ID generator
*/
int fep_id_gen_init(fep_id_gen_t *pid_gen, ulong n_ids) {
    if (! pid_gen)
        return FAILURE;

    fep_slab_list_init(&pid_gen->free_ids);
    fep_slab_list_init(&pid_gen->used_ids);
    pthread_mutex_init(&pid_gen->mutex, NULL);
    pid_gen->n_ids = n_ids;
    pid_gen->ids = (fep_id_t *) calloc(pid_gen->n_ids, sizeof(fep_id_t));

    if (! pid_gen->ids)
        return FAILURE;

    ulong i;
    for (i = 0; i < pid_gen->n_ids; ++i) {
        fep_id_t *pid = &pid_gen->ids[i];

        pid->id = i + 1;
        fep_slab_list_add_tail(&pid->list, &pid_gen->free_ids);
    }

    xprint("FEP id gen: ids %d, size %dMB\n", n_ids, 
           (n_ids * sizeof(fep_id_t)) >> 20 );

    return SUCCESS;
}

/*
* Free ID generator (i.e., ID array)
*/
void fep_id_gen_free(fep_id_gen_t *pid_gen) {
    if (! pid_gen)
        return;

    free(pid_gen->ids);
}

/*
* Allocate ID & associate ID with user data
*/
fep_id_t *fep_id_alloc(fep_id_gen_t *pid_gen, void *pdata) {
    if (! pid_gen)
        return NULL;

    pthread_mutex_lock(&pid_gen->mutex);

    if (fep_slab_list_is_empty(&pid_gen->free_ids)) {
        pthread_mutex_unlock(&pid_gen->mutex);
        return NULL;
    }

    fep_id_t *pid = (fep_id_t *) fep_slab_list_pop_head(&pid_gen->free_ids);

    /*
    * Free IDs list returned NULL?!
    */
    if (! pid) {
        pthread_mutex_unlock(&pid_gen->mutex);
        return NULL;
    }

    pid->pdata = pdata;
    fep_slab_list_add_tail(&pid->list, &pid_gen->used_ids);

    pthread_mutex_unlock(&pid_gen->mutex);

    return pid;
}

/*
* Free ID & unassociate ID with user data
*/
void fep_id_free(fep_id_gen_t *pid_gen, fep_id_t *pid) {
    if ((! pid_gen) || (! pid))
        return;

    pthread_mutex_lock(&pid_gen->mutex);

    pid->pdata = NULL;

    fep_slab_list_del(&pid->list);
    fep_slab_list_add_tail(&pid->list, &pid_gen->free_ids);

    pthread_mutex_unlock(&pid_gen->mutex);
}

/*
* Free ID & unassociate ID with user data
*/
void fep_id_free_by_id(fep_id_gen_t *pid_gen, ulong id) {
    if (! pid_gen)
        return;

    if ((id < 1) || (id > pid_gen->n_ids))
        return;

    ulong index = id - 1;
    fep_id_t *pid = &pid_gen->ids[index];

    fep_id_free(pid_gen, pid);
}

/*
* Get user data given ID
*/
void *fep_id_get_data(fep_id_gen_t *pid_gen, ulong id) {
    if (! pid_gen)
        return NULL;

    if ((id < 1) || (id > pid_gen->n_ids))
        return NULL;

    ulong index = id - 1;
    fep_id_t *pid = &pid_gen->ids[index];

    return pid->pdata;
}
