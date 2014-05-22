# Overview

The 26compat release series tracks the server 2.6 releases one-to-one. As a result, it receives only bugfixes and small updates necessary to keep it building in isolation.

The legacy release series, on the other hand, is under active development. Our philosophy is to keep the legacy branch as close to the 26compat branch as is reasonable, but that when weighing new features against compatibility, we will choose new features. As a result the legacy branch is not 100% source compatible with the 26compat branch.

This page attempts to serve as a transition guide for those users looking to migrate from the 26compat branch to the legacy branch. Note that it does *not* discuss new features in detail and simply points to the per-release notes.

**NOTE: This is a living document, and tracks the current state of the legacy branch. While we have attempted to capture the big ticket breakages here, there are likely to be small ones that we have missed. The C++ driver developers would appreciate it greatly if, when you are moving from 26compat to legacy, you find issues not tracked here: please send us a pull request so we can keep this page up to date and useful.

**NOTE: The legacy branch is currently unstable and under active development.**

# Breaking Changes

## Changes to the build system

* The `--full` flag is no longer required, and it is an error to specify it.
* The `--d` and `--dd` flags have been removed. Use the `--opt` and `--dbg` flags instead.
* The `--use-system-boost` flag is no longer required, and it is an error to specify it.
* All ABI affecting macros are now defined in a generated `config.h` header that is automatically included from `dbclient.h` and `bson.h`.
* Many server specific build options (that were unlikely to have been used when building the driver) have been removed.
* The default installation prefix is now `build/install`, rather than `/usr/local`.
* All build artifacts are now captured under the `build` directory.

## Changes to APIs
* The `mongo::BSONBuilderBase` class has been removed and is no longer a base class of `mongo::BSONObjBuilder` or `mongo::BSONArrayBuilder`
* The `mongo::OpTime` class no longer offers the `now` and related synchronized methods.
* The `globalServerOptions` and `globalSSLOptions` objects and their classes have been removed. All driver configuration should be done through the new `mongo::client::Options` object.
* The `RamLog`, `RotatableFileAppender`, and `Console` classes have been removed from the logging subsystem.
* In addition, many auxiliary types, functions, and headers that were either unused, or minimally used, have been removed from the distribution.

## Behavior Changes
* The driver is now unlikely to function correctly unless `mongo::client::initialize` is invoked before using the driver APIs.
* The driver no longer logs any output by default. You may configure and inject a logger to re-enable logging. See `src/mongo/client/examples/clientTest.cpp` for an example of how to enable logging.

# Improvements

Please see the release notes for the individual legacy branch releases for details on improvements in each release:

* [legacy-0.9.0](https://github.com/mongodb/mongo-cxx-driver/releases/tag/legacy-0.8.0)