#include "pin.H"
#include <iostream>
#include <fstream>
#include "structs.h"



/*!
 * Called when a buffer fills up, or the thread exits, so we can process it or pass it off
 * as we see fit.
 * @param[in] id		buffer handle
 * @param[in] tid		id of owning thread
 * @param[in] ctxt		application context
 * @param[in] buf		actual pointer to buffer
 * @param[in] numElements	number of records
 * @param[in] v			callback value
 * @return  A pointer to the buffer to resume filling.
 */

VOID * BufferFull(BUFFER_ID id, THREADID tid, const CONTEXT *ctxt, VOID *buf,
                  UINT64 numElements, VOID *v)
{
    PIN_GetLock(&lock, tid+1);

    fwrite(buf, sizeof(struct MEMREF), numElements, file_mem);
    
    countMemRef = countMemRef + numElements;

    #ifdef DEBUG_MONITOR 

        
        struct MEMREF * reference = (struct MEMREF *)buf;
        for(UINT64 i=0; i<numElements; i++, reference++)
        {
            //fprintf(file_mem0, "%x,%x,%llx,%x,%x\n", reference->pc, reference->addr, reference->ts, reference->tid, reference->read);       
        
            printf("\033[01;34m[BufferFull]pc:0x%x,addr:0x%x,time:0x%llx,tid:0x%x,read:%x\n",
                reference->pc,reference->addr, reference->ts, reference->tid, reference->read );
        
        }
    #endif

    PIN_ReleaseLock(&lock);

    return buf;
}

VOID Instruction(INS ins, VOID *v)
{
    
    if (!logEnable)
        return;
    
    if(INS_IsBranchOrCall(ins)) 
        return;
    if(INS_IsStackRead(ins))
        return;
    if(INS_IsStackWrite(ins))
        return;  
    if(INS_IsPrefetch(ins))
        return;
    UINT32 addr = INS_Address(ins);
    if(addr > USER_UPPERBOUND or addr < USER_LOWERBOUND) 
        return; 

    //RTN rtn = INS_Rtn(ins);  
    //if(isLibRtnName(RTN_Name(rtn))) 
        //return;


    if (INS_IsMemoryRead(ins)){
        PIN_GetLock(&lock,PIN_ThreadId()+1);
           
        INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
                                 IARG_INST_PTR, offsetof(struct MEMREF, pc),
                                 IARG_MEMORYREAD_EA, offsetof(struct MEMREF, addr),
                                 IARG_TSC, offsetof(struct MEMREF, ts),
                                 IARG_THREAD_ID, offsetof(struct MEMREF, tid),
                                 IARG_UINT32, 0, offsetof(struct MEMREF, read),
                                 IARG_END);
        PIN_ReleaseLock(&lock);
    }
    
    else if(INS_IsMemoryWrite(ins)){
        
        PIN_GetLock(&lock,PIN_ThreadId()+1);
        INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
                                 IARG_INST_PTR, offsetof(struct MEMREF, pc),
                                 IARG_MEMORYWRITE_EA, offsetof(struct MEMREF, addr),
                                 IARG_TSC, offsetof(struct MEMREF, ts),
                                 IARG_THREAD_ID, offsetof(struct MEMREF, tid),
                                 IARG_UINT32, 1, offsetof(struct MEMREF, read),
                                 IARG_END);
        PIN_ReleaseLock(&lock);   
    }

}

