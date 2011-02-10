#ifndef dllist_h_SDR_2729234zxssnxcdqhkd_included
#define dllist_h_SDR_2729234zxssnxcdqhkd_included

//#include <pthread.h>

#ifdef  __cplusplus
extern "C" {
#endif


struct dll_blockh_tag;
typedef struct dll_blockh_tag dll_blockh_t;
struct dll_listh_tag;
typedef struct dll_listh_tag dll_listh_t;
struct dll_list_iterator_tag;
typedef struct dll_list_iterator_tag dll_list_iterator_t;

#define DLL_LIST_MEM_FLAG_FREE_NONE 0
#define DLL_LIST_MEM_FLAG_FREE_HEAD 1 
#define DLL_LIST_MEM_FLAG_FREE_USER_DATA 2 

struct dll_listh_tag {
	dll_blockh_t *dll_head,*dll_tail;
	int dll_list_mem_flags;
	char * dll_list_user_data;
};

#define DLL_BLOCK_MEM_FLAG_FREE_NONE 0 
#define DLL_BLOCK_MEM_FLAG_FREE_HEAD 1 
#define DLL_BLOCK_MEM_FLAG_FREE_USER_DATA 2 

struct dll_blockh_tag {
	dll_listh_t *dll_list;
	dll_blockh_t *dll_prev,*dll_next;
	int dll_block_mem_flags;
	void * dll_block_user_data;
};

struct dll_list_iterator_tag {
	int bwd;
	int eol;
	dll_listh_t *list;
	dll_blockh_t *cur, *next; 
};

dll_listh_t * dll_alloc_list (int listhead_size);
dll_listh_t * dll_alloc_list_ex (int user_data_size,void ** list_user_data);
dll_listh_t * dll_init_list (dll_listh_t * list, int flags);


void dll_deinit_list (dll_listh_t *list, int freeblocks);



dll_blockh_t * dll_alloc_block (int block_size,int initsize);
dll_blockh_t * dll_alloc_block_ex (int user_data_size, int initsize, void** block_user_data);



dll_blockh_t * dll_init_block (dll_blockh_t* block, int sz, int flags);


#define DLL_DEINIT_BLOCK_UNLINK 1 
#define DLL_DEINIT_BLOCK_DONT_UNLINK 0
void dll_deinit_block (dll_blockh_t *bl, int unlink);



dll_blockh_t *dll_add_after (dll_listh_t *list,  dll_blockh_t *block_after, dll_blockh_t *new_block);
dll_blockh_t *dll_add_before (dll_listh_t *list,  dll_blockh_t *block_before, dll_blockh_t *new_block);
dll_blockh_t *dll_rm (dll_listh_t *list,  dll_blockh_t *block);

void dll_iterate_fwd (dll_listh_t*list, dll_list_iterator_t*iterator);
void dll_iterate_bwd (dll_listh_t*list, dll_list_iterator_t*iterator);
dll_blockh_t * dll_next (dll_list_iterator_t*iterator);

#ifdef  __cplusplus
}
#endif

#endif