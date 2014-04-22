The C++ driver builds successfully on Linux, Mac OS X, Windows, FreeBSD and Solaris.

The MongoDB C++ driver library includes a bson package that implements the BSON specification (see http://www.bsonspec.org). This library can be used standalone for object serialization and deserialization even when one is not using MongoDB at all.

### Getting Started
 - [Download and Compile](Download and Compile)
 - [Getting Started with the C++ Driver](Tutorial)
 - [BSON Helper Functions](BSON Helper Functions)


###
API Documentation
Create Tailable Cursor
SQL to mongo Shell to C++
C++ BSON Library

Include bson/bson.h in your application. See the bsondemo.cpp for example usage.

Key classes:

mongo::BSONObj (aka bson::bo): a BSON object
mongo::BSONElement (bson::be): a single element in a BSON object. This is a key and a value.
mongo::BSONObjBuilder (bson::bob): used to make BSON objects
mongo::BSONObjIterator (bson::bo::iterator): used to enumerate BSON objects
See BSON examples in the Getting Started guide

Standalone Usage
You can use the C++ BSON library without MongoDB. Most BSON methods under the bson/ directory are header-only. They require boost, but headers only.

See the bsondemo.cpp example at github.com

API Documentation
http://api.mongodb.org/cplusplus
Short Class Names
Add

using namespace bson;
to your code to use the following shorter names for the BSON classes:

// from bsonelement.h
namespace bson {
    typedef mongo::BSONElement be;
    typedef mongo::BSONObj bo;
    typedef mongo::BSONObjBuilder bob;
}
(Or one could use bson::bo fully qualified for example).

Also available is bo::iterator as a synonym for BSONObjIterator.

C++ DBClientConnection

The C++ driver includes several classes for managing collections under the parent class DBClientInterface.

DBClientConnection is the normal connection class for a connection to a single MongoDB database server (or shard manager). Other classes exist for connecting to a replica set.

See http://api.mongodb.org/cplusplus for details on each of the above classes.

C++ getLastError

string mongo::DBClientWithCommands::getLastError(); Get error result from the last operation on this connection. Empty string if no error.
BSONObj DBClientWithCommands::getLastErrorDetailed(); Get the full last error object. See the getLastError Command page for details.
For an example, see client/simple_client_demo.cpp.

Also see getLastError Command.