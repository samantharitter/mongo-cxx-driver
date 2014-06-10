# Overview

In the legacy-0.9 development cycle, the mechanism by which the driver is configured was refactored. Note that these changes *do not apply to releases on the 26compat branch, or releases pre-dating legacy-0.9.0*.This page describes the new configuration mechanism and documents the various configuration options that may be used.

# Configuring the Driver

## The `mongo::client::initialize` function

The `mongo::client::initialize` function, found in file `src/mongo/client/init.h` configures the driver and starts background threads critical to the functioning of the driver. The function has a single parameter receiving a `mongo::client::Options` object. If no value is provided, a default constructed `mongo::client::Options` object is used.

To configure the driver to use non-default parameters, construct a new `mongo::client::Options` object and use its setter methods to configure the parameters of interest, and pass this object to `mongo::client::initialize`.

It is strongly recommended that you invoke `mongo::client::initialize` as early as possible in your application startup phase, preferably before any additional threads have been created.

## The `mongo::client::terminate` function

When it is time to stop using the client driver, you may terminate its background tasks and release resources by invoking the `mongo::client::terminate` function, found in file `src/mongo/client/init.h`. However, please note that by default, you do not need to do so, since the behavior of `mongo::client::initialize` with a default constructed `mongo::client::Options` object is to organize an automatic invocation of `mongo::client::terminate` by way of `atexit`.

The `mongo::client::terminate` function takes a grace period argument, specified in milliseconds. The shutdown routine will give background tasks the selected grace period to terminate cleanly. If they do not do so, `mongo::client::terminate` will return an error status. If the returned error status is `ExceededTimeLimit` it is safe to retry the call to `mongo::client::terminate`. Otherwise, a non-OK return status from `mongo::client::terminate` represents a permanent failure to shut down the driver cleanly. Please see the documentation for `mongo::client::terminate` for additional details.

If you would prefer to explicitly manage the lifetime of the driver and not rely on `atexit`, you may pass a `mongo::client::Options` object customized by invoking `Options::setCallShutdownAtExit(false)`. It is then the responsibility of the application code to make an appropriate call to `mongo::client::terminate`.

## The `mongo::client::Options` class

### Where to find it

Configuration of the driver is managed through a new class, added in the `legacy-0.9.0` release, called `Options`. This class is hosted in the `mongo::client` namespace, and is defined in the header `mongo/client/options.h`.

### Passing an instance to `mongo::client::initialize`

By default, calling `mongo::client::initialize()` with no parameters is equivalent to calling `mongo::client::initialize(mongo::client::Options())` passing a default constructed `mongo::client::Options` object.

Because the `mongo::client::Options` class has setters returning a `mongo::client::Options&` you can chain together options to easily configure them at the call site:

```
using mongo::client::initialize;
using mongo::client::Options;

// Configure the mongo C++ client driver, enabling SSL and setting
// the SSL Certificate Authority file to 'mycafile'.
Status status = initialize(Options().setSSLMode(Options:kSSLModeRequired).setSSLCAFile('mycafile'));
if !(status.isOK()) {
    // deal with errors
} else {
    // Driver is up in SSL mode.
}

```
### Available Options

#### `Options::callShutdownAtExit` and `Options::setCallShutdownAtExit`

- Type: `bool`
- Default: `true`
- Semantics: If this option is 'true', then a successful call to `mongo::client::initialize` will schedule a call to `mongo::client::terminate` with `atexit`. The call to `mongo::client::terminate` will be made with the value of `mongo::client::Options::current::autoShutdownGracePeriodMillis`


#### `Options::autoShutdownGracePeriodMillis` and `Options::setAutoShutdownGracePeriodMillis`

- Type: `int`, interpreted as milliseconds
- Default: 250
- Semantics: If `mongo::client::initialize` scheduled a call to `mongo::client::terminate` with `atexit`, then that call to `mongo::client::terminate` will use the value `Options::autoShutdownGracePeriodMillis` when calling `mongo::client::terminate`.


#### `Options::setDefaultLocalThresholdMillis` and `Options::defaultLocalThresholdMillis`

- Type: `int`, interpreted as milliseconds
- Default: 15
- Semantics: TODO

