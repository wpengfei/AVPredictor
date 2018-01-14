#sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"

cd work_dir

rm obj-ia32/monitor.so
rm obj-ia32/monitor.o
rm obj-ia32/controller.so
rm obj-ia32/controller.o


make PIN_ROOT=../pin-3.2-81205-gcc-linux/  obj-ia32/monitor.so 
make PIN_ROOT=../pin-3.2-81205-gcc-linux/  obj-ia32/controller.so

echo compilation finished.
