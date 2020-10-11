# gcadapter_oc_kmod

Kernel module for overclocking the Nintendo Wii U/Mayflash GameCube adapter.

The default overclock is from 125 hz to 500 hz. Overclocking up to 1000 hz is possible but 500 hz is set as a "safe" default since the official Nintendo adapter can't keep up with a consistent 1000 hz.

## Building and running

`make` to build gcadapter_oc.ko

`make clean` to clean up

`sudo insmod gcadapter_oc.ko` to load the module into the running kernel.

`sudo rmmod gcadapter_oc.ko` to unload the module.

## Changing the polling rate

Polling rate is set according to the `bInterval` value in the USB endpoint descriptor. The value sets the polling rate in milliseconds, for example: an interval value of 4 equals 250 hz.

You can change the rate by using the kernel parameter `gcadapter_oc.interval=<rate>` (if installed), passing the rate to `sudo insmod` or going into `/sys/module/gcadapter_oc/parameters` using `echo` to change the value.