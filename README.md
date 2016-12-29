# wxVcashGUI
A multiplatform GUI for Vcash, a decentralized currency for the internet (https://vcash.info/).

# Dependencies
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


# Building with Microsoft Visual Studio 2015

These are instructions for building a Windows 64 bits executable and this procedure
has been tested on Windows 10 x64 with MSVC14 (Visual Studio 2015).

Follow instructions to build vcash with Microsoft Visual Studio: 

https://github.com/openvcash/vcash/blob/master/platforms/windows/README.md

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



Download wxVcashGUI from https://github.com/openvcash/wxVcashGUI/archive/master.zip and uncompress this file inside your vcash-master folder.

cd to this directory 
```
cd C:\vcash-master\wxVcashGUI
```

Enter the following command:
```
nmake /f makefile.vc BUILD=release TARGET_CPU=X64 RUNTIME_LIBS=static
```

Enjoy your GUI for Vcash!
