/**************************************************************************//**
@File		aiop_virtual_pools.c

@Description	This file contains the AIOP Virtual Pools 
				Operations implementation.
*//***************************************************************************/

#include "virtual_pools.h"
#include "common/spinlock.h"
#include "dplib/fsl_cdma.h"

//struct virtual_pool_desc virtual_pools[MAX_VIRTUAL_POOLS_NUM];
struct bman_pool_desc virtual_bman_pools[MAX_VIRTUAL_BMAN_POOLS_NUM];

struct virtual_pools_root_desc virtual_pools_root;


/***************************************************************************
 * vpool_create_pool
 ***************************************************************************/
int32_t vpool_create_pool( 
		uint16_t bman_pool_id, 
		int32_t max_bufs, 
		int32_t committed_bufs,
		uint32_t flags,
		int32_t (*callback_func)(uint64_t),
		uint32_t *virtual_pool_id)
{
	
	uint32_t vpool_id;
	uint32_t num_of_virtual_pools = virtual_pools_root.num_of_virtual_pools;
    uint16_t bman_array_index;
	int i;
	
	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	struct callback_s *callback = 
			(struct callback_s *)virtual_pools_root.callback_func_struct;

	#ifdef SL_DEBUG
		/* Check the arguments correctness */
		if (bman_pool_id >= MAX_VIRTUAL_BMAN_POOLS_NUM)
			return VIRTUAL_POOLS_ILLEGAL_ARGS;

		/* max_bufs must be equal or greater than committed_bufs */
		if (committed_bufs > max_bufs)
			return VIRTUAL_POOLS_ILLEGAL_ARGS;	
	
		/* committed_bufs and max_bufs must not be 0 */
		if ((!committed_bufs) || (!max_bufs))
			return VIRTUAL_POOLS_ILLEGAL_ARGS;	
	#endif
	
	/* Check which BMAN pool ID array element matches the ID */ 
	for (i=0; i< MAX_VIRTUAL_BMAN_POOLS_NUM; i++) {
		if (virtual_bman_pools[i].bman_pool_id == bman_pool_id) {
			bman_array_index = (uint16_t)i;
			break;
		}
	}
	// TODO: check if out of range and return error
		
	/* Spinlock this BMAN pool counter */
	lock_spinlock((uint8_t *)&virtual_bman_pools[bman_array_index].spinlock);
	
	/* Check if there are enough buffers to commit */
	if (virtual_bman_pools[bman_array_index].remaining >= committed_bufs) {
		/* decrement the total available BMAN pool buffers */ 
		virtual_bman_pools[bman_array_index].remaining -= committed_bufs;
		
		unlock_spinlock((uint8_t *)&virtual_bman_pools[bman_array_index].spinlock);

	} else {
		unlock_spinlock((uint8_t *)&virtual_bman_pools[bman_array_index].spinlock);
		return VIRTUAL_POOLS_INSUFFICIENT_BUFFERS;
	}
		
	lock_spinlock((uint8_t *)&virtual_pools_root.global_spinlock);
	
	/* Allocate a virtual pool ID */
	/* Return with error if it was not possible to allocate a virtual pool */
	for(vpool_id = 0; vpool_id < num_of_virtual_pools; vpool_id++) {
		if(virtual_pool->max_bufs == 0) {
			virtual_pool->max_bufs = max_bufs; /* use max_bufs as indicator */
			break;
		}
		virtual_pool++; /* increment the pointer */
	}
	
	unlock_spinlock((uint8_t *)&virtual_pools_root.global_spinlock);
	
	*virtual_pool_id = vpool_id; /* Return the ID */

	/* Return with error if no pool is available */
	if (vpool_id == num_of_virtual_pools)
		return VIRTUAL_POOLS_ID_ALLOC_FAIL;
	
	virtual_pool->committed_bufs = committed_bufs;
	virtual_pool->allocated_bufs = 0; 
	virtual_pool->bman_array_index = bman_array_index;
	virtual_pool->flags = (uint8_t)flags;

	
	/* Check if a callback structure exists and initialize the entry */
	if (virtual_pools_root.callback_func_struct != NULL) {
		callback += vpool_id;
		callback->callback_func = callback_func;
	}

	return VIRTUAL_POOLS_SUCCESS;
} /* End of vpool_create_pool */


/***************************************************************************
 * vpool_release_pool
 ***************************************************************************/
