#sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"

cd work_dir

rm obj-ia32/monitor.so
rm obj-ia32/monitor.o

#rm obj-ia32/monitor2.so
#rm obj-ia32/monitor2.o


rm obj-ia32/controller.so
rm obj-ia32/controller.o

rm obj-ia32/pinbase.so
rm obj-ia32/pinbase.o

make PIN_ROOT=../pin-3.7-97619-gcc-linux/  obj-ia32/pinbase.so 
make PIN_ROOT=../pin-3.7-97619-gcc-linux/  obj-ia32/monitor.so
#make PIN_ROOT=../pin-3.7-97619-gcc-linux/  obj-ia32/monitor2.so 
make PIN_ROOT=../pin-3.7-97619-gcc-linux/  obj-ia32/controller.so

echo compilation finished.
