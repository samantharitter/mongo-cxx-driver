### How to ask for Help

If you are having difficulty building the driver after reading the below instructions, please email the [mongodb-user mailing list](https://groups.google.com/forum/#!forum/mongodb-user) to ask for help. Please include in your email **all** of the following information:

 - The version of the driver you are trying to build (branch or tag).
   - Examples: _26compat branch_, _legacy-0.9.0 tag_
 - Host OS, version, and architecture.
   - Examples: _Windows 8 64-bit_ x86, _Ubuntu 12.04 32-bit x86_, _OS X Mavericks_
 - C++ Compiler and version.
   - Examples: _GCC 4.8.2_, _MSVC 2013 Express_, _clang 3.4_, _XCode 5_
 - Boost version.
   - Examples: _boost 1.55_, _boost 1.49_
 - How boost was built or installed.
   - Examples: _apt-get install libboost-all-dev_, _built from source_, _windows binary install_
   - If you built boost from source, please include your boost build invocation as well.
 - The complete SCons invocation.
   - Example: _scons -j10 install-mongoclient_
 - The output of the configure phase of the build.
 - The error you encountered. This may be compiler, SCons, or other output.

Failure to include the relevant information will result in additional round-trip communications to ascertain the necessary details, delaying a useful response. Here is a made-up example of a help request that provides the relevant information:

**PLEASE NOTE: The build invocation below is incomplete and intentionally erroneous. Read the section on building against the pre-built boost binaries under the "Building on Windows" section to understand what is wrong here, and the rest of the page to learn about other important options you will want or need to use when building the driver.**

***

_I'm trying to build the legacy-0.9 tag on Windows 8 64-bit, using MSVC 2013. I have the boost 1.55 pre-built Windows binaries for VC12 installed to D:\local\boost-1.55. When I invoked scons as `scons --mute --64 --extrapath=D:\local\boost-1.55`, **the configure step will not find the boost headers**. The build gives the following configure output:_

```
Checking whether the C++ compiler works yes
Checking whether the C compiler works yes
Checking if C++ compiler "$CC" is MSVC... yes
Checking if C compiler "cl" is MSVC... yes
Checking if we are using libstdc++... no
WARNING: Cannot disable C++11 features when using MSVC
Checking if we are on a POSIX system... no
Checking for __declspec(thread)... yes
Checking for C++ header file boost/version.hpp... no
Could not find boost headers in include search path
```

_Why can't the build system find the boost headers?_

_Thanks_

***

While collecting this information will take some additional time and effort, providing it will make it much more likely for your question to receive a prompt and immediately helpful reply.

### Prerequisites
 - [Boost](http://www.boost.org/) (>= 1.49) # May work with older versions back to 1.41
 - [Python](https://www.python.org/) (2.x)
 - [SCons](http://www.scons.org/)
 - [Git](http://git-scm.com/)

### Get the Source Code

```sh
git clone git@github.com:mongodb/mongo-cxx-driver.git
```

### Choose a Branch

#### Legacy Branch:

Use the [legacy](https://github.com/mongodb/mongo-cxx-driver/tree/legacy) branch if:
 - You are experimenting with the C++ driver and do not need a production ready driver.
 - You had been using 26compat (or the driver inside of the server source) and want to benefit from incremental improvements while having the same overall API.

```
git checkout legacy
```

#### 26compat Branch:

Use the [26compat](https://github.com/mongodb/mongo-cxx-driver/tree/26compat) branch if:
 - You need a production ready release of the C++ driver that is not under active development.
 - You have existing code that used the driver from the server source and want it to continue working without modification.

```
git checkout 26compat
```

### Compile the Driver
From the directory where you cloned the code, compile the C++ driver by running the `scons` command. Use the SCons options described in this section.

To see the list of all SCons options, run: `scons --help`

#### SCons Options when Compiling the C++ Driver
Select options as appropriate for your environment.

**Important 26compat Note**: If you are using the 26compat branch, the `install-mongoclient` target is only enabled when the `--full` flag is provided. Similarly, you must use the `--use-system-boost` flag when building 26compat.

##### Targets

There are several targets you can build, but the most common target for users of the library is `install-mongoclient`, which will build the driver, and install the driver and headers to the location specified with the `--prefix` argument. If no prefix is specified, `--prefix` defaults to a directory named ```build/install``` under the current source directory.

**Important 26compat Note**: On the 26compat branch, the default argument to `--prefix` is `/usr/local`, so the build will fail unless you either build with `sudo`, or change the install prefix to a directory where you have write permissions.


##### Client Options
 - `--prefix=<path>` The directory prefix for the installation directory. Set <path> to the directory where you want the build artifacts (headers and library files) installed. For example, you might set <path> to `/opt/local`, `/usr/local`, or `$HOME/mongo-client-install`.
 - `--ssl` Enables SSL support. You will need a compatible version of the SSL libraries available.
 - `--use-sasl-client` Enables SASL, which MongoDB uses for the Kerberos authentication available on MongoDB Enterprise. You will need a compatible version of the SASL implementation libraries available.
 - `--sharedclient` Builds a shared library version of the client driver alongside the static library. If applicable for your application, prefer using the shared client.

##### Path Options
 - `--libpath=<path-to-libs>` Specifies path to additional libraries.
 - `--cpppath=<path-to-headers>` Specifies path to additional headers.
 - `--extrapath=<path-to-boost>` Specifies the path to your Boost libraries if they are not in a standard search path for your toolchain.
 - `--dllpath` Specifies the runtime search path for DLLs when running tests. Set this to the directory containing boost, ssl, or sasl DLLs as required.
   - NOTE: This is a windows-only option.
   - NOTE: This option is only available on the `legacy` branch at version legacy-0.10.0-pre or later.

##### Build Options
 - `--dbg=[on|off]` Enables runtime debugging checks. Defaults to off. Specifying `--dbg=on` implies `--opt=off` unless explicitly overridden with `--opt=on`.
 - `--opt=[on|off]` Enables compile-time optimization. Defaults to on. Can be freely mixed with the values for the `--dbg` flag.

##### Scons Options
 - `--cache` Enables caching of object files.

##### Compiler Options
 - `--cc` The compiler to use for C. Use the following syntax: `--cc=<path-to-c-compiler>`
 - `--cxx` The compiler to use for C++. Use the following syntax: `--cxx=<path-to-c++-compiler>`

##### Windows Options (Windows Only)
 - `--dynamic-windows` By default, on Windows, compilation uses `/MT`. Use this flag to compile with `/MD`. Note that `/MD` is required to build the shared client on Windows. Also note that your application compiler flags must match. If you build with `--dbg=on`, `/MTd` or `/MDd` will be used in place of `/MT` or `/MD`, respectively.

##### Mac OS X Options (Mac OS X Only)
 - `--osx-version-min=[10.7|10.8|10.9]` Minimum version of Mac OS X to build for.

##### Deprecated Options (26Compat Branch Only)
 - `--full` Enables the “full” installation, directing SCons to install the driver headers and libraries to the prefix directory. This is required when building 26compat.
 - `--use-system-boost` This is required when building 26compat, and is a vestige of a time when this code could be built either against the system boost or a private copy of boost in the repository. The driver no longer offers this built-in boost, so the use of the flag becomes mandatory. If your Boost libraries are not in a standard search path for your toolchain, include the `--extrapath` option, described next.
 - `--allocator=[system|tcmalloc]` The allocator to use. You almost certainly do *not* want to set this flag, since the choice of the allocator should be tied to an application, not to a library. This option is documented here for completeness.

Please note that there are many other flags in the build system, particularly on the 26compat branch, which are either no longer necessary or potentially actively harmful. We have removed these options on the legacy branch.

> **Note:** In the legacy release stream of the driver these options are implied, you always build using system boost and with the full installation (if you provide a prefix).

#### Windows Considerations
When building on Windows, use of the SCons `--dynamic-windows` option can result in an error unless all libraries and sources for the application use the same C runtime library. This option builds the driver to link against the dynamic link C RTL instead of the static C RTL. If the Boost library being linked against is expecting an `/MT` build (static C RTL), this can result in an error similar to the following:
```
error LNK2005: ___ already defined in msvcprt.lib(MSVCP100.dll) libboost_thread-vc100-mt-1_42.lib(thread.obj)
```
The same caveat applies to building with the --dbg=on flag, which will select the debug runtime library.

You may want to define _CRT_SECURE_NO_WARNINGS to avoid warnings on use of strncpy and such by the MongoDB client code.

Include the WinSock library in your application: Linker ‣ Input ‣ Additional Dependencies. Add ws2_32.lib.

### Example C++ Driver Compilations

The following are examples of building the C++ driver.

The following example installs the driver to `$HOME/mongo-client-install`:

```sh
scons --prefix=$HOME/mongo-client-install install-mongoclient
```

To enable SSL, add the `--ssl` option:
```sh
scons --prefix=$HOME/mongo-client-install --ssl install-mongoclient
```

To enable SASL support for use with Kerberos authentication on MongoDB Enterprise, add the `--use-sasl-client` option:
```sh
scons --prefix=$HOME/mongo-client-install --use-sasl-client install-mongoclient
```

To build a shared library version of the driver, along with the normal static library, use the `--sharedclient` option:
```sh
scons --prefix=$HOME/mongo-client-install --sharedclient install-mongoclient
```

To use a custom version of boost installed to /dev/boost, use the `--extrapath=<path-to-boost>` option:
```sh
scons --prefix=$HOME/mongo-client-install --extrapath=/dev/boost install-mongoclient
```

##### Debug Builds
To build a version of the library with debugging enabled, use `--dbg=on`. This turns off optimization, which is on by default. To enable both debugging and optimization, pass `--dbg=on --opt=on`:

```sh
scons --prefix=$HOME/mongo-client-install --dbg=on --opt=on install-mongoclient
```

To override the default compiler to a newer GCC installed in `/opt/local/gcc-4.8`, use the `--cc` and `--cxx` options:
```sh
scons --prefix=$HOME/mongo-client-install --cc=<path-to-gcc> --cxx=<path-to-g++> install-mongoclient
```

##### Building on Windows

###### Building against the pre-built boost binaries.

Building boost from source can be challenging on Windows. If appropriate for your situation, we recommend using the [pre built boost Windows binaries](http://sourceforge.net/projects/boost/files/boost-binaries/). Please note that you must select a download that properly reflects your target architecture (i.e. 32-bit or 64-bit) and toolchain revision (MSVC 10, 11, etc. Note that this is the VC version **not** the Visual Studio version).

Due to the layout of the boost installation in the pre-built binaries, you cannot use the `--extrapath` SCons flag to inform the build of the installation path for the boost binaries. Instead, you should use the `--cpppath` flag to point to the root of the chosen boost installation path, and `--libpath` to point into the appropriately named library subdirectory of the boost installation. For example, if you have installed the 64-bit boost 1.55 libraries for MSVC11 into `D:\local\boost_1_55_0_msvc11`, then you would add
```
--cpppath=d:\local\boost_1_55_0_msvc --libpath=d:\local\boost_1_55_0_msvc11\lib64-msvc-11.0
````
to your SCons invocation.

###### Building a DLL (New in version 2.5.5)
```sh
scons
    <--64 or --32>
    --sharedclient
    --dynamic-windows
    --prefix=<install-path>
    --cpppath=<path-to-boost-headers>
    --libpath=<path-to-boost-libs>
    install-mongoclient
```

###### The following example will build and install the C++ driver, in a PowerShell:
```sh
scons
    --64
    --sharedclient
    --dynamic-windows
    --prefix="%HOME%\mongo-client-install"
    --cpppath="C:\local\boost_1_55_0\include"
    --libpath="C:\local\boost_1_55_0\lib64-msvc-12.0"
    install-mongoclient
```

###### Building multiple Windows library variants:

As of legacy-0.8, the Windows libraries are now tagged with boost-like ABI tags (see http://www.boost.org/doc/libs/1_55_0/more/getting_started/windows.html#library-naming), so it is possible to build several different variants (debug vs retail, static vs dynamic runtime) and install them to the same location. We have added support for autolib, so the selection of the appropriate library is handled automatically (see https://jira.mongodb.org/browse/CXX-200). To build all of the different driver variants, repeatedly invoke scons as follows:

```
scons $ARGS install-mongoclient
scons $ARGS install-mongoclient --dbg=on
scons $ARGS install-mongoclient --dynamic-windows --sharedclient
scons $ARGS install-mongoclient --dynamic-windows --sharedclient --dbg=on
```

Where ```$ARGS``` are the arguments you would normally pass (e.g. ```--cpppath```, ```--libpath```, ```--64```, ```--prefix```, etc.). You should ensure that you use the same arguments for all four invocations. If this works properly, your ```$PREFIX/lib``` directory should contain the following files:

```
libmongoclient.lib
libmongoclient-gd.lib
libmongoclient-s.lib
libmongoclient-sgd.lib
mongoclient.dll
mongoclient.exp
mongoclient.lib
mongoclient.pdb
mongoclient-gd.dll
mongoclient-gd.exp
mongoclient-gd.lib
mongoclient-gd.pdb
``` 