int32_t vpool_release_pool(uint32_t virtual_pool_id)
{

	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	virtual_pool += virtual_pool_id;
	
	lock_spinlock((uint8_t *)&virtual_pools_root.global_spinlock);
	
	
	if (virtual_pool->allocated_bufs != 0) {
		unlock_spinlock((uint8_t *)&virtual_pools_root.global_spinlock);
		return VIRTUAL_POOLS_RELEASE_POOL_FAILED;	
	}
	
	/* max_bufs = 0 indicates a free pool */
	virtual_pool->max_bufs = 0; 
	
	unlock_spinlock((uint8_t *)&virtual_pools_root.global_spinlock);
	
	/* Increment the total available BMAN pool buffers */ 
	atomic_incr32(&virtual_bman_pools[virtual_pool->bman_array_index].remaining, 
			virtual_pool->committed_bufs);
		
	return VIRTUAL_POOLS_SUCCESS;	
} /* End of vpool_release_pool */

/***************************************************************************
 * vpool_read_pool
 ***************************************************************************/
int32_t vpool_read_pool(uint32_t virtual_pool_id, 
		uint16_t *bman_pool_id, 
		int32_t *max_bufs, 
		int32_t *committed_bufs,
		int32_t *allocated_bufs,
		uint32_t *flags,
		int32_t *callback_func)


{
	
	struct callback_s *callback;
			
	// TODO: remove this if moving to handle
	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	
	virtual_pool += virtual_pool_id;
	
	*max_bufs = virtual_pool->max_bufs;
	*committed_bufs = virtual_pool->committed_bufs;
	*allocated_bufs = virtual_pool->allocated_bufs; 
	*bman_pool_id = virtual_bman_pools[virtual_pool->bman_array_index].bman_pool_id;
	
	*flags =	(uint8_t)virtual_pool->flags;

	/* Check if callback exists and return its address (can be null) */
	if (virtual_pools_root.callback_func_struct != NULL) {
		callback = 
				(struct callback_s *)virtual_pools_root.callback_func_struct;
		callback += virtual_pool_id;
		*callback_func = (int32_t)callback->callback_func;
	} else {
		*callback_func = 0;
	}
	
	return VIRTUAL_POOLS_SUCCESS;	
} /* End of vpool_read_pool */

/***************************************************************************
 * vpool_init
 ***************************************************************************/
int32_t vpool_init(
		uint64_t virtual_pool_struct, 
		uint64_t callback_func_struct,
		uint32_t num_of_virtual_pools,
		uint32_t flags)
{
	int32_t i;
	
	struct virtual_pool_desc *virtual_pool;

	/* Mask when down-casting.
	 *  Currently the address is only in Shared RAM (32 bit) 
	*/
	virtual_pool = 
			(struct virtual_pool_desc *)(virtual_pool_struct & 0xFFFFFFFF);
	
	virtual_pools_root.virtual_pool_struct = virtual_pool_struct;
	virtual_pools_root.callback_func_struct = callback_func_struct;
	virtual_pools_root.num_of_virtual_pools = num_of_virtual_pools;
	virtual_pools_root.flags = flags;
	
	/* Init 'max' to zero, since it's an indicator to pool ID availability */
	for(i = 0; i < num_of_virtual_pools; i++) {
		virtual_pool->max_bufs = 0;
		virtual_pool->spinlock = 0; /* clear spinlock indicator */
		virtual_pool++; /* increment the pointer */
	}

	/* Init 'remaining' to -1, since it's an indicator an empty index */
	for (i=0; i< MAX_VIRTUAL_BMAN_POOLS_NUM; i++) {
		virtual_bman_pools[i].remaining = -1;
		virtual_bman_pools[i].spinlock = 0; /* clear spinlock indicator */
	}
	
	return VIRTUAL_POOLS_SUCCESS;	
} /* End of vpool_init */


/***************************************************************************
 * vpool_init_total_bman_bufs
 ***************************************************************************/
