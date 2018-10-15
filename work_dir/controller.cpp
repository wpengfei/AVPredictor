#include "pin.H"
#include "structs.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <sstream>



int load_next_ci(){

    first_ready = false;
    inter_ready = false;
    second_ready = false;

    if(cur_ci + 1 ==  max_size)
        return 0;

    if(cur_ci >= ci_count)
        return 0;

    

    printf("========================Load CI %d ==========================\n", cur_ci);
    
    if (type[cur_ci]){ // locked type
        is_locked_type = type[cur_ci];
        first_tid = first_id[cur_ci];
        first_lock = first_arg1[cur_ci];
        first_unlock = first_arg2[cur_ci];
        second_tid = second_id[cur_ci];
        second_lock = second_arg1[cur_ci];
        second_unlock = second_arg2[cur_ci];
        inter_tid = inter_id[cur_ci];
        inter_lock = inter_arg1[cur_ci];
        inter_unlock = inter_arg2[cur_ci];
    }
    else{
        is_locked_type = type[cur_ci];
        first_tid = first_id[cur_ci];
        first_inst = first_arg1[cur_ci];
        first_addr = first_arg2[cur_ci];
        second_tid = second_id[cur_ci];
        second_inst = second_arg1[cur_ci];
        second_addr = second_arg2[cur_ci];
        inter_tid = inter_id[cur_ci];
        inter_inst = inter_arg1[cur_ci];
        inter_addr = inter_arg2[cur_ci];

    }
    cur_ci++;
    return 1;

}

