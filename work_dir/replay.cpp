
#include "callbacks.h"


#define SLEEP_TIME 5

unsigned int ci_count = 0;

unsigned int is_locked_type = 0;

COUNT first_tid = 0;
ADDRESS first_inst = 0; //use for non-critical section situations
ADDRESS first_addr = 0;
ADDRESS first_lock = 0; // use for critical section situations
ADDRESS first_unlock = 0;

COUNT second_tid = 0;
ADDRESS second_inst = 0;
ADDRESS second_addr = 0;
ADDRESS second_lock = 0;
ADDRESS second_unlock = 0;

COUNT inter_tid = 0;
ADDRESS inter_inst = 0;
ADDRESS inter_addr = 0;
ADDRESS inter_lock = 0;
ADDRESS inter_unlock = 0;

bool first_ready = false;
bool inter_ready = false;
bool second_ready = false;

VOID BeforeMemAccess(VOID * ip, VOID * addr, THREADID tid, VOID* rtnName){
    //delay the inter to between first and second
    //pattern 2:
    //
    //   T1           T2
    //                         
    //   First
    //                    <--- delay Inter to after First
    //               Inter
    //
    //          <--- delay Second to after Inter
    //   Second
    //   

    if(is_locked_type) //only unlocked type is handled in this function.
        return;

    if(first_ready == false && inter_ready == false && second_ready == false){
        if((ADDRINT)ip == first_inst && (ADDRINT)addr == first_addr && (COUNT)tid == first_tid){
            first_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[1][execute First] inst:%x, addr:%x, tid:%u \033[0m\n",(ADDRINT)ip,(ADDRINT)addr,(COUNT)tid);
            }
            return;
        }
        else if((ADDRINT)ip == inter_inst && (ADDRINT)addr == inter_addr && (COUNT)tid == inter_tid){
            while(first_ready == false){ 
                if(DEBUG_REPLAY){              
                    printf("\033[01;33m[2][delay Inter] inst:%x, addr:%x, tid:%u \033[0m\n", (ADDRINT)ip,(ADDRINT)addr,(COUNT)tid);
                }
                PIN_Sleep(SLEEP_TIME);
            }
            inter_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[3][execute Inter after delay] inst:%x, addr:%x, tid:%u \033[0m\n", (ADDRINT)ip,(ADDRINT)addr,(COUNT)tid);
            }
            return;
        }

    }
    else if(first_ready == true && inter_ready == false && second_ready == false){ 
        if((ADDRINT)ip == inter_inst && (ADDRINT)addr == inter_addr && (COUNT)tid == inter_tid){
            inter_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[4][execute Inter] inst:%x, addr:%x, tid:%u \033[0m\n", (ADDRINT)ip,(ADDRINT)addr,(COUNT)tid);
            }
            return;
        }
        else if((ADDRINT)ip == second_inst && (ADDRINT)addr == second_addr && (COUNT)tid == second_tid){
            while(inter_ready == false){  
                if(DEBUG_REPLAY){              
                    printf("\033[01;33m[5][delay Second] inst:%x, addr:%x, tid:%u \033[0m\n", (ADDRINT)ip,(ADDRINT)addr,(COUNT)tid);
                }
                PIN_Sleep(SLEEP_TIME);
            }
            second_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[6][execute Second after delay] inst:%x, addr:%x, tid:%u \033[0m\n", (ADDRINT)ip,(ADDRINT)addr,(COUNT)tid);
            }
            return;
        }

    }
    else if(first_ready == true && inter_ready == true && second_ready == false){ 
        if((ADDRINT)ip == second_inst && (ADDRINT)addr == second_addr && (COUNT)tid == second_tid){
            second_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[5][execute Second] inst:%x, addr:%x, tid:%u \033[0m\n",(ADDRINT)ip,(ADDRINT)addr,(COUNT)tid);
            }
            return;
        }
    }
}



