# Simple Multiplayer Networking Librayr

libsmpn is a simple networking library designed on top of Enet.
My goals with libsmpn is to make it easy to make multiplayer games in C/C++.

## Supported platforms
- Linux
- Windows
*You can theoretically run this library on any platform that supports ENet and GCC.*

## Compiling the library
Here is some guids on how to compile the source of the library into a static library.
### Compiling on Linux

To compile on linux, you need to have a C compiler installed (duh).
You will need to install Enet on your system weather its by compiling it by source or installing it via your linux package manager.
You can find enet [here](http://sauerbraten.org/enet/).

Once you have installed enet, you can run these commands to compile the static library (make sure to have make and git installed)
```sh
git clone https://github.com/KBOX1234/multiplayer-lib
cd multiplayer-lib
make
sudo make install
```
After that, when you compile programs using libsmpn, just make sure to add -lsmpn as a linker flag.

### Compiling on Windows
Compiling on Windows is a little trickier but not too bad.
As of now, we only support GCC in our makefile but that can easily by changed later.

You will need to install Enet on your system weather its by compiling it by source or installing it via your linux package manager.
You can find enet [here](http://sauerbraten.org/enet/).

Once you have installed enet, you can run these commands to compile the static library (make sure to have make and git installed)

```bat
git clone https://github.com/KBOX1234/multiplayer-lib
cd multiplayer-lib
make
make install
```

Since GCC toolchains don't have a universal directory to store all libraries, you will need to specify where everything should be install when running ```make install```
```bat
make install PREFIX=C:/mylibs
```

When compiling a program with libsmpn on windows, unless you installed it to the compilers default lib and header directories, you will need to point the compiler to where it is installed like this: ```-LC:/mylibs/lib -IC:/mylibs/include```.
Then add add -lsmpn as a linker flag.