VOID BeforeMemAccess(UINT32 ip, UINT32 addr, THREADID tid, UINT32 rtnName){
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
        if(ip == first_inst && addr == first_addr && tid == first_tid){
            first_ready = true;
            #ifdef DEBUG_REPLAY
                printf("\033[01;34m[1][execute First] inst:0x%x, addr:0x%x, tid:%u, rtn:%s\033[0m\n",ip, addr, tid, (char*)rtnName);
            #endif
            return; //return early to imporve efficiency
        }
        else if(ip == inter_inst && addr == inter_addr && tid == inter_tid){
            countTimeout = 0;
            while(first_ready == false){ 
                #ifdef DEBUG_REPLAY             
                    printf("\033[01;33m[2][delay Inter] inst:0x%x, addr:0x%x, tid:%u, rtn:%s \033[0m\n", ip, addr, tid, (char*)rtnName);
                #endif
                PIN_Sleep(SLEEP_TIME);
                countTimeout++;
                if (countTimeout > TIMEOUT){
                    //printf("\033[01;33m[2][delay Inter] timeout!\033[0m\n");
                    exit(0);
                }
            }
            inter_ready = true;
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[3][execute Inter after delay] inst:0x%x, addr:0x%x, tid:%u, rtn:%s \033[0m\n", ip,addr,tid,(char*)rtnName);
            #endif
            return;
        }
        

    }
    
    else if(first_ready == true && inter_ready == false && second_ready == false){ 
        if(ip == inter_inst && addr == inter_addr && tid == inter_tid){
            inter_ready = true;
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[4][execute Inter] inst:0x%x, addr:0x%x, tid:%u, rtn:%s \033[0m\n", ip,addr,tid,(char*)rtnName);
            #endif
            return;
        }
        else if(ip == second_inst && addr == second_addr && tid == second_tid){
            countTimeout = 0;
            while(inter_ready == false){  
                #ifdef DEBUG_REPLAY               
                    printf("\033[01;33m[5][delay Second] inst:0x%x, addr:0x%x, tid:%u, rtn:%s \033[0m\n", ip,addr,tid,(char*)rtnName);
                #endif
                PIN_Sleep(SLEEP_TIME);
                countTimeout++;
                if (countTimeout > TIMEOUT){  
                    //printf("\033[01;33m[2][delay Second] timeout!\033[0m\n");
                    exit(0);
                }
            }
            second_ready = true;
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[6][execute Second after delay] inst:0x%x, addr:0x%x, tid:%u, rtn:%s \033[0m\n", ip,addr,tid,(char*)rtnName);
            #endif
            return;
        }
        

    }
    else if(first_ready == true && inter_ready == true && second_ready == false){ 
        if(ip == second_inst && addr == second_addr && tid == second_tid){
            second_ready = true;
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[5][execute Second] inst:0x%x, addr:0x%x, tid:%u, rtn:%s \033[0m\n",ip,addr,tid,(char*)rtnName);
            #endif
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == true && second_ready == true){ 
        #ifdef DEBUG_REPLAY  
            printf("\033[01;34m[5][load next ci] \033[0m\n");
        #endif
        load_next_ci();
        return;
    }
    else
        assert(false);
}



VOID beforeThreadLock_replay(THREADID tid, UINT32 ip, UINT32 callsite_v)
{

    if(!is_locked_type) //only locked type is handled in this function.
        return;
    //PIN_GetLock(&lock, tid+1);
    
    if(first_ready == false && inter_ready == false && second_ready == false){
        if(tid == inter_tid && callsite_v == inter_lock){
            countTimeout = 0;
            while(first_ready == false){  
                #ifdef DEBUG_REPLAY                
                    printf("\033[01;33m[beforeThreadLock][delay Inter] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
                #endif
                PIN_Sleep(SLEEP_TIME);
                countTimeout++;
                if (countTimeout > TIMEOUT){
                    //printf("\033[01;33m[2][beforeThreadLock][delay Inter] timeout!\033[0m\n");
                    exit(0);
                }
            }
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadLock][enter Inter after delay] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        else if(tid == first_tid && callsite_v == first_lock){
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadLock][enter First] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == false && second_ready == false){ 
        if(tid == second_tid && callsite_v == second_lock){
            countTimeout = 0;
            while(inter_ready == false){               
                #ifdef DEBUG_REPLAY  
                    printf("\033[01;33m[beforeThreadLock][delay Second] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
                #endif
                PIN_Sleep(SLEEP_TIME);
                countTimeout++;
                if (countTimeout > TIMEOUT){ 
                    //printf("\033[01;33m[2][beforeThreadLock][delay Second] timeout!\033[0m\n");
                    exit(0);
                }
            }
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadLock][enter Second after delay] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        else if(tid == inter_tid && callsite_v == inter_lock){
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadLock][enter Inter] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        

    }
    else if(first_ready == true && inter_ready == true && second_ready == false){
        if(tid == second_tid && callsite_v == second_lock){
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadLock][enter Second] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        
    }
  

    //PIN_ReleaseLock(&lock);
}
VOID beforeThreadUnLock_replay(THREADID tid, UINT32 ip, UINT32 callsite_v)
{
    if(!is_locked_type) //only locked type is handled in this function.
        return;

    //PIN_GetLock(&lock, tid+1);

    if(first_ready == false && inter_ready == false && second_ready == false){
        if(tid == first_tid && callsite_v == first_unlock){
            first_ready = true;
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadUnLock][executed First] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == false && second_ready == false){ 
        if(tid == inter_tid && callsite_v == inter_unlock){
            inter_ready = true;
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadUnLock][executed Inter] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == true && second_ready == false){ 
        if(tid == second_tid && callsite_v == second_unlock){
            second_ready = true;
            #ifdef DEBUG_REPLAY  
                printf("\033[01;34m[beforeThreadUnLock][executed Second] callsite_v:0x%x, tid:%u \033[0m\n", callsite_v,tid);
            #endif
            return;
        }
        
    }
    else if(first_ready == true && inter_ready == true && second_ready == true){ 
        #ifdef DEBUG_REPLAY  
            printf("\033[01;34m[load next ci]\033[0m\n");
        #endif
        load_next_ci();
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

    if(INS_IsStackRead(ins))
        return;
    if(INS_IsStackWrite(ins))
        return; 

    
    RTN rtn = INS_Rtn(ins); 
    //if(isLibRtnName(RTN_Name(rtn))) 
        //return; 
    
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

    /*
    printf("argc = %d\n",argc);
    printf("=========================\n");
    for (unsigned int j = 0; j < (unsigned int)argc; j++)    
        printf("argv[%d] = %s\n", j, argv[j]);
    

    replay_log = fopen(argv[argc-1], "r"); //argv[7] is the file of the groupd results
    if (!replay_log){
        printf("Open file %s failed!\n", argv[7]);
        return 0;
    }
    */

    FILE * counter;
    int c;
    counter = fopen("work_dir/groupset/counter.log","r");
    if (!counter){
        replay_log = fopen("work_dir/groupset/group_0.log", "r");
        counter = fopen("work_dir/groupset/counter.log","w");
        c = 1;
        fprintf(counter,"%d",c); // first time to read, pointer to  group_1 
        fclose(counter);
    }
    else{
        counter = fopen("work_dir/groupset/counter.log","r");
        fscanf(counter,"%d", &c);
        fclose(counter);

        

        // point counter to the next group
        std::string s1 = "work_dir/groupset/group_";
        std::string s2 = ".log";

        std::ostringstream oss;
        oss << c;
        std::string s0 = oss.str();

        std:: string s = s1+s0+s2;

        std::cout<<s<<endl;
        replay_log = fopen(s.c_str(), "r");

        counter = fopen("work_dir/groupset/counter.log","w");
        c++;
        fprintf(counter,"%d",c);
        fclose(counter);

        
    }



    ci_count = 0;
    while (ci_count < max_size){
        if((fscanf(replay_log,"%u[%u]%d,%d; [%u]%d,%d; [%u]%d,%d\n",
                &type[ci_count], &first_id[ci_count], &first_arg1[ci_count], &first_arg2[ci_count], 
                &second_id[ci_count], &second_arg1[ci_count], &second_arg2[ci_count], 
                &inter_id[ci_count], &inter_arg1[ci_count], &inter_arg2[ci_count]))!=EOF){

            #ifdef DEBUG_REPLAY  
                printf("\033[22;36m[load CI from file]:%u[%u]0x%x,0x%x; [%u]0x%x,0x%x; [%u]%x,%x \033[0m\n",
                type[ci_count], first_id[ci_count], first_arg1[ci_count], first_arg2[ci_count], 
                second_id[ci_count], second_arg1[ci_count], second_arg2[ci_count], 
                inter_id[ci_count], inter_arg1[ci_count], inter_arg2[ci_count]);
            #endif
            ci_count++;


        }
        else 
            break;

    }

    #ifdef DEBUG_REPLAY  
        printf("Loaded %d CI(s) for replay\n", ci_count);
    #endif

    cur_ci = 0;
    load_next_ci();


    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    //OutFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    INS_AddInstrumentFunction(Instruction, 0);

    // Register Routine to be called to instrument rtn
    RTN_AddInstrumentFunction(Routine, 0);


    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}