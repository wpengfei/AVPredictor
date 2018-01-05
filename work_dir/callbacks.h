#include "pin.H"
#include "structs.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>


//Step 5: record the three memory access for replay in the second run.


void record_pattern(memAccess first, memAccess second, memAccess inter){

    /*here determine whether these three accesses are protected by locks
    *if not protected by locks (critical sections), then we record these accesses directly.
    *otherwitse, we record the lock and unlock of each access, 
    *which will be used to delay the access later on in the replay phase.
    */

    //---------------------------------

    unsigned int locked = 0;

    if(inter.lock_ev == 0){
        fprintf(replay_log, "%u[%u]%x,%x; [%u]%x,%x; [%u]%x,%x\n",
            locked,first.tid, first.inst, first.addr,
            second.tid, second.inst, second.addr,
            inter.tid, inter.inst, inter.addr);
    }
    else{//inter is protected by lock
        if(first.lock_ev == inter.lock_ev && second.lock_ev == inter.lock_ev){
            locked = 1;
            fprintf(replay_log, "%u[%u]%x,%x; [%u]%x,%x; [%u]%x,%x\n",
                locked,first.tid, first.lock_cv, first.unlock_cv,
                second.tid, second.lock_cv, second.unlock_cv,
                inter.tid, inter.lock_cv, inter.unlock_cv);   
        }
        else{
            fprintf(replay_log, "%u[%u]%x,%x; [%u]%x,%x; [%u]%x,%x\n",
                locked,first.tid, first.inst, first.addr,
                second.tid, second.inst, second.addr,
                inter.tid, inter.inst, inter.addr);

        }

    }




}


//Step 4: Check whether the found interleaving could cause an atomicity-violation
//  if it is buggy, then report immediatly,
void examine_interleaving(memAccess first, memAccess second, memAccess interleave){
    if(interleave.time > first.time && interleave.time < second.time){
        printf("\033[01;31m[examine_interleaving] Buggy interleaving. \033[0m\n\n");
        record_pattern(first, second, interleave);
        return;
    }
    // pattern 1:
    //
    //  T1     T2
    //          I
    //   A1
    //   ...
    //   A2
    //
    if(interleave.time < first.time){
        unsigned int i;
        // check whether a synchronization (e.g. barriar) pretents this pattern form an AV
        for(i=0; i<synchTable.size(); i++){
            if(synchTable[i].tid == interleave.tid 
                && synchTable[i].time > interleave.time
                && synchTable[i].time < first.time){

                if(DEBUG_TRACING){
                    printf("\033[22;36m[examine_interleaving] Pattern 1, Synchronization exists, skip. \033[0m\n\n");
                    print_synch(synchTable[i], "examine_interleaving");
                }
                return;
            }
               
        }
        printf("\033[01;31m[examine_interleaving] Pattern 1, No synch protection. Record potential buggy pattern for replay. \033[0m\n\n");
        record_pattern(first, second, interleave);
        return;
    }
    //pattern 2:
    //
    //  T1     T2
    //          
    //   A1
    //   ...
    //   A2
    //          I
    if(interleave.time > second.time){
        unsigned int i;
        // check whether a synchronization (e.g. barriar) pretents this pattern form an AV
        for(i=0; i<synchTable.size(); i++){
            if(synchTable[i].tid == interleave.tid 
                && synchTable[i].time < interleave.time
                && synchTable[i].time > second.time){

                if(DEBUG_TRACING){
                    printf("\033[22;36m[examine_interleaving] Pattern 2, Synchronization exists, skip. \033[0m\n\n");
                    print_synch(synchTable[i], "examine_interleaving");
                }
                return;
            }
                
        }
        printf("\033[01;31m[examine_interleaving] Pattern 2, No synch protection. Record potential buggy pattern for replay. \033[0m\n\n");
        record_pattern(first, second, interleave);
        return;
    }
}


