# gcadapter_oc_kmod

Kernel module for overclocking the Nintendo Wii U/Mayflash GameCube adapter.

The default overclock is from 125 hz to 1000 hz. If you experience stutter or dropped inputs try lowering the rate to 500 hz.

This [document](https://docs.google.com/document/d/1cQ3pbKZm_yUtcLK9ZIXyPzVbTJkvnfxKIyvuFMwzWe0/edit) by [SSBM_Arte](https://twitter.com/SSBM_Arte) has more detailed information regarding controller overclocking.

## Building and running

`make` to build gcadapter_oc.ko

`make clean` to clean up

`sudo insmod gcadapter_oc.ko` to load the module into the running kernel.

`sudo rmmod gcadapter_oc.ko` to unload the module.

## Packaging

A PKGBUILD is available for Arch Linux in `packaging/`. This package uses DKMS to install and auto-update the module when the kernel is updated. A configuration file is added to load the module automatically on boot. 

Prepackaged versions can be found under "Releases".

## Changing the polling rate

Polling rate is set according to the `bInterval` value in the USB endpoint descriptor. The value sets the polling rate in milliseconds, for example: an interval value of 4 equals 250 hz.

You can change the rate by using the kernel parameter `gcadapter_oc.rate=n` (if installed), passing the rate to `insmod gcadapter_oc.ko rate=n` or going into `/sys/module/gcadapter_oc/parameters` and using `echo n > rate` to change the value.

## Video

This video shows how to load the module and change the polling rate: https://streamable.com/sxysq
