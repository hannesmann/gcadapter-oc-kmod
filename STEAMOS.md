# Compiling on SteamOS

Using the module on SteamOS is possible but some additional steps are required because Valve has made the root filesystem read-only by default.

First, switch to desktop mode and run these commands in the terminal to set up the package manager:
```
sudo steamos-readonly disable

sudo pacman-key --init
sudo pacman-key --populate archlinux
```

Install the base development tools and Valve kernel headers (accept all packages when prompted):
```
sudo pacman -S base-devel linux-neptune-headers
```

Switch to the directory where you downloaded gcadapter-oc-kmod (or open a new terminal by right clicking and selecting `Open Terminal Here`). These commands will build the module and only need to be executed once:
```
make
sudo steamos-readonly enable # optional step, to relock the filesystem 
```

The last command needs to be executed every time you restart the device (but it persists when switching in and out of desktop mode): 
```
sudo insmod gcadapter_oc.ko
```

**These steps need to be repeated every time you update SteamOS.** If Valve updates their kernel, the old module you built will no longer work (`insmod` will tell you when there's a problem).