//Step 3: Find a write from a remote thread that could possibly interleave the local pair
void find_interleave_write(memAccess first, memAccess second){
    
    assert(first.addr == second.addr);
    assert(first.tid == second.tid);

    unsigned int i, j;

    bool found = false;
    for(i = 0; i < threadExisted; i++){
        for(j=0; j<maTable[i].size(); j++){
            if(maTable[i][j].op == 'W' && maTable[i][j].addr == first.addr && maTable[i][j].tid != first.tid){

                if(DEBUG_TRACING){
                    printf("\033[22;36m[find_interleave_write] Find remote write to the same address.\033[0m\n");
                    print_ma(maTable[i][j],"Inter-write");
                }
                /* if first and second are protected by the same critical section, 
                and the remote access are also protected by a same lock, then it won't cause a atomicity violation */
                
                if(first.lock_cv == second.lock_cv && first.unlock_cv == second.unlock_cv 
                    && first.lock_cv != 0 && maTable[i][j].lock_ev == first.lock_ev && maTable[i][j].lock_ev != 0){
                    if(DEBUG_TRACING){
                        printf("\033[22;36m[find_interleave_write] Remote write protected by the same lock,skip.\033[0m\n");
                    }

                }
                else{
                    if(DEBUG_TRACING){
                        printf("\033[01;31m[find_interleave_write] Found Remote write.\033[0m\n");
                    }
                    examine_interleaving(first, second, maTable[i][j]);
                    found = true;
                }

            }


        }
    }

    if(DEBUG_TRACING){
        if(!found)
            printf("\033[22;36m[find_interleave_write] Not Find remote write to the same address.\033[0m\n");
    }
}

//Step 3: Find a read from a remote thread that could interleave the local pair
void find_interleave_read(memAccess first, memAccess second){
    
    assert(first.addr == second.addr);
    assert(first.tid == second.tid);

    unsigned int i, j;

    bool found = false;
    for(i = 0; i < threadExisted; i++){
        for(j=0; j<maTable[i].size(); j++){
            if(maTable[i][j].op == 'R' && maTable[i][j].addr == first.addr && maTable[i][j].tid != first.tid){
                
                if(DEBUG_TRACING){
                    printf("\033[22;36m[find_interleave_read] Find remote read to the same address.\033[0m\n");
                    print_ma(maTable[i][j],"Inter-read");
                }
                /* if first and second are protected by the same critical section, 
                and the remote access are also protected by a same lock, then it won't cause a atomicity violation */
                
                if(first.lock_cv == second.lock_cv && first.unlock_cv == second.unlock_cv 
                    && first.lock_cv != 0 && maTable[i][j].lock_ev == first.lock_ev && maTable[i][j].lock_ev != 0){
                    if(DEBUG_TRACING){
                        printf("\033[22;36m[find_interleave_read] Remote read protected by the same lock,skip.\033[0m\n");
                    }
                }
                else{
                    if(DEBUG_TRACING){
                        printf("\033[01;31m[find_interleave_read] Found Remote read.\033[0m\n");
                    }
                    examine_interleaving(first, second, maTable[i][j]);
                    found = true;
                }

            }


        }
        
    }

    if(DEBUG_TRACING){
        if(!found)
            printf("\033[22;36m[find_interleave_read] Not Find remote read to the same address.\033[0m\n");
    }

}


// Step 1: Find memory access pairs from the same thread accessing the same address
// then use the access pair to find a interleave access from other thread
void start_anlysis(){
    
    bool found = false;
    
    unsigned int i,j,k;

    for(i=0; i<threadExisted; i++){
        for(j=0; j<maTable[i].size(); j++){
            for(k=j+1; k<maTable[i].size(); k++){
                if(maTable[i][j].addr == maTable[i][k].addr){

                    if(DEBUG_TRACING){
                        printf("\033[22;36m[start_anlysis] Find access pair accessing the same address:\033[0m\n");
                        print_ma(maTable[i][j], "First");
                        print_ma(maTable[i][k], "Second");
                    }

                    
                    if(maTable[i][j].op == 'R'){
                        if(maTable[i][k].op == 'R')
                            find_interleave_write(maTable[i][j], maTable[i][k]);
                        else
                            find_interleave_write(maTable[i][j], maTable[i][k]);
                    }
                    else{
                        if(maTable[i][k].op == 'R')
                            find_interleave_write(maTable[i][j], maTable[i][k]);
                        else
                            find_interleave_read(maTable[i][j], maTable[i][k]);
                    }
                    

                    found = true;
                }
            }
        }
    }
    
    if(DEBUG_TRACING){
        if(!found)
            printf("\033[22;36m[find_same_address_accesses] Did not find same address pair. \033[0m\n");
    }
}



// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr, THREADID tid, VOID* rtnName)
{
    if((unsigned int)addr > STACK_LOWERBOUND)
        return;

    PIN_GetLock(&lock,tid+1);

    timestamp++;


    memAccess ma={'R',(ADDRINT)ip, (ADDRINT)addr, tid, timestamp, string((char*)rtnName), 0, 0, 0};

    if(DEBUG_TRACING){
        print_ma(ma, "RecordMemRead");
    }

    if (logging_start){
        maTable[tid].push_back(ma); //add new record

    }
    PIN_ReleaseLock(&lock);
}