VOID Trace(TRACE trace, VOID *v)
{
    if (!logEnable)
        return;
    for(BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl=BBL_Next(bbl))
    {
        for(INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins=INS_Next(ins))
        {
            if (!INS_IsStandardMemop(ins) && !INS_HasMemoryVector(ins))
            {
                // We don't know how to treat these instructions
                continue;
            }
	        if(INS_Address(ins) > USER_UPPERBOUND) 
                continue; 
            if(INS_Address(ins) < USER_LOWERBOUND) 
                continue;
            if(INS_IsBranchOrCall(ins))
                continue;
            if(INS_IsStackRead(ins))
                continue;
            if(INS_IsStackWrite(ins))
                continue; 
            if(INS_IsPrefetch(ins))
                continue;
            RTN rtn = INS_Rtn(ins);  
            if(isLibRtnName(RTN_Name(rtn))) 
                continue;

            if (INS_IsMemoryRead(ins)){
                PIN_GetLock(&lock,PIN_ThreadId()+1);
                   
                   INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
                                         IARG_INST_PTR, offsetof(struct MEMREF, pc),
                                         IARG_MEMORYREAD_EA, offsetof(struct MEMREF, addr),
					                     IARG_TSC, offsetof(struct MEMREF, ts),
                                         IARG_THREAD_ID, offsetof(struct MEMREF, tid),
                                         IARG_UINT32, 0, offsetof(struct MEMREF, read),
                                         IARG_END);
                PIN_ReleaseLock(&lock);
            }
            
            else if(INS_IsMemoryWrite(ins)){
                
                PIN_GetLock(&lock,PIN_ThreadId()+1);
                INS_InsertFillBuffer(ins, IPOINT_BEFORE, bufId,
                                         IARG_INST_PTR, offsetof(struct MEMREF, pc),
                                         IARG_MEMORYWRITE_EA, offsetof(struct MEMREF, addr),
					                     IARG_TSC, offsetof(struct MEMREF, ts),
                                         IARG_THREAD_ID, offsetof(struct MEMREF, tid),
                                         IARG_UINT32, 1, offsetof(struct MEMREF, read),
                                         IARG_END);
                PIN_ReleaseLock(&lock);   
            }

		
        }
    }
}


//VOID afterThreadCreate(THREADID tid, UINT64 ts){}

VOID beforeThreadLock(THREADID tid, UINT64 ts, UINT32 lock_callsite_v, UINT32 lock_entry_v)
{
    PIN_GetLock(&lock, tid+1);

    #ifdef DEBUG_MONITOR
        printf("\033[01;33m[Thread--Lock] Tid: %x, time: 0x%llx, lock_callsite_v: 0x%x, lock_entry_v: 0x%x.\033[0m\n", 
        tid, ts, lock_callsite_v, lock_entry_v);
    #endif

    if (countLockRef < LOCKBUFSIZE){
        lockBuf[countLockRef].tid = tid;
        lockBuf[countLockRef].ts = ts;
        lockBuf[countLockRef].callv = lock_callsite_v;
        lockBuf[countLockRef].entryv = lock_entry_v;
        lockBuf[countLockRef].lock = 1; //lock
        countLockRef++;
    }
    else{
        #ifdef DEBUG_MONITOR
            printf("\032[01;33m[Thread--Lock] buffer is full!\033[0m\n");
        #endif
    }

    //fprintf(file_lock, "tid:%d,op:%c,time:%d,callsite_v:0x%x,entry_v:0x%x\n", tid, 'L', timestamp, (unsigned int)lock_callsite_v, (unsigned int)lock_entry_v);


    PIN_ReleaseLock(&lock);
}


VOID beforeThreadUnLock( THREADID tid, UINT64 ts,  UINT32 unlock_callsite_v, UINT32 unlock_entry_v)
{
    PIN_GetLock(&lock, tid+1);

    
    #ifdef DEBUG_MONITOR
        printf("\033[01;33m[ThreadUnLock] Tid: %x, time: 0x%llx, unlock_callsite_v: 0x%x, unlock_entry_v: 0x%x.\033[0m\n", 
        tid, ts, unlock_callsite_v, unlock_entry_v);
    #endif

    if (countLockRef < LOCKBUFSIZE){
        lockBuf[countLockRef].tid = tid;
        lockBuf[countLockRef].ts = ts;
        lockBuf[countLockRef].callv = unlock_callsite_v;
        lockBuf[countLockRef].entryv = unlock_entry_v;
        lockBuf[countLockRef].lock = 0; //unlock
        countLockRef++;
    }
    else{
        #ifdef DEBUG_MONITOR
            printf("\032[01;33m[ThreadUnLock] buffer is full!\033[0m\n");
        #endif
    }
    //fprintf(file_lock, "tid:%d,op:%c,time:%d,callsite_v:0x%x,entry_v:0x%x\n", tid, 'U', timestamp, unlock_callsite_v, unlock_entry_v);
  
    PIN_ReleaseLock(&lock);
}

VOID afterThreadBarrier(THREADID tid, UINT64 ts)
{
    PIN_GetLock(&lock, tid+1);

    #ifdef DEBUG_MONITOR
        printf("\033[01;33m[ThreadBarrier] T %d Barrier, time: 0x%llx.\033[0m\n", tid, ts);
    #endif

    if (countLockRef < SYNCBUFSIZE){
        syncBuf[countSyncRef].tid = tid;
        syncBuf[countSyncRef].ts = ts;
        syncBuf[countSyncRef].type = 1;
        countSyncRef++;
    }
    else{
        #ifdef DEBUG_MONITOR
            printf("\032[01;33m[ThreadBarrier] buffer is full!\033[0m\n");
        #endif
    }
    //fprintf(file_sync, "tid:%d,time:%d,type:%c\n", tid, timestamp, 'b');

    PIN_ReleaseLock(&lock);
}

