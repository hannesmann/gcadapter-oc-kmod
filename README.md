# gcadapter_oc_kmod

Kernel module for overclocking the Nintendo Wii U/Mayflash GameCube adapter.

The default overclock is from 125 Hz to 1000 Hz. Official adapters should be able to handle this but if you experience stutter or dropped inputs you can try lowering the rate to 500 Hz.

This [document](https://docs.google.com/document/d/1cQ3pbKZm_yUtcLK9ZIXyPzVbTJkvnfxKIyvuFMwzWe0/edit) by [SSBM_Arte](https://twitter.com/SSBM_Arte) has more detailed information regarding controller overclocking.

## Installing

- **Arch Linux, Manjaro, etc:** Arch-based distros use DKMS to rebuild the module on every kernel update. A PKGBUILD is available in `packaging/arch` and in the [AUR](https://aur.archlinux.org/packages/gcadapter-oc-dkms). A prebuilt package can be found in the "Releases" tab.
- **Fedora, Nobara, etc:** RPM-based distros use AKMOD to rebuild the module on every kernel update. A SPEC file is available in `packaging/rpms`. A prebuilt package can be found in the "Releases" tab.
- **SteamOS 3 (Steam Deck):** Instructions for a manual install are available in [STEAMOS.md](STEAMOS.md).

For other distros, follow the build guide and copy the module to an appropriate directory under `/usr/lib/modules` (example: `/usr/lib/modules/$(uname -r)/extra`). After copying the module, run `depmod` and create a file called `/usr/lib/modules-load.d/gcadapter-oc.conf` with the contents `gcadapter_oc`. This should be enough to load the module automatically. The module will need to be rebuilt every time you update the kernel.

## Building

Use `make` to build gcadapter_oc.ko and `sudo insmod gcadapter_oc.ko` to load the module into the running kernel.

[![asciicast](https://asciinema.org/a/455371.svg)](https://asciinema.org/a/455371)

If you want to unload the module (revert the increased polling rate) use `sudo rmmod gcadapter_oc.ko`. You can also use `make clean` to clean up any files created by `make`.

If you get an error saying "building multiple external modules is not supported" it's because you have a space somewhere in the path to the gcadapter-oc-kmod directory.

GNU Make can't handle spaces in filenames so move the directory to a path without spaces (example: `/home/falco/My Games/gcadapter-oc-kmod` -> `/home/falco/gcadapter-oc-kmod`).

## Changing the polling rate

Polling rate is set according to the `bInterval` value in the USB endpoint descriptor. The value sets the polling rate in milliseconds, for example: an interval value of 4 equals 250 Hz.

You can change the rate by using the kernel parameter `gcadapter_oc.rate=n` (if installed), passing the rate to `insmod gcadapter_oc.ko rate=n` or going into `/sys/module/gcadapter_oc/parameters` and using `echo n > rate` to change the value ([video](https://asciinema.org/a/455373)).
