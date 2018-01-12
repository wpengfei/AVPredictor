



rm work_dir/replay/*

#main_bank_lock  2 2
#main_bank_nolock 2 2
#main_circular  11 1  3 1
#log_proc_sweep  4 4
#string_buffer  2 2
#mysql_169  37 37 dump  / 192 192    117 117  42 42
#apache_httpd  6 6    5 5
#mysql_4012  1 1
#mozilla  1 1

echo ================================================ monitor run
#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/mysql_169 
#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/RADIX -p 2
time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pbzip2 -b15qkf test_dir/testfile
#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pbzip2 -df testfile.bz2
#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_cases/pfscan/pfscan -d file ../test_cases/pfscan/pfscan.c

echo ================================================ prediction 
time python work_dir/predictor.py



echo ================================================ relay run

resultdir=$(pwd)/'work_dir/replay'

for curfile in $(ls ${resultdir})
do
    if test -f ${resultdir}/${curfile}
    then
    	echo ${curfile}
    	time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/replay.so -- test_dir/pbzip2 -b15qkf test_dir/testfile  "work_dir/replay/"${curfile}
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/replay.so -- test_dir/pbzip2 -b15qk -f my.tar "work_dir/replay/"${curfile}
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/replay.so  -- test_dir/mysql_169 "work_dir/replay/"${curfile}
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/replay.so -- .test_cases/pfscan/pfscan -d file ../test_cases/pfscan/pfscan.c "replay/"${curfile}

   
    fi
done