int32_t vpool_init_total_bman_bufs( 
		uint16_t bman_pool_id, 
		int32_t total_avail_bufs, 
		uint32_t buf_size)
{
	int i;
	
	#ifdef SL_DEBUG
		/* Check the arguments correctness */
		if (bman_pool_id >= MAX_VIRTUAL_BMAN_POOLS_NUM)
			return VIRTUAL_POOLS_ILLEGAL_ARGS;
	#endif

	//TODO: bman_pool_id as index is a problem
	for (i=0; i< MAX_VIRTUAL_BMAN_POOLS_NUM; i++) {
		// check if virtual_bman_pools[i] is empty
		if (virtual_bman_pools[i].remaining == -1) {
			virtual_bman_pools[i].bman_pool_id = bman_pool_id; 
			virtual_bman_pools[i].remaining = total_avail_bufs;
			virtual_bman_pools[i].buf_size = buf_size;
			break;
		}
	}
	
	// TODO: check if out of range and return error

	return VIRTUAL_POOLS_SUCCESS;
} /* End of vpool_init_total_bman_bufs */


/***************************************************************************
 * vpool_add_total_bman_bufs
 ***************************************************************************/
int32_t vpool_add_total_bman_bufs( 
		uint16_t bman_pool_id, 
		int32_t additional_bufs)
{
	
	int i;
	uint16_t bman_array_index;
	
	#ifdef SL_DEBUG
		/* Check the arguments correctness */
		if (bman_pool_id >= MAX_VIRTUAL_BMAN_POOLS_NUM)
				return VIRTUAL_POOLS_ILLEGAL_ARGS;
	#endif
		
	/* Check which BMAN pool ID array element matches the ID */ 
	for (i=0; i< MAX_VIRTUAL_BMAN_POOLS_NUM; i++) {
		if (virtual_bman_pools[i].bman_pool_id == bman_pool_id) {
			bman_array_index = (uint16_t)i;
			break;
		}
	}
		
	/* Increment the total available BMAN pool buffers */ 
	atomic_incr32(&virtual_bman_pools[bman_array_index].remaining, 
			additional_bufs);

	return VIRTUAL_POOLS_SUCCESS;
} /* End of vpool_add_total_bman_bufs */

/***************************************************************************
 * vpool_allocate_buf
 ***************************************************************************/
int32_t vpool_allocate_buf(uint32_t virtual_pool_id, 
		uint64_t *context_address)
{
	int32_t return_val;
	int allocate = 0;
	
	// TODO: remove this if moving to handle
	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	virtual_pool += virtual_pool_id;

	lock_spinlock((uint8_t *)&virtual_pool->spinlock);

	/* First check if there are still available buffers in the VP committed area */ 
	if(virtual_pool->allocated_bufs < 
			virtual_pool->committed_bufs) 	{
		allocate = 1; /* allocated from committed area */
	/* Else, check if there are still available buffers in the VP max-committed area */ 
	} else if (virtual_pool->allocated_bufs < virtual_pool->max_bufs) {
		/* There is still an extra space in the virtual pool, check BMAN pool */
		
		/* spinlock this BMAN pool counter */
		lock_spinlock((uint8_t *)&virtual_bman_pools[virtual_pool->
		                                       bman_array_index].spinlock);

		if ((virtual_bman_pools[virtual_pool->bman_array_index].remaining) > 0)
		{ 
			allocate = 2; /* allocated from remaining area */
			virtual_bman_pools[virtual_pool->bman_array_index].remaining--; 
		}
		
		unlock_spinlock((uint8_t *)&virtual_bman_pools[virtual_pool->
		                                       bman_array_index].spinlock);
	}
		
	/* Request CDMA to allocate a buffer*/
	if (allocate) {
		
		virtual_pool->allocated_bufs++;
	
		unlock_spinlock((uint8_t *)&virtual_pool->spinlock);

		/* allocate a buffer with the CDMA */
		return_val = (int32_t)cdma_acquire_context_memory(
				(uint32_t)virtual_bman_pools[virtual_pool->bman_array_index].buf_size, 
				(uint16_t)virtual_bman_pools[virtual_pool->bman_array_index].bman_pool_id, 
				(uint64_t *)context_address); /* uint64_t *context_memory */ 

		/* If allocation failed, undo the counters increment/decrement */
		if (return_val) {
			atomic_decr32(&virtual_pool->allocated_bufs, 1); 
			if (allocate == 2) /* only if it was allocated from the remaining area */ 
				atomic_incr32(&virtual_bman_pools[virtual_pool->
				                               bman_array_index].remaining, 1);
			return (VIRTUAL_POOLS_CDMA_ERR | return_val);
		}
		
		return VIRTUAL_POOLS_SUCCESS;
	
	} else {
		unlock_spinlock((uint8_t *)&virtual_pool->spinlock);
		return VIRTUAL_POOLS_BUF_ALLOC_FAIL;
	}
	
} /* End of vpool_allocate_buf */


