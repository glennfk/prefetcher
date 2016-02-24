/*
 * A sample prefetcher which does sequential one-block lookahead.
 * This means that the prefetcher fetches the next block _after_ the one that
 * was just accessed. It also ignores requests to blocks already in the cache.
 */

#include "interface.hh"

//#globals
/*int WindowSize = 1;
int Threshold = 1;
int Lock = 0;*/
int N = 3; //degree of prefetching (number of blocks issued after a miss)

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */

    DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
    /* pf_addr is now an address within the _next_ cache block */
    //Addr pf_addr = stat.mem_addr;
	/*int hits = 0;
    for(int n=0; n<WindowSize; i++){
    	Addr pf_nth_addr = stat.mem_addr + n*BLOCK_SIZE;
		if(!pf_nth_addr.miss && get_prefetch_bit(pf_nth_addr)){ hits++; }
    }*/
	Addr pf_ith_addr;
	for(int i=0; i<N; i++){
		pf_ith_addr = stat.mem_addr + (i+1)*BLOCK_SIZE;
		if(	stat.miss &&
			!in_cache(pf_ith_addr) &&
			!in_mshr_queue(pf_ith_addr)){
			issue_prefetch(pf_ith_addr);
		}
	}

    /*
     * Issue a prefetch request if a demand miss occured,
     * and the block is not already in cache.
     */
    /*if (stat.miss && !in_cache(pf_addr)) {
        issue_prefetch(pf_addr);
    }*/
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
