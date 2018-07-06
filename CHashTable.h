
#ifndef CHASHTABLE_H_
#define CHASHTABLE_H_
//Defines
#define _TABLE_MAX_SIZE 		(65535 - 1)
#define CHASH_INVALID 			0xFFFF
#define CHASH_STR_NULL 			""
#define CHASH_FAILED 			-1
#define CHASH_SUCCESS			1
#define CHASH_EMPTY 			0
#define LOADFACTOR_LIMIT		80	//to be configured

//includes
#include "UTLs/common.h"

//The hashTable main datastructure
typedef struct pair
{
	uint16_t key;
	char* value;
	struct pair* anotherNode; //use as the linked-list of the data collision
}KeyValue_Pair_T;

typedef struct
{
	size_t size;
	uint16_t count;
	KeyValue_Pair_T* _nodes;
}CHashTable_T;

//Create a immutable hashtable, mutable is dangerous as the key will change
//the basic operations is insert, update, delete, add find
PUBLIC int CHASH_CreateNew(CHashTable_T* ,size_t  _size);
PUBLIC KeyValue_Pair_T* CHASH_AddNew(CHashTable_T*, const uint16_t , char*);
PUBLIC int CHASH_Update(CHashTable_T*, const uint16_t , char*);
PUBLIC int CHASH_Delete(CHashTable_T*, const uint16_t);
PUBLIC KeyValue_Pair_T* CHASH_Find(const CHashTable_T*, const uint16_t);
PUBLIC size_t CHASH_Size( const CHashTable_T* );
PUBLIC uint8_t CHASH_GetLoadFactor( const CHashTable_T* );
PUBLIC void CHASH_ReleaseObj(CHashTable_T*);
PUBLIC void CHASH_Copy(const CHashTable_T*, CHashTable_T*);
PUBLIC void CHASH_Move(CHashTable_T*, CHashTable_T*);

#endif /* CHASHTABLE_H_ */
