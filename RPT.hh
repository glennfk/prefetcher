#include <list>

namespace RPT {

	#define PREFETCHER_SIZE (8*1024)

	/*
	*	type: class
	*	name: RPTEntry
	*	description:
	*		The struct is 
	*/
	struct RPTEntry {
		Addr pc, lastAdress;
		int delta;

		RPTEntry(Addr pc);
		void issue_if_matching_delta(Addr addr);
	};
	/*
	*	type: constructor-function
	*	name: RPTEntry:RPTEntry
	*	arguments:
	*		@Addr pc
	*	description:
	*		The constructor sets the given @pc and initializes the other variables to 0.
	*/
	RPTEntry::RPTEntry(Addr pc): pc(pc), lastAdress(0), delta(0){}
	/*
	*	type: function
	*	name: RPTEntry::issue_if_matching_delta
	*	arguments:
	*		@Addr addr
	*	description:
	*		
	*/
	void RPTEntry::issue_if_matching_delta(Addr addr){
		int newDelta = addr-lastAdress;
		if(delta==newDelta && !in_mshr_queue(addr+delta) && !in_cache(addr+delta)){ issue_prefetch(addr+delta); }
		lastAdress = addr;
		delta = newDelta;
	}

	/*
	*	type: class
	*	name: RPTTable
	*	description:
	*		The class handles the interactions with a table of references to RPTEntry's.
	*		The maximum size of the list is given by the maximum size of the prefetcher
	*		and the size of each block in the prefetcher.
	*/
	class RPTTable {
		public:
			static const int MAX_LIST_ENTRIES = 128;//(int)(PREFETHCER_SIZE / BLOCK_SIZE);
			RPTTable();
			RPTEntry* get(Addr pc);
		private:
			std::list<RPTEntry*> table;
	};
	/*
	*	type: constructor-function
	*	name: RPTTable::RPTTable
	*	description:
	*		Constructor for this instance.
	*/
	RPTTable::RPTTable(){}

	/*
	*	type: function
	*	name: RPTEntry::get
	*	arguments:
	*		@Addr pc: program counter
	*	returns:
	*		@RPTEntry* entry/newEntry
	*	description:
	*		The function will check if the entry with given @pc is present in the reference table.
	*		If the entry is present, it will move it to the front of the reference table. If the 
	*		entry is not present, a new reference has occured and it will be added to the front of
	*		the reference table. If the reference table has reached its maximum size, the oldest
	*		entry will be deleted from the reference table.
	*/
	RPTEntry* RPTTable::get(Addr pc){
		std::list<RPTEntry*>::iterator i;
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
}
