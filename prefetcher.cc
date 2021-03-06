/*
*	This is a sample of a Reference Prediction Table(RPT) prefetcher
*/

#include "interface.hh"
#include <list>

using namespace std;

enum State {initial, evaluate, steady, idle};

class RPTEntry {
	public:
		Addr pc, lastAddr;
		int stride;
		State state;

		RPTEntry(Addr pc);
		void miss(Addr addr);
	private:
		void updateState(bool match);
};

RPTEntry::RPTEntry(Addr pc): pc(pc), lastAddr(0), stride(0), state(initial){}

void RPTEntry::miss(Addr addr){
	int delta = addr - lastAddr;
	if(delta != 0){
		bool match = (delta == stride);
		if(match && !in_mshr_queue(addr+delta) && !in_cache(addr+delta)){
			issue_prefetch(addr+delta);
		}
		lastAddr = addr;
		if(state != steady && !match) stride = delta;
		updateState(match);
	}
}

void RPTEntry::updateState(bool match){
	switch(state){
		case idle:
			state = match ? evaluate : idle;
			break;
		case evaluate:
			state = match ? steady : idle;
			break;
		case steady:
			state = match ? steady : initial;
			break;
		case initial:
			state = match ? steady : evaluate;
			break;
		default:
			state = idle;
			break;
	}
}




class RPTTable{
	public:
		static const int MAX_LIST_ENTRIES = 128;
		RPTTable();
		RPTEntry* get(Addr pc);
	private:
		list<RPTEntry*> table;
		
};

RPTTable::RPTTable(){}

RPTEntry* RPTTable::get(Addr pc){
	list<RPTEntry*>::iterator i;
	for(i=table.begin(); i!=table.end(); ++i){
		RPTEntry* entry = *i;
		if(entry->pc == pc){
			i = table.erase(i);
			table.push_front(entry);
			return entry;
		}
	}

	RPTEntry* newEntry = new RPTEntry(pc);
	if(table.size() > MAX_LIST_ENTRIES){
		RPTEntry* last = table.back();
		table.pop_back();
		delete last;
	}
	table.push_front(newEntry);
	return newEntry;
}


static RPTTable* rpttable;

void prefetch_init(void)
{
    /* Called before any calls to prefetch_access. */
    /* This is the place to initialize data structures. */
	rpttable = new RPTTable;

    DPRINTF(HWPrefetch, "Initialized sequential-on-access prefetcher\n");
}

void prefetch_access(AccessStat stat)
{
	RPTEntry* entry = rpttable->get(stat.pc);
	if(stat.miss) entry->miss(stat.mem_addr);
}

void prefetch_complete(Addr addr) {
    /*
     * Called when a block requested by the prefetcher has been loaded.
     */
}
