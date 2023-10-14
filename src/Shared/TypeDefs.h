#ifndef TYPEDEFS_H
#define TYPEDEFS_H


typedef unsigned long long uint64;
typedef signed long long int64;
typedef unsigned long uint32;
typedef signed long int32;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned char uint8;
typedef signed char int8;

#include <string>
typedef std::string string;
typedef std::stringstream sstream;

#include <list>
#define List std::list
#ifdef __MINGW32__
#include <tr1\unordered_map>
#define HashMap std::tr1::unordered_map
#else
#include <unordered_map>
#define HashMap std::unordered_map
#endif
#include <map>
#define Map std::map
#include <vector>
#define Vector std::vector
#define AutoPtr std::auto_ptr

typedef uint16 ItemID;
typedef uint64 ItemUID;
typedef uint32 NpcID;

#endif // TYPEDEFS_H
