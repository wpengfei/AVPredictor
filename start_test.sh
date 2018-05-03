
rm work_dir/groupset/*
rm work_dir/trace_mem.log
rm work_dir/trace_lock.log
rm work_dir/trace_sync.log

#main_bank_lock  2 2
#main_bank_nolock 2 2
#main_circular  11 1  3 1
#log_proc_sweep  4 4
#string_buffer  2 2
#mysql_169  37 37 dump  / 192 192    117 117  42 42
#mysql_4012  1 1
#apache_httpd  6 6    5 5
#mozilla  1 1

#FFT
#LU
#RADIX

echo ================================================ monitored run

time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/main_bank_lock

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/FFT -p 2

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pbzip2 -b15kf test_dir/testfile

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pbzip2 -dfk test_dir/testfile1.bz2 

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pfscan -dlv file test_dir/testfile

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/aget -n10 http://bbs.pdfwork.cn/static/image/common/research.png

#sudo time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so --  ./test_apache.sh

#time sudo  pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- /usr/local/apache2/bin/apachectl start

#time sudo pin-3.2-81205-gcc-linux/pin -follow_execv -t work_dir/obj-ia32/monitor.so -- /usr/local/apache2/bin/httpd -k start

#time /usr/local/apache2/bin/ab -n 2 -c 2 http://localhost/index.php

#sudo /usr/local/apache2/bin/httpd -k stop

#-follow_execv

echo ================================================ prediction 

cd work_dir
time python offline_modules.py
cd ..


echo ================================================ controlled run

resultdir=$(pwd)/'work_dir/groupset'

for curfile in $(ls ${resultdir})
do
    if test -f ${resultdir}/${curfile}
    then
    	echo ${curfile} "work_dir/groupset/"${curfile}

    	time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so  -- test_dir/main_bank_lock 

    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/FFT -p 2  

    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/pbzip2 -b15kf test_dir/testfile  
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/pbzip2 -dfk test_dir/testfile1.bz2  
    	
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/pfscan -dlv file test_dir/testfile 

	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/aget -n10 http://bbs.pdfwork.cn/static/image/common/research.png 
   

        #time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so  --  /usr/local/apache2/bin/ab -n 100 -c 2 http://localhost/index.php/ 


    fi
done

