# Compiling on SteamOS

Using the module on SteamOS is possible but some additional steps are required because Valve has made the root filesystem read-only by default.

First, switch to desktop mode and run these commands in the terminal to set up the package manager:

```text
sudo steamos-readonly disable

sudo pacman-key --init
sudo pacman-key --populate archlinux holo
```

Install the base development tools and kernel headers (accept all packages when prompted):

```text
sudo pacman -S --needed base-devel "$(cat /usr/lib/modules/$(uname -r)/pkgbase)-headers"
```

These commands will download and build the module. They only need to be executed once.

```text
git clone https://github.com/hannesmann/gcadapter-oc-kmod.git
cd gcadapter-oc-kmod
make
# optional step, to relock the filesystem 
sudo steamos-readonly enable 
```

The last command needs to be executed every time you restart the device (but it persists when switching in and out of desktop mode):

```text
sudo insmod gcadapter_oc.ko
```

**These steps need to be repeated every time you update SteamOS.** If Valve updates their kernel, the old module you built will no longer work (`insmod` will tell you when there's a problem).
