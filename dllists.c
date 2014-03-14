
#include <stdlib.h>
#include <string.h>


#include "dllists.h"

dll_listh_t * dll_alloc_list (int listhead_size){
	if (listhead_size<sizeof(dll_listh_t)) return NULL;
	dll_listh_t * res=malloc(listhead_size);
	if (!res) return NULL; //out of mem?
	dll_init_list (res,DLL_LIST_MEM_FLAG_FREE_HEAD);
	if (listhead_size>sizeof(dll_listh_t)) res->dll_list_user_data=(char *)(res+1);
	return res;
}

dll_listh_t * dll_alloc_list_ex (int user_data_size, void ** list_user_data){
	dll_listh_t * res=dll_alloc_list(sizeof(dll_listh_t)+user_data_size);
	if (!res) return NULL; //out of mem?
	if (list_user_data) *list_user_data=res->dll_list_user_data;
	return res;
}

dll_listh_t * dll_init_list (dll_listh_t * list, int flags){
	if (!list) return NULL;
	memset (list,0,sizeof(dll_listh_t));
	list->dll_list_mem_flags=flags;
	return list;
}


void dll_deinit_list (dll_listh_t *list, int freeblocks){
	if (!list) return;
	if (freeblocks){
		dll_list_iterator_t li;
		dll_iterate_fwd(list,&li);
		dll_blockh_t *bl;
		while (NULL!=(bl=dll_next(&li))) dll_deinit_block (bl,0);
	}
	if (list->dll_list_user_data && (list->dll_list_mem_flags & DLL_LIST_MEM_FLAG_FREE_USER_DATA) == DLL_LIST_MEM_FLAG_FREE_USER_DATA ) free (list->dll_list_user_data);
	if ((list->dll_list_mem_flags & DLL_LIST_MEM_FLAG_FREE_HEAD) == DLL_LIST_MEM_FLAG_FREE_HEAD ) free (list);
}



dll_blockh_t * dll_alloc_block (int block_size,int initsize){
	if (block_size<sizeof(dll_blockh_t)) return NULL;
	dll_blockh_t * res=malloc(block_size);
	if (!res) return NULL; //out of mem

	if (initsize<sizeof(dll_blockh_t)) initsize=sizeof(dll_blockh_t);
	if (initsize>block_size) initsize=block_size;
	
	dll_init_block (res,initsize,DLL_BLOCK_MEM_FLAG_FREE_HEAD);
	if (block_size>sizeof(dll_blockh_t)) res->dll_block_user_data=(char *)(res+1);
	
	return res;
}

dll_blockh_t * dll_alloc_block_ex (int user_data_size, int initsize, void ** block_user_data){
	if (initsize>user_data_size) initsize=user_data_size;
	dll_blockh_t * res=dll_alloc_block(sizeof(dll_blockh_t)+user_data_size,sizeof(dll_blockh_t)+initsize);
	if (!res) return NULL; //out of mem
	if (block_user_data) *block_user_data=res->dll_block_user_data;
	return res;
}

dll_blockh_t * dll_init_block (dll_blockh_t * block,int sz, int flags){
	if (!block) return NULL;
	if (sz!=-1) {
		if (sz<=sizeof(dll_blockh_t)) sz=sizeof(dll_blockh_t);
		memset (block,0,sz);
	}
	block->dll_block_mem_flags=flags;
	return block;
	
}

void dll_deinit_block (dll_blockh_t *bl, int unlink){
	if (!bl) return;
	if (unlink && bl->dll_list) dll_rm(bl->dll_list,bl);
	if (bl->dll_block_user_data && (bl->dll_block_mem_flags & DLL_BLOCK_MEM_FLAG_FREE_USER_DATA) == DLL_BLOCK_MEM_FLAG_FREE_USER_DATA ) free (bl->dll_block_user_data);
	if ((bl->dll_block_mem_flags & DLL_BLOCK_MEM_FLAG_FREE_HEAD) == DLL_BLOCK_MEM_FLAG_FREE_HEAD ) free (bl);
}