VOID afterThreadCondWait(THREADID tid, UINT64 ts)
{
    PIN_GetLock(&lock, tid+1);

    #ifdef DEBUG_MONITOR
        printf("\033[01;33m[ThreadCondWait] T %d Condwait, time: 0x%llx.\033[0m\n", tid, ts);
    #endif

    if (countLockRef < SYNCBUFSIZE){
        syncBuf[countSyncRef].tid = tid;
        syncBuf[countSyncRef].ts = ts;
        syncBuf[countSyncRef].type = 2;
        countSyncRef++;
    }
    else{
        #ifdef DEBUG_MONITOR
            printf("\032[01;33m[ThreadCondWait] buffer is full!\033[0m\n");
        #endif
    }
    //fprintf(file_sync, "tid:%d,time:%d,type:%c\n", tid, timestamp, 'w');
 
    PIN_ReleaseLock(&lock);
}

VOID afterThreadCondTimedwait(THREADID tid, UINT64 ts)
{
    PIN_GetLock(&lock, tid+1);

    #ifdef DEBUG_MONITOR
        printf("\033[01;33m[ThreadCondTimedwait] T %d CondTimewait, time: 0x%llx.\033[0m\n", tid, ts);
    #endif

    if (countLockRef < SYNCBUFSIZE){
        syncBuf[countSyncRef].tid = tid;
        syncBuf[countSyncRef].ts = ts;
        syncBuf[countSyncRef].type = 3;
        countSyncRef++;
    }
    else{
        #ifdef DEBUG_MONITOR
            printf("\032[01;33m[ThreadCondTimedwait] buffer is full!\033[0m\n");
        #endif
    }
    //fprintf(file_sync, "tid:%d,time:%d,type:%c\n", tid, tid, 't');
    
    PIN_ReleaseLock(&lock);
}

VOID afterThreadSleep(THREADID tid, UINT64 ts)
{
    PIN_GetLock(&lock, tid+1);

    #ifdef DEBUG_MONITOR
        printf("\033[01;33m[ThreadSleep] T %d sleep, time: 0x%llx.\033[0m\n", tid, ts);
    #endif
    
    if (countLockRef < SYNCBUFSIZE){
        syncBuf[countSyncRef].tid = tid;
        syncBuf[countSyncRef].ts = ts;
        syncBuf[countSyncRef].type = 4;
        countSyncRef++;
    }
    else{
        #ifdef DEBUG_MONITOR
            printf("\032[01;33m[ThreadSleep] buffer is full!\033[0m\n");
        #endif
    }
    //fprintf(file_sync, "tid:%d,time:%d,type:%c\n", tid, tid, 's');
    
    PIN_ReleaseLock(&lock);
}




// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v)
{
    string rtn_name = RTN_Name(rtn);
    /*
    
    if(isLibRtnName(rtn_name)) 
        return;
    
    if(rtn_name=="__pthread_create"||rtn_name=="pthread_create"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCreate,IARG_THREAD_ID, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadCreate, IARG_THREAD_ID, IARG_TSC,IARG_END);
        RTN_Close(rtn);
    } 
    else if(rtn_name=="__pthread_join"||rtn_name=="pthread_join"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadJoin, IARG_THREAD_ID, IARG_END);
        //RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadJoin, IARG_THREAD_ID, IARG_END); //IARG_UINT32
        RTN_Close(rtn); 
    }*/ 
    if(rtn_name=="__pthread_mutex_lock"||rtn_name=="pthread_mutex_lock"){ 
        RTN_Open(rtn);
        //IARG_FUNCARG_CALLSITE_REFERENCE, IARG_FUNCARG_CALLSITE_VALUE indicates different critical sections
        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadLock,  
            IARG_THREAD_ID,  IARG_TSC, IARG_FUNCARG_CALLSITE_VALUE, 0, IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END);
        RTN_Close(rtn);
    } 
    else if(rtn_name=="__pthread_mutex_unlock"||rtn_name=="pthread_mutex_unlock"){
        RTN_Open(rtn);
        //IARG_ADDRINT, IARG_INST_PTR,
        //IARG_FUNCARG_ENTRYPOINT_REFERENCE, IARG_FUNCARG_ENTRYPOINT_VALUE indicates whether use the same lock
        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadUnLock, 
            IARG_THREAD_ID, IARG_TSC, IARG_FUNCARG_CALLSITE_VALUE, 0, IARG_FUNCARG_ENTRYPOINT_VALUE, 0, IARG_END); // 0 indicates the first arg
        RTN_Close(rtn);
    }
    else if(rtn_name=="__pthread_barrier_wait"||rtn_name=="pthread_barrier_wait"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCondWait, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadBarrier, IARG_THREAD_ID, IARG_TSC, IARG_END);
        RTN_Close(rtn);
    } 
    else if(rtn_name=="__pthread_cond_wait"||rtn_name=="pthread_cond_wait"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCondWait, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadCondWait, IARG_THREAD_ID, IARG_TSC, IARG_END);
        RTN_Close(rtn);
    } 
    else if(rtn_name=="__pthread_cond_timedwait"||rtn_name=="pthread_cond_timedwait"){
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadCondTimedwait, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadCondTimedwait, IARG_THREAD_ID, IARG_TSC, IARG_END);
        RTN_Close(rtn);
    } 
    else if(rtn_name=="sleep"||rtn_name=="usleep"||rtn_name=="__sleep"||rtn_name=="__usleep"){
    //we consider thread sleep as it likes thread wait 
        RTN_Open(rtn);
        //RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadSleep, IARG_END);
        RTN_InsertCall(rtn,IPOINT_AFTER,(AFUNPTR)afterThreadSleep, IARG_THREAD_ID, IARG_TSC, IARG_END);
        RTN_Close(rtn);
    }
    
    
}


VOID ThreadStart(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v)
{

    PIN_GetLock(&lock, tid+1);
    
    logEnable = true;
    

    #ifdef DEBUG_MONITOR
        printf("\033[01;32m[ThreadStart] Thread %d created, total number is %d\033[0m\n", tid, threadNum);
        if (threadNum == 2)
            printf("\033[01;32m[ThreadStart] start logging\033[0m\n");
    #endif

    threadNum++;

    PIN_ReleaseLock(&lock);

}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID tid, const CONTEXT *ctxt, INT32 code, VOID *v)
{
    
    PIN_GetLock(&lock, tid+1);
    
    threadNum--;

    if (threadNum == 1){
        logEnable = false;
    }

    #ifdef DEBUG_MONITOR
        if (threadNum > 1)
                printf("\033[01;32m[ThreadFini] Thread %d joined, total number is %d\033[0m\n", tid, threadNum);
        else if (threadNum == 1)
        {
                printf("\033[01;32m[ThreadFini] Thread %d joined, total number is %d\033[0m\n", tid, threadNum);
                printf("\033[01;32m[ThreadFini] stop logging.\033[0m\n");
        }
        else{
                printf("\033[01;32m[ThreadFini] Main Thread joined, total number is %d\033[0m\n", tid);
        }
    #endif

    
    PIN_ReleaseLock(&lock);
    
}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    //printf("size of MEMREF = %d\n", sizeof(MEMREF));
    printf("\033[22;36mcountMemRef = %d \ncountLockRef = %d \ncountSyncRef = %d \033[0m\n",countMemRef,countLockRef,countSyncRef);

    fwrite(lockBuf, sizeof(struct LOCKREF), countLockRef, file_lock);
    fwrite(syncBuf, sizeof(struct SYNCREF), countSyncRef, file_sync);

    fclose(file_mem);
    //fclose(file_mem0);
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

    cerr << "This Pintool monitors the program traces." << endl;
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

    // Initialize the pin lock
    PIN_InitLock(&lock);

    // Initialize symbol table code, needed for rtn instrumentation
    PIN_InitSymbols();


    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    // open log file
    file_mem = fopen("work_dir/trace_mem.log", "wb");
    //file_mem0 = fopen("work_dir/trace_mem0.log", "w");
    file_lock = fopen("work_dir/trace_lock.log", "wb");
    file_sync = fopen("work_dir/trace_sync.log", "wb");

    bufId = PIN_DefineTraceBuffer(sizeof(struct MEMREF), NUM_BUF_PAGES, BufferFull, 0);

                                  
    if(bufId == BUFFER_ID_INVALID)
    {
        cerr << "Error: could not allocate initial buffer" << endl;
        return 1;
    }

    // add an instrumentation function
    //TRACE_AddInstrumentFunction(Trace, 0);

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
