# wxVcashGUI
A multiplatform GUI for Vcash, a decentralized currency for the internet (https://vcash.info/).

# Building vcash

In order to build wxVcashGUI, you will need to build firstly vcash.

## On Linux

You can build vcash by using the script at:

https://github.com/xCoreDev/vcash-scripts

The resulting build will be located at ~/vcash/.

## On Windows using Microsoft Visual Studio 2015

These are instructions for building a Windows 64 bits executable and this procedure
has been tested on Windows 10 x64 with MSVC14 (Visual Studio 2015).

Follow the following instructions to build vcash with Microsoft Visual Studio:

https://github.com/openvcash/vcash/blob/master/platforms/windows/README.md

For building with Visual Studio 2015, the only difference with respect to those instructions is to use the following command to build boost:
```
b2 -j3 toolset=msvc-14.0 address-model=64 architecture=x86 link=static threading=multi runtime-link=static --with-system --stagedir=stage/x64
```


# Building and installing wxWidgets

## On linux
Download source code of wxWidgets: https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxWidgets-3.1.0.tar.bz2

Uncompress this file and cd to corresponding directory:
```
cd wxWidgets-3.1.0
```

Make a build directory , configure and build:
```
cd build
mkdir gtk
cd gtk
../../configure --enable-unicode --enable-debug --disable-shared
make
```

Go for a cup of coffee :) .

Install wxWidgets:
```
sudo make install
wx-config --version
```


## On Windows using Microsoft Visual Studio 2015

Download source code of wxWidgets: https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.0/wxWidgets-3.1.0.zip

Uncompress this file to folder C:\wxWidgets-3.1.0

Open a Visual Studio x64 native tools command prompt (** choose the right prompt or compilation will fail **).

cd to directory C:\wxWidgets-3.1.0\build\msw:
```
cd C:\wxWidgets-3.1.0\build\msw
```

Enter the following command to build wxWidgets:
```
nmake /f makefile.vc BUILD=release TARGET_CPU=X64 RUNTIME_LIBS=static
```

and go for a cup of coffee :) .


# Downloading wxVcashGUI

## On linux

Download wxVcashGUI from https://github.com/openvcash/wxVcashGUI/archive/master.zip and uncompress this file inside your ~/vcash/src folder.


## On Windows

Download wxVcashGUI from https://github.com/openvcash/wxVcashGUI/archive/master.zip and uncompress this file inside your vcash-master folder (C:\vcash-master).


# Building libqrencode

libqrencode (https://github.com/fukuchi/libqrencode) library must be built previously in directory deps.

On Linux, cd to that directory:
```
cd ~/vcash-master/wxVcashGUI/deps
```

Clone libqrencode library:
```
git clone https://github.com/fukuchi/libqrencode
```

Build the library:
```
cd libqrencode
cmake CMakeLists.txt
make
```

On Windows, I was also able to build this library using cmake.



# Building wxVcashGUI

# On Linux

cd to wxVcashGUI directory:
```
cd ~/vcash/src/wxVcashGUI-master
```

Enter the following commands:
```
cmake CMakeLists.txt
make
```

## On Windows using Microsoft Visual Studio 2015

cd to wxVcashGUI directory:
```
cd C:\vcash-master\wxVcashGUI-master
```

Enter the following command:
```
nmake /f makefile.vc BUILD=release TARGET_CPU=X64 RUNTIME_LIBS=static
```

Enjoy your GUI for Vcash!