VOID beforeThreadLock_replay(THREADID tid, VOID * ip, ADDRINT callsite_v)
{

    if(!is_locked_type) //only locked type is handled in this function.
        return;
    //PIN_GetLock(&lock, tid+1);
    
    if(first_ready == false && inter_ready == false && second_ready == false){
        if((COUNT)tid == inter_tid && (ADDRESS)callsite_v == inter_lock){
            while(first_ready == false){  
                if(DEBUG_REPLAY){               
                    printf("\033[01;33m[beforeThreadLock][delay Inter] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
                }
                PIN_Sleep(SLEEP_TIME);
            }
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadLock][enter Inter after delay] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            return;
        }
        else if((COUNT)tid == first_tid && (ADDRESS)callsite_v == first_lock){
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadLock][enter First] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == false && second_ready == false){ 
        if((COUNT)tid == second_tid && (ADDRESS)callsite_v == second_lock){
            while(inter_ready == false){               
                if(DEBUG_REPLAY){
                    printf("\033[01;33m[beforeThreadLock][delay Second] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
                }
                PIN_Sleep(SLEEP_TIME);
            }
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadLock][enter Second after delay] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            return;
        }
        else if((COUNT)tid == inter_tid && (ADDRESS)callsite_v == inter_lock){
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadLock][enter Inter] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            return;
        }
        

    }
    else if(first_ready == true && inter_ready == true && second_ready == false){
        if((COUNT)tid == second_tid && (ADDRESS)callsite_v == second_lock){
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadLock][enter Second] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            return;
        }
        
    }

    //PIN_ReleaseLock(&lock);
}
VOID beforeThreadUnLock_replay(THREADID tid, VOID * ip, ADDRINT callsite_v)
{
    if(!is_locked_type) //only locked type is handled in this function.
        return;

    //PIN_GetLock(&lock, tid+1);

    if(first_ready == false && inter_ready == false && second_ready == false){
        if((COUNT)tid == first_tid && (ADDRESS)callsite_v == first_unlock){
            first_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadUnLock][executed First] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == false && second_ready == false){ 
        if((COUNT)tid == inter_tid && (ADDRESS)callsite_v == inter_unlock){
            inter_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadUnLock][executed Inter] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == true && second_ready == false){ 
        if((COUNT)tid == second_tid && (ADDRESS)callsite_v == second_unlock){
            second_ready = true;
            if(DEBUG_REPLAY){
                printf("\033[01;34m[beforeThreadUnLock][executed Second] callsite_v:%x, tid:%u \033[0m\n", (ADDRESS)callsite_v,(COUNT)tid);
            }
            //PIN_GetLock(&lock, tid+1);
            return;
        }
        
    }
  
    //PIN_ReleaseLock(&lock);
}








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

    if(INS_IsMemoryRead(ins)){
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)BeforeMemAccess, IARG_INST_PTR, IARG_MEMORYREAD_EA, 
          IARG_THREAD_ID, IARG_PTR, (VOID*)RTN_Name(rtn).c_str(), IARG_END);
    }

    if(INS_IsMemoryWrite(ins)){
        INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)BeforeMemAccess,IARG_INST_PTR, IARG_MEMORYWRITE_EA, 
          IARG_THREAD_ID, IARG_PTR, (VOID*)RTN_Name(rtn).c_str(), IARG_END);
    }

}

// Pin calls this function every time a new rtn is executed
VOID Routine(RTN rtn, VOID *v)
{
    string rtn_name = RTN_Name(rtn);

    PIN_GetLock(&lock,PIN_ThreadId()+1);

    if(rtn_name=="__pthread_mutex_lock"||rtn_name=="pthread_mutex_lock"){ //,IARG_FUNCARG_CALLSITE_REFERENCE,IARG_FUNCARG_CALLSITE_VALUE
        RTN_Open(rtn);
        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadLock_replay, IARG_THREAD_ID, IARG_INST_PTR, IARG_FUNCARG_CALLSITE_VALUE, 0,IARG_END);
        RTN_Close(rtn);
    } else if(rtn_name=="__pthread_mutex_unlock"||rtn_name=="pthread_mutex_unlock"){
        RTN_Open(rtn);
        RTN_InsertCall(rtn,IPOINT_BEFORE,(AFUNPTR)beforeThreadUnLock_replay, IARG_THREAD_ID, IARG_INST_PTR, IARG_FUNCARG_CALLSITE_VALUE, 0,IARG_END);
        RTN_Close(rtn);
    }

    PIN_ReleaseLock(&lock);
  
}





VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v)
{

}

// This routine is executed every time a thread is destroyed.
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v)
{

}

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    // Write to a file since cout and cerr maybe closed by the application

    fclose(replay_log);
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{

    cerr << "This Pintool counts the number of times a routine is executed" << endl;
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

    replay_log = fopen("replay/file_result.log", "r");

    while (1){
        if((fscanf(replay_log,"%u[%u]%x,%x; [%u]%x,%x; [%u]%x,%x\n",
                &is_locked_type, &first_tid, &first_inst, &first_addr, 
                &second_tid, &second_inst, &second_addr, 
                &inter_tid, &inter_inst, &inter_addr))!=EOF){


            printf("\033[22;36m[load CI from file]:%u[%u]%x,%x; [%u]%x,%x; [%u]%x,%x \033[0m\n",
                is_locked_type, first_tid, first_inst, first_addr, 
                second_tid, second_inst, second_addr, 
                inter_tid, inter_inst, inter_addr);


        }
        else 
            break;

    }

    //fscanf(replay_log,"%u[%u]%x,%x; [%u]%x,%x; [%u]%x,%x\n",
        //&is_locked_type, &first_tid, &first_inst, &first_addr, 
                //&second_tid, &second_inst, &second_addr, 
                //&inter_tid, &inter_inst, &inter_addr);

    printf("========================================Replay\n");
    
    /* convert to critical section situation format */
    if(is_locked_type){
        first_lock = first_inst;
        first_unlock = first_addr;
        second_lock = second_inst;
        second_unlock = second_addr;
        inter_lock = inter_inst;
        inter_unlock = inter_addr;

    }

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