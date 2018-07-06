#include "common.h"
#include "HashFunctions.h" //depends on common.h

//PRIVATE FUCNTIONS
PRIVATE uint16_t hash(uint16_t key, size_t size );

//GetHashFunction
PUBLIC uint16_t GetHash(uint16_t _key, size_t _size) { return hash(_key, _size); }

//Simple hash function
#ifdef SIMPLE_HASH
PRIVATE uint16_t hash( uint16_t key, size_t size ){
	return (uint16_t) ( key % size );
}
#endif

//Open Addressing
//Linear Probing: f(i) = i
#ifdef LINEAR_PROBING
PRIVATE uint16_t hash( uint16_t key, size_t size ){
	return (uint16_t) ( key % size );
}
#endif

//Quadratic Probing: f(i) = i * i
#ifdef QUADRATIC_PROBING
PRIVATE uint16_t hash( uint16_t key, size_t size ){
	return (uint16_t) ( key % size );
}
#endif

//Double Hashing: f(i) = i * hash2(elem)
#ifdef DOUBLE_HASHING
PRIVATE uint16_t hash( uint16_t key, size_t size ){
	return (uint16_t) ( key % size );
}
#endif

//Hash string
#ifdef STR_HASH
PRIVATE uint16_t hash( uint16_t key, size_t size ){
	return 0;
}
#endif

//Crypto hash
#ifdef CRYPTO_HASH
PRIVATE uint16_t hash( uint16_t key, size_t size ){
	return 0;
}
#endif
