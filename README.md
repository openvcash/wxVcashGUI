# wxVcashGUI
A multiplatform GUI for Vcash, a decentralized currency for the internet (https://vcash.info/).

# Building with Microsoft Visual Studio

These are instructions for building a Windows 64 bits executable and this procedure
has been tested on Windows 10 x64 with MSVC14 (Visual Studio 2015).

Follow instructions to build vcash with Microsoft Visual Studio: 

https://github.com/openvcash/vcash/blob/master/platforms/windows/README.md

For building with Visual Studio 2015, the only differences with respect to those instructions are:

* Use this command to build boost:
```
b2 -j3 toolset=msvc-14.0 address-model=64 architecture=x86 link=static threading=multi runtime-link=static --with-system --stagedir=stage/x64
```

* Add legacy_stdio_definitions.lib to list of linked libraries for Vcash.sln.

* Add this code to file C:\vcash-master\coin\test\main.cpp:
```
#if defined(_MSC_VER) && (_MSC_VER >= 1900)
FILE _iob[] = { *stdin, *stdout, *stderr };
extern "C" FILE * __cdecl __iob_func(void) { return _iob; }
#endif
```

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

Enjoy your GUI for vcash!