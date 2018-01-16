
rm work_dir/groupset/*

#main_bank_lock  2 2
#main_bank_nolock 2 2
#main_circular  11 1  3 1
#log_proc_sweep  4 4
#string_buffer  2 2
#mysql_169  37 37 dump  / 192 192    117 117  42 42
#mysql_4012  1 1
#apache_httpd  6 6    5 5
#mozilla  1 1


echo ================================================ monitor run
#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/mozilla 

time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/FFT -p 2

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pbzip2 -b15qkf test_dir/testfile

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pbzip2 test_dir/testfile.bz2 -dfk

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/pfscan -d file test_dir/testfile

#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/monitor.so -- test_dir/aget -n10 http://bbs.pdfwork.cn/static/image/common/research.png


echo ================================================ prediction 

cd work_dir
time python offline_modules.py
cd ..


echo ================================================ relay run

resultdir=$(pwd)/'work_dir/groupset'

for curfile in $(ls ${resultdir})
do
    if test -f ${resultdir}/${curfile}
    then
    	echo ${curfile}
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so  -- test_dir/mozilla "work_dir/groupset/"${curfile}

    	
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/pbzip2 -b15qkf test_dir/testfile  "work_dir/groupset/"${curfile}
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/pbzip2 -dfk test_dir/testfile.bz2  "work_dir/groupset/"${curfile}
    	
    	#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/pfscan -d file test_dir/testfile "work_dir/groupset/"${curfile}

		#time pin-3.2-81205-gcc-linux/pin -t work_dir/obj-ia32/controller.so -- test_dir/aget -n10 http://bbs.pdfwork.cn/static/image/common/research.png "work_dir/groupset/"${curfile}
   

    fi
done

