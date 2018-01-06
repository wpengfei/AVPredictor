#sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"

#CPLUS_INCLUDE_PATH=$CPLUS_INCLUDE_PATH:/home/wpf/Desktop/jsoncpp/include/
#export CPLUS_INCLUDE_PATH

cd ~/Desktop/AVPredictor
#gcc test.c -lpthread -o test

cd work_dir

rm obj-ia32/monitor.so
rm obj-ia32/monitor.o
rm obj-ia32/replay.so
rm pattern_for_replay.log


echo ================================================

#main_bank_lock  2 2
#main_bank_nolock 2 2
#main_circular  11 1
#log_proc_sweep  4 4
#string_buffer  2 2
#mysql_169  37 37 dump  / 192 192
#apache_httpd  6  6
#mysql_4012  1 1
#mozilla  1 1

make PIN_ROOT=../pin-3.2-81205-gcc-linux/  obj-ia32/monitor.so 
time ../pin-3.2-81205-gcc-linux/pin -t  obj-ia32/monitor.so -- ~/Desktop/pin_work/test_dir/apache_httpd 
#time ../pin-3.2-81205-gcc-linux/pin -t obj-ia32/pintool_main.so -- ~/Desktop/pin_work/test_dir/FFT -p 2
#time ../pin-3.2-81205-gcc-linux/pin -t obj-ia32/pintool_main.so -- ~/Desktop/pin_work/test_cases/pfscan/pfscan -d file ~/Desktop/pin_work/test_cases/pfscan/pfscan.c
echo ================================================

#make PIN_ROOT=../pin-3.2-81205-gcc-linux/  obj-ia32/replay.so
#time ../pin-3.2-81205-gcc-linux/pin -t obj-ia32/replay.so -- ~/Desktop/pin_work/test_dir/main_circular