#### `Options::setSSLMode` and `Options::SSLMode`

- Type: [`Options::kSSLEnabled`|`Options:kSSLDisabled`]
- Default: `Options::kSSLDisabled`
- Semantics: If set to `Options:kSSLEnabled` the driver will require SSL connections to all mongo servers. If disabled, it will not request SSL. Note that if the servers you are connected to are in SSL required mode, you may not be able to connect. This value is an enumeration so that we may later extend it with a `kSSLPreferred` option, but that is not currently implemented.

#### `Options::setFIPSMode` and `Options::FIPSMode`

- Type: `bool`
- Default: `false`
- Semantics: If true, will attempt to use FIPS-140 validated crypto if supported by the crypto library currently in use.

#### `Options::setSSLCAFile` and `Options::SSLCAFile`

- Type: `std::string`
- Default: `""`
- Semantics: This flag only has an effect if `Options::current::SSLMode` is `Options::kSSLRequired`. If set, it specifies a file containing the certificate authority file to use. See the [MongoDB SSL documentation](http://docs.mongodb.org/manual/tutorial/configure-ssl/#set-up-mongod-and-mongos-with-ssl-certificate-and-key) for additional information on the CA file.

#### `Options::setSSLPemKeyFile` and `Options::SSLPEMKeyFile`

- Type: `std::string`
- Default: `""`
- Semantics: This flag only has an effect if `Options::current::SSLMode` is `Options::kSSLRequired`. If set, it specifies a file containing the SSL PEM key file to use. See the [MongoDB SSL documentation](http://docs.mongodb.org/manual/tutorial/configure-ssl/#set-up-mongod-and-mongos-with-ssl-certificate-and-key) for additional information on the PEM key file.

#### `Options::setSSLPemKeyPassword` and `Options::SSLPEMKeyPassword`

- Type: `std::string`
- Default: `""`
- Semantics: This flag only has an effect if `Options::current::SSLMode` is `Options::kSSLRequired`, and is only meaningful if a PEM key file has been set with `Options::setSSLPEMKeyFile`. If set, it specifies the password to be used to decrypt the SSL PEM key file specified with `Options::setSSLPEMKeyFile`. See the [MongoDB SSL documentation](http://docs.mongodb.org/manual/tutorial/configure-ssl/#set-up-mongod-and-mongos-with-ssl-certificate-and-key) for additional information on the PEM key file password.

#### `Options::setSSLCRLFile` and `Options::SSLCRLFile`

- Type: `std::string`
- Default: `""`
- Semantics: This flag only has an effect if `Options::current::SSLMode` is `Options::kSSLRequired`. If set, it specifies the file to use as the SSL certificate revocation list. See the [MongoDB SSL documentation](http://docs.mongodb.org/manual/tutorial/configure-ssl/#set-up-mongod-and-mongos-with-ssl-certificate-and-key) for additional information on the certificate revocation list file.

#### `Options::setSSAllowInvalidCertificates` and `Options::SSLAllowInvalidCertificates`

- Type: `bool`
- Default: `false`
- Semantics: This flag only has an effect if `Options::current::SSLMode` is `Options::kSSLRequired`. Setting this option to `true` suppresses validation of certificates. In other words, invalid certificates will be accepted.

#### `Options::setValidateObjects` and `Options::validateObjects`

- Type: `bool`
- Default: `false`
- Semantics: If enabled, the client library will run BSON validation on data returned from the server to ensure that the returned data is valid BSON. Note that there is a performance cost to doing so.

## Caveats

- You must call `mongo::client::initialize` before using the driver. You may only call `mongo::client::initialize` once.

- Configuration of the driver is global. You may access the global configuration state of the driver by calling `mongo::client::Options::current`. If called before entering `main`, the values returned by `Options::current` are indeterminate. If called after `main` but before calling `mongo::client::initialize`, a default constructed instance of the Options class will be returned. If called after `mongo::client::initialize`, the value returned by `Options::current` will reflect any customized `Options` instance passed to `mongo::client::initialize`.

- Configuration of the driver is not synchronized, and you may only invoke `mongo::client::initialize` once. We strongly recommend that you call `mongo::client::initialize` as early as possible in `main` or your application startup code, preferably before creating any additional threads.