// Print a memory write record
VOID RecordMemWrite(VOID * ip, VOID * addr, THREADID tid, VOID* rtnName)
{
    if((unsigned int)addr > STACK_LOWERBOUND)
        return;

    PIN_GetLock(&lock,tid+1);
    
    timestamp++;
 
    memAccess ma={'W',(ADDRINT)ip, (ADDRINT)addr, tid, timestamp, string((char*)rtnName), 0, 0, 0};

    if(DEBUG_TRACING){
        print_ma(ma,  "RecordMemWrite");
    }

    if (logging_start){
        maTable[tid].push_back(ma); //add new record

    }

    PIN_ReleaseLock(&lock);
}



VOID afterThreadCreate(THREADID threadid)
{

    PIN_GetLock(&lock, threadid+1);
    
    logging_start = true;

    if(DEBUG_TRACING){
        printf("\033[01;34m[afterThreadCreate] start logging\033[0m\n");
    }

    PIN_ReleaseLock(&lock);
    
}

VOID beforeThreadLock(VOID * ip, THREADID threadid,  ADDRINT lock_callsite_v, ADDRINT lock_entry_v)
{
    PIN_GetLock(&lock, threadid+1);
    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadLock] T %d Locked, time: %d, ip: 0x%x, lock_callsite_v: 0x%x, lock_entry_v: 0x%x.\033[0m\n", 
        threadid, timestamp, (ADDRESS)ip, lock_callsite_v, lock_entry_v);
    }

    criticalSection cs = {threadid, lock_callsite_v, 0, lock_entry_v, 0, timestamp, 0}; //use 0 by default when a critical section is not finished

    csTable.push_back(cs);

    PIN_ReleaseLock(&lock);
}

VOID beforeThreadUnLock(VOID * ip, THREADID threadid,  ADDRINT unlock_callsite_v, ADDRINT unlock_entry_v)
{
    PIN_GetLock(&lock, threadid+1);
    timestamp++;
    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadUnLock] T %d Unlocked, time: %d, ip: 0x%x, unlock_callsite_v: 0x%x, unlock_entry_v: 0x%x.\033[0m\n", 
        threadid, timestamp, (ADDRESS)ip, unlock_callsite_v, unlock_entry_v);
    }

    unsigned int i;
    bool matched = false;
    assert(csTable.size()>0);
    //printf("cstablesize: %d\n",csTable.size());
    if (csTable.size() == 1){
        if(csTable[i].tid == threadid && csTable[i].unlock_callsite_v == 0 && csTable[i].ft == 0){
            csTable[i].ft = timestamp;//finish the critical section
            csTable[i].unlock_callsite_v = unlock_callsite_v;
            csTable[i].unlock_entry_v = unlock_entry_v;
        }
    }
    else if (csTable.size() > 1){
        for (i = csTable.size()-1; i >= 0; i--){ //critical sections can be embedded, should be processed like a stack, from back to front
            if(csTable[i].tid == threadid && csTable[i].unlock_callsite_v == 0 && csTable[i].ft == 0){
                csTable[i].ft = timestamp;//finish the critical section
                csTable[i].unlock_callsite_v = unlock_callsite_v;
                csTable[i].unlock_entry_v = unlock_entry_v;
                matched = true;
                break;
            }
        }
        assert(matched);//should always matched.
    }
    else{
        assert(0); // should always >= 1
    }

  
    PIN_ReleaseLock(&lock);
}

VOID afterThreadBarrier(THREADID threadid)
{
    PIN_GetLock(&lock, threadid+1);

    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadBarrier] T %d Barrier, time: %d.\033[0m\n", threadid, timestamp);
    }

    synch s = {"barrier", threadid, timestamp};
    synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}

VOID afterThreadCondWait(THREADID threadid)
{
    PIN_GetLock(&lock, threadid+1);
    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadCondWait] T %d Condwait, time: %d.\033[0m\n", threadid, timestamp);
    }

    synch s = {"condwait", threadid, timestamp};
    synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}

VOID afterThreadCondTimedwait(THREADID threadid)
{
    PIN_GetLock(&lock, threadid+1);
    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadCondTimedwait] T %d CondTimewait, time: %d.\033[0m\n", threadid, timestamp);
    }

    synch s = {"condtimewait", threadid, timestamp};
    synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}

VOID afterThreadSleep(THREADID threadid)
{
    PIN_GetLock(&lock, threadid+1);
    timestamp++;
    
    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadSleep] T %d sleep, time: %d.\033[0m\n", threadid, timestamp);
    }

    synch s = {"sleep", threadid, timestamp};
    synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}

