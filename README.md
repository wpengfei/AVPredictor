# AVPredictor
A prototype tool to find Atomicity-violation bugs in multi-threaded programs

Before use:

1. This tool works for Ubuntu 16.04 x86 platform.
2. Install scanf-1.2 before use .
3. Run sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space" to disable ASLR.


Use:

1. Put the binaries in test_dir/ for test.
2. Run ./start_compile.sh to compile AVPredictor.
3. Modify the items in ./start_test.sh to claim the test program and specify the arguments (see the annotation), including the monitored run and the controlled run.
4. Run ./start_test.sh to test the program.
5. The grouped interleavings are in work_dir/groupset/.
6. The test cases used are in test_cases/, install bzip2 before testing pbzip2.

Contact wpengfeinudt@gmail.com if you have any questions.
