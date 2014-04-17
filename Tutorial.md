## BSON

#### Working with BSON

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