/***************************************************************************
 * vpool_release_buf
 ***************************************************************************/
int32_t vpool_release_buf(uint32_t virtual_pool_id, 
		uint64_t context_address)
{
	int32_t return_val;
	int32_t cdma_status;
	
	cdma_status = cdma_release_context_memory(context_address);
	
	if (!cdma_status) {
		return_val = __vpool_internal_release_buf(virtual_pool_id); 
		return return_val; 
	} else {
		/* Keep original CDMA return value */
		return VIRTUAL_POOLS_BUF_NOT_RELEASED | cdma_status; 
	}
		
} /* End of vpool_release_buf */

/***************************************************************************
 * __vpool_internal_release_buf
 ***************************************************************************/
int32_t __vpool_internal_release_buf(uint32_t virtual_pool_id)
{
	
	// TODO: remove this if moving to handle
	struct virtual_pool_desc *virtual_pool = 
			(struct virtual_pool_desc *)virtual_pools_root.virtual_pool_struct;
	virtual_pool += virtual_pool_id;
	
	lock_spinlock((uint8_t *)&virtual_pool->spinlock);

	/* First check if buffers were allocated from the common pool */ 
	if(virtual_pool->allocated_bufs > 
			virtual_pool->committed_bufs) 	{
		/* One buffer returns to the common pool */
		atomic_incr32(&virtual_bman_pools[virtual_pool->bman_array_index].remaining, 1);
	}	
	
	virtual_pool->allocated_bufs--;
	
	unlock_spinlock((uint8_t *)&virtual_pool->spinlock);
	
	return VIRTUAL_POOLS_SUCCESS;
} /* End of __vpool_internal_release_buf */


/***************************************************************************
 * vpool_refcount_increment
 ***************************************************************************/
int32_t vpool_refcount_increment(uint64_t context_address)
{
	return cdma_refcount_increment(context_address);
}

/***************************************************************************
 * vpool_decr_ref_counter
 ***************************************************************************/
int32_t vpool_refcount_decrement_and_release(
		uint32_t virtual_pool_id,
		uint64_t context_address,
		int32_t *callback_status)
{
	int32_t return_val;
	int32_t cdma_status;
	int32_t release = FALSE;
	int32_t no_callback = TRUE;
	struct callback_s *callback; 

	/* cdma_refcount_decrement_and_release: 
	 *	This routine decrements reference count of Context memory
	 *	object. If resulting reference count is zero, the following
	 *	CDMA_REFCOUNT_DECREMENT_TO_ZERO status code is reported
	 *	and the Context memory block is automatically released to the
	 *	BMan pool it was acquired from.
	*/
	
	// TODO: need to take care of callback if moving to handle
	// It can probably not be in a separate structure, since only 
	// vpool_id is given. 
	
	*callback_status = 0;
			
	/* Check if a callback structure and function exist */
	if (virtual_pools_root.callback_func_struct != NULL) {
		callback = 
				(struct callback_s *)virtual_pools_root.callback_func_struct;
		callback += virtual_pool_id;
		if (callback->callback_func != NULL) {
			no_callback = FALSE;
			/* Decrement ref counter without release */
			cdma_status = cdma_refcount_decrement(context_address);
			if (cdma_status == CDMA_REFCOUNT_DECREMENT_TO_ZERO) {
				/* Call the callback function */
				*callback_status = callback->callback_func(context_address);
				/* Release the buffer */
				cdma_status = cdma_release_context_memory(context_address);
				if (!cdma_status) {
					release = TRUE;
				}
			}
		}
	} 
	
	if (no_callback) {
		/* decrement and release without a callback */
		cdma_status = cdma_refcount_decrement_and_release(context_address);
		if (cdma_status == CDMA_REFCOUNT_DECREMENT_TO_ZERO) {
			release = TRUE;
			cdma_status = 0;
		}	
	}

	// TODO: the return value is inconsistent with cdma_refcount_decrement_and_release
	
	if (release) {
		return_val = __vpool_internal_release_buf(virtual_pool_id);
		return return_val;
	} else {
		/* Keep CDMA return value in case of error */
		return VIRTUAL_POOLS_BUF_NOT_RELEASED | cdma_status; 
	}
} /* End of vpool_refcount_decrement_and_release */








