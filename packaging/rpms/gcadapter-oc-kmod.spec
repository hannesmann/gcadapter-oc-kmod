# (un)define the next line to either build for the newest or all current kernels
# %define buildforkernels newest
# %define buildforkernels current
%define buildforkernels akmod

# Turn off debug package
%global debug_package %{nil}

%define commit       6e3381d07d01714bd6bd2198be83f45230e2f3d0
%define short_commit %(c=%{commit}; echo ${c:0:7})

%define repo         rpmfusion

# name should have a -kmod suffix
Name:           gcadapter-oc-kmod
Version:        1.4
Release:        1%{?dist}
Summary:        Kernel module(s)
Group:          System Environment/Kernel
License:        GPLv2
URL:            https://github.com/hannesmann/gcadapter-oc-kmod
Source0:        %{URL}/archive/%{commit}/%{name}-%{short_commit}.tar.gz
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

# Packages this RPM provides
Provides: gcadapter-oc-kmod-common

# Packages require to build
BuildRequires: kmodtool
BuildRequires: gcc
BuildRequires: make
%{!?kernels:BuildRequires: buildsys-build-rpmfusion-kerneldevpkgs-%{?buildforkernels:%{buildforkernels}}%{!?buildforkernels:current}-%{_target_cpu} }

# kmodtool does its magic here
%{expand:%(kmodtool --target %{_target_cpu} --repo %{repo} --kmodname %{name} %{?buildforkernels:--%{buildforkernels}} %{?kernels:--for-kernels "%{?kernels}"} 2>/dev/null) }

%description
This module contains the kmod module from %{URL} and overclocks the GameCube USB adapter.

%prep
echo "PREP--------------------------------------------------" 
# error out if there was something wrong with kmodtool
%{?kmodtool_check}

# print kmodtool output for debugging purposes:
kmodtool  --target %{_target_cpu}  --repo %{repo} --kmodname %{name} %{?buildforkernels:--%{buildforkernels}} %{?kernels:--for-kernels "%{?kernels}"} 2>/dev/null
echo "------------------------------------------------------"

%setup -q -c
echo "SETUP-------------------------------------------------"
# For each kernel version we are targeting
for kernel_version in %{?kernel_versions} ; do
  # Make a copy of the source code that was downloaded by running spectool and automatically extracted
  %{__cp} -a %{name}-%{commit} _kmod_build_${kernel_version%%___*}
done
echo "------------------------------------------------------"

%build
echo "BUILD-------------------------------------------------"
# For each kernel version we are targeting
for kernel_version in %{?kernel_versions}; do
  # Make/Build the kernel module (by running make in the directories previous copied) (This makes the .ko files in each of those respective directories)
  %{__make} %{?_smp_mflags} -C "${kernel_version##*___}" M=${PWD}/_kmod_build_${kernel_version%%___*} modules
done
# Create gcadapter_oc.conf file needed for autoloading module at boot (will be installed into /etc/modules-load.d/ in install step)
cat > gcadapter_oc.conf <<EOF
gcadapter_oc
EOF
echo "------------------------------------------------------"

%install
echo "INSTALL-----------------------------------------------"
# For each kernel version we are targeting
for kernel_version in %{?kernel_versions}; do
  # Make the directory the kernel module will be installed into in the BUILDROOT folder
  mkdir -p %{buildroot}%{kmodinstdir_prefix}/${kernel_version%%___*}/%{kmodinstdir_postfix}/
  # Install the previously built kernel module (This moves and compresses the .ko file to the directory created above)
  install -D -m 755 _kmod_build_${kernel_version%%___*}/gcadapter_oc.ko %{buildroot}%{kmodinstdir_prefix}/${kernel_version%%___*}/%{kmodinstdir_postfix}/
  # Make the installed kernel module executable for all users 
  chmod a+x %{buildroot}%{kmodinstdir_prefix}/${kernel_version%%___*}/%{kmodinstdir_postfix}/*.ko
done
# Make the directory the .conf file will be installed into in the BUILDROOT folder
mkdir -p %{buildroot}/etc/modules-load.d/
# Install the previously built .conf file
install -m 755 gcadapter_oc.conf %{buildroot}/etc/modules-load.d/gcadapter_oc.conf
# AKMOD magic I guess?
%{?akmod_install}
echo "------------------------------------------------------"

%clean
echo "CLEAN-------------------------------------------------"
# Cleanup the BUILDROOT
%{__rm} -rf %{buildroot}
echo "------------------------------------------------------"

%files
/etc/modules-load.d/gcadapter_oc.conf
