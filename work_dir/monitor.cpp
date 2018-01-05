#include "pin.H"
#include "structs.h"
#include <iostream>
#include <fstream>



// Print a memory read record
VOID RecordMemRead(VOID * ip, VOID * addr, THREADID tid, VOID* rtnName)
{
    if((unsigned int)addr > STACK_LOWERBOUND)
        return;

    PIN_GetLock(&lock,tid+1);

    timestamp++;


    //memAccess ma={'R',(ADDRINT)ip, (ADDRINT)addr, tid, timestamp, string((char*)rtnName), 0, 0, 0};

 	if (logging_start){

	    fprintf(file_mem_access, "tid:%d,op:%c,time:%d,ip:0x%x,addr:0x%x,rtn:%s\n", tid, 'R', timestamp, (unsigned int)ip, (unsigned int)addr, (char*)rtnName);
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
 
    if (logging_start){
    	fprintf(file_mem_access, "tid:%d,op:%c,time:%d,ip:0x%x,addr:0x%x,rtn:%s\n", tid, 'W', timestamp, (unsigned int)ip, (unsigned int)addr, (char*)rtnName);
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

VOID beforeThreadLock(VOID * ip, THREADID tid,  ADDRINT lock_callsite_v, ADDRINT lock_entry_v)
{
    PIN_GetLock(&lock, tid+1);
    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadLock] T %d Locked, time: %d, ip: 0x%x, lock_callsite_v: 0x%x, lock_entry_v: 0x%x.\033[0m\n", 
        tid, timestamp, (ADDRESS)ip, lock_callsite_v, lock_entry_v);
    }

    fprintf(file_lock, "tid:%d,op:%c,time:%d,callsite_v:0x%x,entry_v:0x%x\n", 
    		tid, 'L', timestamp, (unsigned int)lock_callsite_v, (unsigned int)lock_entry_v);



    //criticalSection cs = {threadid, lock_callsite_v, 0, lock_entry_v, 0, timestamp, 0}; //use 0 by default when a critical section is not finished

    //csTable.push_back(cs);

    PIN_ReleaseLock(&lock);
}

VOID beforeThreadUnLock(VOID * ip, THREADID tid,  ADDRINT unlock_callsite_v, ADDRINT unlock_entry_v)
{
    PIN_GetLock(&lock, tid+1);
    timestamp++;
    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadUnLock] T %d Unlocked, time: %d, ip: 0x%x, unlock_callsite_v: 0x%x, unlock_entry_v: 0x%x.\033[0m\n", 
        tid, timestamp, (ADDRESS)ip, unlock_callsite_v, unlock_entry_v);
    }

    fprintf(file_lock, "tid:%d,op:%c,time:%d,callsite_v:0x%x,entry_v:0x%x\n", 
    		tid, 'U', timestamp, (unsigned int)unlock_callsite_v, (unsigned int)unlock_entry_v);

/*
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
*/
  
    PIN_ReleaseLock(&lock);
}

VOID afterThreadBarrier(THREADID tid)
{
    PIN_GetLock(&lock, tid+1);

    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadBarrier] T %d Barrier, time: %d.\033[0m\n", tid, timestamp);
    }

    fprintf(file_sync, "tid:%d,type:%s,time:%d\n", tid, "barrier", timestamp);

    //synch s = {"barrier", threadid, timestamp};
    //synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}

VOID afterThreadCondWait(THREADID tid)
{
    PIN_GetLock(&lock, tid+1);
    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadCondWait] T %d Condwait, time: %d.\033[0m\n", tid, timestamp);
    }

    fprintf(file_sync, "tid:%d,type:%s,time:%d\n", tid, "condwait", timestamp);

    //synch s = {"condwait", threadid, timestamp};
    //synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}

VOID afterThreadCondTimedwait(THREADID tid)
{
    PIN_GetLock(&lock, tid+1);
    timestamp++;

    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadCondTimedwait] T %d CondTimewait, time: %d.\033[0m\n", tid, timestamp);
    }

    fprintf(file_sync, "tid:%d,type:%s,time:%d\n", tid, "condtimewait", tid);
    //synch s = {"condtimewait", threadid, timestamp};
    //synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}

VOID afterThreadSleep(THREADID tid)
{
    PIN_GetLock(&lock, tid+1);
    timestamp++;
    
    if(DEBUG_TRACING){
        printf("\033[01;33m[ThreadSleep] T %d sleep, time: %d.\033[0m\n", tid, timestamp);
    }

    fprintf(file_sync, "tid:%d,type:%s,time:%d\n", tid, "sleep", tid);

    //synch s = {"sleep", threadid, timestamp};
    //synchTable.push_back(s);

    PIN_ReleaseLock(&lock);
}


//--------------------------------------------------------------------------------------------------------------
// Is called for every instruction and instruments reads and writes
VOID Instruction(INS ins, VOID *v)
{
    if(INS_Address(ins) > USER_UPPERBOUND) 
        return; 
    if(INS_Address(ins) < USER_LOWERBOUND) 
        return;
    if(INS_IsBranchOrCall(ins)) 
        return;
    
    RTN rtn = INS_Rtn(ins);  
    if(isLibRtnName(RTN_Name(rtn))) 
        return;

    if (INS_IsMemoryRead(ins)){
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemRead, IARG_INST_PTR, IARG_MEMORYREAD_EA, 
          IARG_THREAD_ID, IARG_PTR, (VOID*)RTN_Name(rtn).c_str(), IARG_END);
    }

    
    if(INS_IsMemoryWrite(ins)){
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)RecordMemWrite,IARG_INST_PTR,IARG_MEMORYWRITE_EA, 
          IARG_THREAD_ID, IARG_PTR, (VOID*)RTN_Name(rtn).c_str(), IARG_END);
    }

}


// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v)
{
    
    string rtn_name = RTN_Name(rtn);

    PIN_GetLock(&lock,PIN_ThreadId()+1);

    if(rtn_name=="pthread_create"||rtn_name=="pthread_create"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCreate,IARG_THREAD_ID, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadCreate, IARG_THREAD_ID, IARG_END);
        RTN_Close(rtn);
    } else if(rtn_name=="__pthread_join"||rtn_name=="pthread_join"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadJoin, IARG_THREAD_ID, IARG_END);
        //RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadJoin, IARG_THREAD_ID, IARG_END); //IARG_UINT32
        RTN_Close(rtn); 
    } else if(rtn_name=="__pthread_mutex_lock"||rtn_name=="pthread_mutex_lock"){ 
        RTN_Open(rtn);
        //IARG_FUNCARG_CALLSITE_REFERENCE, IARG_FUNCARG_CALLSITE_VALUE indicates different critical sections
        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadLock, IARG_ADDRINT, 
            IARG_INST_PTR, IARG_THREAD_ID, IARG_FUNCARG_CALLSITE_VALUE, 0, IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);
        RTN_Close(rtn);
    } else if(rtn_name=="__pthread_mutex_unlock"||rtn_name=="pthread_mutex_unlock"){
        RTN_Open(rtn);
        //IARG_FUNCARG_ENTRYPOINT_REFERENCE, IARG_FUNCARG_ENTRYPOINT_VALUE indicates whether use the same lock
        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadUnLock, IARG_ADDRINT, 
            IARG_INST_PTR, IARG_THREAD_ID, IARG_FUNCARG_CALLSITE_VALUE, 0, IARG_FUNCARG_ENTRYPOINT_VALUE, 0,IARG_END);
        RTN_Close(rtn);
    } else if(rtn_name=="__pthread_barrier_wait"||rtn_name=="pthread_barrier_wait"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCondWait, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadBarrier, IARG_THREAD_ID, IARG_END);
        RTN_Close(rtn);
    } else if(rtn_name=="__pthread_cond_wait"||rtn_name=="pthread_cond_wait"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCondWait, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadCondWait, IARG_THREAD_ID, IARG_END);
        RTN_Close(rtn);
    } else if(rtn_name=="__pthread_cond_timedwait"||rtn_name=="pthread_cond_timedwait"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCondTimedwait, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadCondTimedwait, IARG_THREAD_ID, IARG_END);
        RTN_Close(rtn);
    } else if(rtn_name=="sleep"||rtn_name=="usleep"||rtn_name=="__sleep"||rtn_name=="__usleep"){
    //we consider thread sleep as it likes thread wait 
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadSleep, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadSleep, IARG_THREAD_ID, IARG_END);
        RTN_Close(rtn);
    }

    PIN_ReleaseLock(&lock);
    
}


VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{

   
    PIN_GetLock(&lock, threadid+1);

    threadNum++;
    threadExisted++;
    if (threadNum == 1){
        if(DEBUG_TRACING)
            printf("\033[01;34m[ThreadStart] Main Thread T 0 created, total number is %d\033[0m\n", threadNum);
    }
    else{
        if(DEBUG_TRACING)
            printf("\033[01;34m[ThreadStart] Thread %d created, total number is %d\033[0m\n", threadid, threadNum);
    }
     
    PIN_ReleaseLock(&lock);
    
}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v)
{
    
    PIN_GetLock(&lock, threadid+1);
    threadNum--;
    if (threadNum > 1)
    {
        if(DEBUG_TRACING)
            printf("\033[01;34m[ThreadFini] Thread %d joined, total number is %d\033[0m\n", threadid, threadNum);
    }
    else if (threadNum == 1)
    {
        if(DEBUG_TRACING)
            printf("\033[01;34m[ThreadFini] Thread %d joined, total number is %d, stop logging.\033[0m\n", threadid, threadNum);
        logging_start = false;
    }
    else{
        if(DEBUG_TRACING)
            printf("\033[01;34m[ThreadFini] Main Thread joined, total number is %d\033[0m\n", threadNum);
    }
    PIN_ReleaseLock(&lock);
    
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
	//unsigned int i = 0;
    //unsigned int j = 0;



    //fprintf(file_mem_access, "#eof\n");
    fclose(file_lock);
    fclose(file_sync);

}



/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR( "This Pintool prints a trace of memory addresses\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");

    cerr << "This Pintool counts the number of times a routine is executed" << endl;
    cerr << "and the number of instructions executed in a routine" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
/*   argc, argv are the entire command line: pin -t <toolname> -- ...    */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    // Initialize symbol table code, needed for rtn instrumentation
    PIN_InitSymbols();

    file_mem_access = fopen("file_mem_access.log", "w");
	file_lock = fopen("file_lock.log", "w");
	file_sync = fopen("file_sync.log", "w");

    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    //OutFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);



    // Register Routine to be called to instrument rtn
    RTN_AddInstrumentFunction(Routine, 0);

    // Register Analysis routines to be called when a thread begins/ends
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
