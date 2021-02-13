# AVR C++ Projects

## Introduction

All of the projects here were built using avr-g++ version 10.1.0. This was to enable use of better optimisations and newer C++ standard features. For information on building GCC follow [Building GCC from source](#Building-gcc-from-source)

## Projects

Here are the projects included in this repository

### AVR libC++

Standard C++ library for AVR. Currently ported from GCC-x86 headers. No implementation of functions yet. (Feel free to contribute)

### AVR libUSB++

A Full USB-1.1 Low Speed Stack with a C++ API, support for 32 endpoints and more!

### AVR libMultiProcessing

A simple multi-processing library with round-robin scheduling.


## Building GCC from source

I recommend following this guide, since it has all the configurations required for AVR, however if this is your first time building GCC from source, I recommend getting familiar with the process by following [this guide](https://gcc.gnu.org/install/)

Firstly, download the GCC source repository from [UK GCC Mirror](ftp://ftp.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/) or go to the official [releases](https://gcc.gnu.org/releases.html)

Now that you have the source downloaded in `Downloads/gcc-version/`, create a new directory where you wish to configure the source for building, e.g. `Downloads/avr-gcc-version-build/` would be suitable.

Next, using the command below (modify the `../gcc-10.1.0/configure` to your path), configure the source for building

```bash
../gcc-10.1.0/configure -v --enable-languages=c,c++ --prefix=/usr/lib --infodir=/usr/share/info --mandir=/usr/share/man --bindir=/usr/bin --libexecdir=/usr/lib --libdir=/usr/lib --enable-shared --with-system-zlib --enable-long-long --enable-nls --without-included-gettext --disable-libssp --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=avr CFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-q39GPj/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat ' CPPFLAGS='-Wdate-time -D_FORTIFY_SOURCE=2' CXXFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-q39GPj/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat ' FCFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-q39GPj/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong' FFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-q39GPj/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong' GCJFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-q39GPj/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong' LDFLAGS='-Wl,-Bsymbolic-functions -Wl,-z,relro' OBJCFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-q39GPj/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat ' OBJCXXFLAGS='-g -O2 -fdebug-prefix-map=/build/gcc-avr-q39GPj/gcc-avr-5.4.0+Atmel3.6.1=. -fstack-protector-strong -Wformat '
```

Once that has run successfully, run `make` to build GCC. If you wish to also install you may optionally run `make install`. For more information please reference the official build/install guide.