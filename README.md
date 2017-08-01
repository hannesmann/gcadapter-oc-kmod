# gcadapter_oc_kmod

Kernel module for overclocking (setting the polling rate of) the Nintendo Wii U/Mayflash GameCube adapter.

The default overclock is from 125 hz -> 500 hz.

## Requirements

`linux` + `linux-headers`

## Building and running

`make` to build gcadapter_oc.ko

`make clean` to clean up

`sudo insmod gcadapter_oc.ko` to load the module into the running kernel (you'll need to replug the adapter to get any effect, check `dmesg`)

`sudo rmmod gcadapter_oc.ko` to unload the module (restores the default rate of 125 hz)

## Changing the polling rate

Polling rate is set according to the `bInterval` value in the USB endpoint descriptor. The value sets the rate in ms, so you can for example set the rate to 4 to get (1000 / 4) 250 hz.

You can change the rate by using the kernel parameter `gcadapter_oc.rate=<rate>` (if installed), passing the rate to `sudo insmod` or going into `/sys/module/gcadapter_oc/parameters` and `echo`-ing the value.

## Video
https://streamable.com/sxysq
