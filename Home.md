The C++ driver builds successfully on Linux, Mac OS X, Windows, FreeBSD and Solaris.

The MongoDB C++ driver library includes a bson package that implements the BSON specification (see http://www.bsonspec.org). This library can be used standalone for object serialization and deserialization even when one is not using MongoDB at all.

### Getting Started
 - [Download and Compile](Download and Compile)
 - [Getting Started with the C++ Driver](Tutorial)
 - [BSON Helper Functions](BSON Helper Functions)

### Documentation
 - [API Documentation](http://api.mongodb.org/cxx/)
 - [SQL to Mongo Shell to C++](SQL to Shell to CPP)
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