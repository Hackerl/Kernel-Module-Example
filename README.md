# Kernel-Module-Example
## Prepare
Please install linux headers first.
```sh=
sudo apt install linux-headers-$(uname -r)
```
## Build
```sh=
mkdir build; cd build
cmake ..; make
```
## Test
Require root.
```sh=
insmod driver_demo.ko
echo "hello world" > /proc/lkm_demo/data_pipe
cat /proc/lkm_demo/data_pipe
rmmod driver_demo
```
