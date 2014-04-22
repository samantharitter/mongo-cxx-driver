## Getting started with the C++ Driver

This is an introduction to usage of the MongoDB database from a C++ program.

First, install MongoDB – see the [installation](http://docs.mongodb.org/manual/installation/) page for details.

Next, you may wish to take a look at the [MongoDB Manual](http://docs.mongodb.org/manual/) for a language independent look at how to use MongoDB. Also, we suggest some basic familiarity with the [mongo shell](http://docs.mongodb.org/manual/mongo/) – the shell is the primary database administration tool and is useful for manually inspecting the contents of a database after your C++ program runs.

## Installing the Driver Library and Headers

Please see [download and compile page](Download and Compile) for instructions on how to download, build, and install the C++ client driver.

## Building projects with the C++ driver

The C++ driver utilizes several Boost libraries. Be sure they are in your include and lib paths. You can usually install them from your OS’s package manager if you don’t already have them. We recommend using Boost 1.49.

## BSON

The MongoDB database stores data in BSON format. BSON is a binary object format that is JSON-like in terms of the data which can be stored (some extensions exist, for example, a Date datatype).

To save data in the database we must create objects of class BSONObj. The components of a BSONObj are represented as BSONElement objects. We use BSONObjBuilder to make BSON objects, and BSONObjIterator to enumerate BSON objects.

#### Working with BSON

Let’s now create a BSON “person” object which contains name and age. We might invoke:

```cpp
BSONObjBuilder b;
b.append("name", "Joe");
b.append("age", 33);
BSONObj p = b.obj();
```

Or more concisely:

```cpp
BSONObj p = BSONObjBuilder().append("name", "Joe").append("age", 33).obj();
```

We can also create BSON objects using the stream oriented syntax:

```cpp
BSONObjBuilder b;
b << "name" << "Joe" << "age" << 33;
BSONObj p = b.obj();
```

The BSON Macro lets us be even more compact:

```cpp
BSONObj p = BSON( "name" << "Joe" << "age" << 33 );
```

Use the GENOID helper to add an object id to your object. The server will add an _id automatically if it is not included explicitly.

```cpp
BSONObj p = BSON( GENOID << "name" << "Joe" << "age" << 33 );
// result is: { _id : ..., name : "Joe", age : 33 }
```

GENOID should be at the beginning of the generated object. We can do something similar with the non-stream builder syntax:

```cpp
BSONObj p = BSONObjBuilder().genOID().append("name","Joe").append("age",33).obj();
```

Other helpers are listed [BSON Helpers](BSON Helper Functions).

## Using the driver

```cpp
#include "mongo/bson/bson.h"
#include "mongo/client/dbclient.h"
```

#### Making a Connection
```cpp
using namespace mongo;
DBClientConnection conn;
conn.connect("localhost");
```

#### Using the Connection Pool
```cpp
ScopedDbConnection conn("localhost");

// The scoped connection can be used like a pointer to a connection
conn->query();
// etc

// When finished with the connection you must call done
conn.done();
```

#### Inserting a Document
```cpp
conn.insert(
    "test.test",
    BSON(
        "name" << "Tyler" <<
        "age" << 29 <<
        "awesome" << true
    )
);
```

#### Querying for a document
#### Updating a document
#### Removing a document