dll_blockh_t *dll_add_after (dll_listh_t *list,  dll_blockh_t *block_after, dll_blockh_t *new_block){
	if (!list || !new_block) return NULL;
	if (list->dll_head==NULL) { // first elem
		new_block->dll_list=list;
		new_block->dll_next=NULL;
		new_block->dll_prev=NULL;
		list->dll_head=new_block;
		list->dll_tail=new_block;
	} else {
		if (block_after->dll_list!=list) return NULL;
		new_block->dll_list=list;
		
		new_block->dll_prev=block_after;
		new_block->dll_next=block_after->dll_next;
		
		block_after->dll_next=new_block;
		if (list->dll_tail==block_after) list->dll_tail=new_block;
		else new_block->dll_next->dll_prev=new_block; 
	}
	return new_block;
}

dll_blockh_t *dll_add_before (dll_listh_t *list,  dll_blockh_t *block_before, dll_blockh_t *new_block) {
	if (!list || !new_block) return NULL;
	if (list->dll_head==NULL) { // first elem
		new_block->dll_list=list;
		new_block->dll_next=NULL;
		new_block->dll_prev=NULL;
		list->dll_head=new_block;
		list->dll_tail=new_block;
	} else {
		if (block_before->dll_list!=list) return NULL;
		new_block->dll_list=list;
		
		new_block->dll_next=block_before;
		new_block->dll_prev=block_before->dll_prev;
		
		block_before->dll_prev=new_block;
		if (list->dll_head==block_before) list->dll_head=new_block;
		else new_block->dll_prev->dll_next=new_block;
	}
	return new_block;
}

dll_blockh_t *dll_rm (dll_listh_t *list,  dll_blockh_t *block) {
	if (!list || !block) return NULL;
	if (list!=block->dll_list) return NULL;
	if (list->dll_head==block) {
		list->dll_head=block->dll_next;
		if (list->dll_head) list->dll_head->dll_prev=NULL;
	}
	if (list->dll_tail==block) {
		list->dll_tail=block->dll_prev;
		if (list->dll_tail) list->dll_tail->dll_next=NULL;
	}
	
	if (block->dll_next) block->dll_next->dll_prev=block->dll_prev;
	if (block->dll_prev) block->dll_prev->dll_next=block->dll_next;
	block->dll_next=NULL;
	block->dll_prev=NULL;
	block->dll_list=NULL;
	return block;
}

void dll_iterate_fwd (dll_listh_t*list, dll_list_iterator_t*iterator){
	if (iterator){
		memset (iterator,0,sizeof(dll_list_iterator_t));
		iterator->eol=1;
	}
	if (!list || !iterator) return;
	
	iterator->list=list;
	if (list->dll_head==NULL) {
		iterator->eol=1;
	} else {
		iterator->eol=0;
		iterator->cur=NULL;
		iterator->next=list->dll_head;
	}
}

void dll_iterate_bwd (dll_listh_t*list, dll_list_iterator_t*iterator){
	if (iterator){
		memset (iterator,0,sizeof(dll_list_iterator_t));
		iterator->eol=1;
		iterator->bwd=1;
	}
	if (!list || !iterator) return;
	
	iterator->list=list;
	if (list->dll_tail==NULL) {
		iterator->eol=1;
	} else {
		iterator->eol=0;
		iterator->cur=NULL;
		iterator->next=list->dll_tail;
	}
}

dll_blockh_t * dll_next (dll_list_iterator_t*iterator){
	if (!iterator || iterator->eol) return NULL;
	iterator->cur=iterator->next;
	
	if (!iterator->cur) {
		iterator->eol=1;
		return NULL;
	}
	
	if (iterator->bwd){
		iterator->next=iterator->cur->dll_prev;
	} else {
		iterator->next=iterator->cur->dll_next;
	}
	if (!iterator->next) {
		iterator->eol=1;
	}
	return iterator->cur;
}
