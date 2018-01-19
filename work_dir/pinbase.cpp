#include "pin.H"
#include <iostream>
#include <fstream>



/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    PIN_ERROR( "This Pintool is used to measure the overhead introduced by pin.\n" 
              + KNOB_BASE::StringKnobSummary() + "\n");

    cerr << "This Pintool is used to measure the overhead introduced by pin." << endl;
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


    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    //OutFile.open(KnobOutputFile.Value().c_str());

    // Register Instruction to be called to instrument instructions
    //INS_AddInstrumentFunction(Instruction, 0);



    // Register Routine to be called to instrument rtn
    //RTN_AddInstrumentFunction(Routine, 0);

    // Register Analysis routines to be called when a thread begins/ends
    //PIN_AddThreadStartFunction(ThreadStart, 0);
    //PIN_AddThreadFiniFunction(ThreadFini, 0);

    // Register Fini to be called when the application exits
    //PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
