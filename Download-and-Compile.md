### Prerequisites
 - [Boost](http://www.boost.org/) (>= 1.49)
 - [Python](https://www.python.org/) (2.x)
 - [Scons](http://www.scons.org/)

### Getting the Source Code

```sh
git clone git@github.com:mongodb/mongo-cxx-driver.git
git checkout legacy # if you want the legacy driver
```

### Compile the Driver

From the directory where you cloned the code, compile the C++ driver by running the scons command. Use the scons options described in this section.

To see the list of all SCons options, run: `scons --help`

#### SCons Options when Compiling the C++ Driver
Select options as appropriate for your environment.

##### Client Options
 - `--prefix=<path>` The directory prefix for the installation directory. Set <path> to the directory where you want the build artifacts (headers and library files) installed. For example, you might set <path> to /opt/local, /usr/local, or $HOME/mongo-client-install.
 - `--ssl` Enables SSL support. You will need a compatible version of the SSL libraries available.
 - `--use-sasl-client` Enables SASL, which MongoDB uses for the Kerberos authentication available on MongoDB Enterprise. You will need a compatible version of the SASL implementation libraries available.
 - `--sharedclient` Builds a shared library version of the client driver alongside the static library. If applicable for your application, prefer using the shared client.
 - `--extrapath=<path-to-boost>` Specifies the path to your Boost libraries if they are not in a standard search path for your toolchain.
install-mongoclient. This is the build target.

##### Build Options
 - `--dbg=[on|off]` Enables runtime debugging checks. Defaults to off. Specifying --dbg=on implies --opt=off unless explicitly overridden with --opt=on.
 - `--opt=[on|off]` Enables compile-time optimization. Defaults to on. Can be freely mixed with the values for the --dbg flag.

##### Compiler Options
 - `--cc` The compiler to use for C. Use the following syntax: `--cc=<path-to-c-compiler>`
 - `--cxx` The compiler to use for C++. Use the following syntax: `--cxx=<path-to-c++-compiler>`

##### Windows Options (Windows Only)
 - `--dynamic-windows` By default, on Windows, compilation uses /MT. Use this flag to compile with /MD. Note that /MD is required to build the shared client on Windows. Also note that your application compiler flags must match. If you build with --dbg=on, /MTd or /MDd will be used in place of /MT or /MD, respectively.

##### Deprecated Options (26Compat Branch Only)
 - `--full` Enables the “full” installation, directing SCons to install the driver headers and libraries to the prefix directory.
 - `--use-system-boost` This is strongly recommended. This builds against the system version of Boost rather than the MongoDB vendor copy. If your Boost libraries are not in a standard search path for your toolchain, include the --extrapath option, described next.

> **Note:** In the legacy release stream of the driver these options are implied, you always build using system boost and with the full installation (if you provide a prefix).

#### Additional Windows Considerations
When building on Windows, use of the SCons `--dynamic-windows` option can result in an error unless all libraries and sources for the application use the same runtime library. This option builds the driver to link against the dynamic windows libraries instead of the static windows runtime libraries. If the Boost library being linked against is expecting an /MT build (static libraries), this can result in an error similar to the following:

```
error LNK2005: ___ already defined in msvcprt.lib(MSVCP100.dll) libboost_thread-vc100-mt-1_42.lib(thread.obj)
```

You may want to define _CRT_SECURE_NO_WARNINGS to avoid warnings on use of strncpy and such by the MongoDB client code.

Include the WinSock library in your application: Linker ‣ Input ‣ Additional Dependencies. Add ws2_32.lib.

### Example C++ Driver Compilations

The following are examples of building the C++ driver.

The following example installs the driver to $HOME/mongo-client-install:

```sh
scons --prefix=$HOME/mongo-client-install install-mongoclient
```

To enable SSL, add the --ssl option:
```sh
scons --prefix=$HOME/mongo-client-install --ssl install-mongoclient
```

To enable SASL support for use with Kerberos authentication on MongoDB Enterprise, add the --use-sasl-client option:
```sh
scons --prefix=$HOME/mongo-client-install --use-sasl-client install-mongoclient
```

To build a shared library version of the driver, along with the normal static library, use the --sharedclient option:
```sh
scons --prefix=$HOME/mongo-client-install --sharedclient install-mongoclient
```

To use a custom version of boost installed to /dev/boost, use the --extrapath=<path-to-boost> option:
```sh
scons --prefix=$HOME/mongo-client-install --extrapath=/dev/boost install-mongoclient
```

To build a version of the library with debugging enabled, use --dbg=on. This turns off optimization, which is on by default. To enable both debugging and optimization, pass --dbg=on --opt=on:
```sh
scons --prefix=$HOME/mongo-client-install --dbg=on --opt=on install-mongoclient
```

To override the default compiler to a newer GCC installed in /opt/local/gcc-4.8, use the --cc and --cxx options:
```sh
scons --prefix=$HOME/mongo-client-install --cc=<path-to-gcc> --cxx=<path-to-g++> install-mongoclient
```
To build as a DLL on Windows:

**New in version 2.5.5**

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

The following example issues the build command in a PowerShell:
```sh
scons
    --64
    --sharedclient
    --dynamic-windows
    --release
    --prefix="%HOME%\mongo-client-install"
    --cpppath="C:\local\boost_1_55_0\include"
    --libpath="C:\local\boost_1_55_0\lib64-msvc-12.0"
    install-mongoclient
```

**Windows Notes**
 - You must configure Visual Studio for release builds when building your application with the C++ driver DLL.
 - Disable STL iterator debugging to ensure compatibility with the STL binary.