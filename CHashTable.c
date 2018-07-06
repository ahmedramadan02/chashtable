/***************************
 * HISTORY
 * --------------
 * V0.4		Update and Delete should return the count, not PASSED and FAILED, Updated
 * V0.3 	Changed the return type of the Update function
 * V0.2  	Added the load factor (not complete)
 * V0.11	More open addressing methods has been added (Double probing, Quadratic probing)
 * V0.1 	Initial version of CHASHTable.c
 * */

#include "CHashTable.h"
#include "UTLs/HashFunctions.h"

PRIVATE bool isCollision(const CHashTable_T*, const uint16_t);
PRIVATE void ManageCollisionsAdd(CHashTable_T*, uint16_t, char* );
PRIVATE uint16_t Hash( const uint16_t, size_t);
PRIVATE int destroy( KeyValue_Pair_T*);
PRIVATE int copy(const CHashTable_T* pOriginal, CHashTable_T* pEmptyTable);
PRIVATE int reset_nodes(CHashTable_T*);
PRIVATE KeyValue_Pair_T* GetNode(const CHashTable_T*, const uint16_t);
PRIVATE KeyValue_Pair_T* OA_FindEmptySlot(const CHashTable_T*, const uint16_t, const uint32_t);
PRIVATE uint8_t get_loadfactor(const CHashTable_T*);

//Public
PUBLIC int CHASH_CreateNew(CHashTable_T* _ptable, size_t _size) {
	size_t temp_size = _size;
	if(temp_size > _TABLE_MAX_SIZE)
		temp_size = _TABLE_MAX_SIZE;

	if(temp_size != 0) {
		KeyValue_Pair_T* pKeyPair = (KeyValue_Pair_T*) malloc(sizeof(KeyValue_Pair_T) * temp_size);

		if(pKeyPair != NULL){
			_ptable->_nodes = pKeyPair;
			_ptable->size = _size;
			_ptable->count = CHASH_EMPTY; //set empty for the load factor
			reset_nodes(_ptable); //Not implemented yet, test first without it
		}else {
			return CHASH_FAILED; //FIX05: should have only one exit point
	    }
	}else{
		return CHASH_FAILED;
	}

}

PUBLIC KeyValue_Pair_T* CHASH_AddNew(CHashTable_T* ptable, const uint16_t _key, char* _value) {
	if(ptable != NULL && _key <= _TABLE_MAX_SIZE){
		if( isCollision( ptable, _key )  ) {
			ManageCollisionsAdd( ptable, _key, _value ); //FIX01: should return
			ptable->count++;
		}
		else{
			uint16_t hash = Hash(_key, ptable->size);
			ptable->_nodes[hash].key = _key;
			ptable->_nodes[hash].value = _value;
			ptable->count++; //FIX02: should be one exit point
			return &(ptable->_nodes[hash]);
		}

		//if success, update the count for the load factor
		if(get_loadfactor(ptable) >= LOADFACTOR_LIMIT) {
			//re-construct the hash table with double sized
		}
	}else{
		return NULL;
	}
	//FIX05: should have only one exit point
}

PUBLIC KeyValue_Pair_T* CHASH_Find(const CHashTable_T* ptable, const uint16_t _key){ return GetNode(ptable, _key); }

PUBLIC void CHASH_ReleaseObj(CHashTable_T* ptable){
	if(ptable != NULL){

#ifdef SIMPLE_HASH
		//Release all the linked list inside the table
		KeyValue_Pair_T* pKeyPairTemp = NULL;

		for(int i=0;i<ptable->size;i++) {
			while( ptable->_nodes[i].anotherNode != NULL ){
				pKeyPairTemp = ptable->_nodes[i].anotherNode;
				ptable->_nodes[i].anotherNode =
						ptable->_nodes[i].anotherNode->anotherNode; //NextNode
				destroy(pKeyPairTemp);
			}
		}
#endif


		destroy(ptable->_nodes);
		ptable->size = (size_t) 0;
	}
}


PUBLIC int CHASH_Update(CHashTable_T* ptable, const uint16_t _key, char* NewValue){
	KeyValue_Pair_T* pPairTemp = GetNode(ptable, _key);
	if(pPairTemp != NULL){
		pPairTemp->value = NewValue;
		return ptable->count;
	}else{
		return CHASH_FAILED;
	}
}

PUBLIC int CHASH_Delete(CHashTable_T* ptable, const uint16_t _key){
	KeyValue_Pair_T* pPairTemp = GetNode(ptable, _key);
	if(pPairTemp != NULL){
		pPairTemp->value = CHASH_STR_NULL;
		pPairTemp->key = CHASH_INVALID;
		ptable->count--;
	}else{
		return CHASH_FAILED;
	}

	return ptable->count;
}


// Privates
/***************************
 * Inputs: void
 * OUTPUT: TRUE or FLASE
 * Description: this version of copy will succeed to copy all the elements in case of open addressing
 * in closed addressing, not all the elements are  guaranteed to be copied
 * */
PUBLIC void CHASH_Copy(const CHashTable_T* pOriginal, CHashTable_T* pEmptyTable) {
	//then Copy
	copy(pOriginal, pEmptyTable);
}

PUBLIC void CHASH_Move(CHashTable_T* pSource, CHashTable_T* pDestination) {
	copy(pSource, pDestination);
	CHASH_ReleaseObj(pSource); //use of public interface!, change it later
}

// Privates
/***************************
 * Inputs: void
 * OUTPUT: TRUE or FLASE
 * Description: Private function to check if there is collision
 * */
PRIVATE bool isCollision( const CHashTable_T* ptable, const uint16_t _key ) {
	uint16_t hash = Hash(_key, ptable->size);

	if(  ( ptable->_nodes[hash].key !=  CHASH_INVALID )
			&& ( ptable->_nodes[hash].value != CHASH_STR_NULL )  ) {
		return true;
	}
	else{
		return false;
	}
}

PUBLIC uint8_t CHASH_GetLoadFactor( const CHashTable_T* ptable ) {
	return get_loadfactor(ptable);
}

// Privates
/***************************
 * Inputs: void
 * OUTPUT: TRUE or FLASE
 * Description: This function is used GetNode by passing the key
 * In case of Open Addressing, we apply an algorithm according to the type of probing
 * Instead we can loop all the value of the HashTable until we
 * find the element (not recommended as HashTable complexity in case of find is O(1), and this will lead to O(n))
 * */
PRIVATE KeyValue_Pair_T* GetNode(const CHashTable_T* ptable, const uint16_t _key){
	if(ptable != NULL && _key <= _TABLE_MAX_SIZE ){

		uint16_t hash = Hash(_key, ptable->size);

		uint32_t newHash = (uint32_t) hash;

//Manage if the Open Addressing is turned on
#ifdef LINEAR_PROBING
		uint16_t step = 0;
		newHash = hash;
		while ( ptable->_nodes[newHash].key != _key ) {
			step++;
			newHash += step;
		}
#endif

#ifdef QUADRATIC_PROBING
		uint16_t step = 1;
		newHash = hash;
		while ( ptable->_nodes[newHash].key != _key ) {
			if( step == 1 ) step++;
			else step = step * step;

			newHash = hash + step;
		}
#endif

#ifdef DOUBLE_HASHING
		uint16_t step = 1;
		while ( ptable->_nodes[newHash].key != _key ) {
			step = step * (hash*2);

			newHash = hash + step;
		}
#endif

		return &(ptable->_nodes[newHash]);
	}else {
		return NULL;
	}
}

/***************************
 * Inputs: A. ptable: Pointer to the hashtable
 * OUTPUT: void
 * Description:
 * */
PRIVATE void ManageCollisionsAdd(CHashTable_T* ptable, uint16_t _key , char* value){
#ifdef SIMPLE_HASH
	//Get the hash value
	uint16_t hash = Hash(_key, ptable->size);
	//Prepare the new Node
	KeyValue_Pair_T* pKeyPairTemp = NULL;
	pKeyPairTemp = (KeyValue_Pair_T*) malloc(sizeof(KeyValue_Pair_T));
	pKeyPairTemp->anotherNode = NULL;
	pKeyPairTemp->key = _key;
	pKeyPairTemp->value = value;

	KeyValue_Pair_T* pKeyPair_iterator = ptable->_nodes[hash].anotherNode;
	while(pKeyPair_iterator != NULL){
		pKeyPair_iterator++;
	}

	//the last node has been found
	pKeyPair_iterator = pKeyPairTemp;
#endif

#ifdef LINEAR_PROBING
	//get hash
	uint16_t hash = Hash(_key, ptable->size);
	//Quadratic Probing: f(i) = i
	const static uint16_t step = 1;

	//Add the new node
	KeyValue_Pair_T* pKeyPairTemp = OA_FindEmptySlot(ptable, hash, step);
	if(pKeyPairTemp != NULL) {
		pKeyPairTemp->key = _key;
		pKeyPairTemp->value = value;
		pKeyPairTemp->anotherNode = NULL;
	}
#endif

#ifdef QUADRATIC_PROBING
	//get hash
	uint16_t hash = Hash(_key, ptable->size);

	//Quadratic Probing: f(i) = i * i
	static uint32_t step = 1;
	if( step == 1 ) step++;
	else step = step * step;


	//Add the new node
	KeyValue_Pair_T* pKeyPairTemp = OA_FindEmptySlot(ptable, hash, step);
	if(pKeyPairTemp != NULL) {
		pKeyPairTemp->key = _key;
		pKeyPairTemp->value = value;
		pKeyPairTemp->anotherNode = NULL;
	}
#endif

#ifdef DOUBLE_HASHING
	//get hash
	uint16_t hash = Hash(_key, ptable->size);

	//Double Hashing: f(i) = i * hash2(elem)
	static uint32_t step = 1;
	step = step * (hash*2);

	//Add the new node
	KeyValue_Pair_T* pKeyPairTemp = OA_FindEmptySlot(ptable, hash, step);

	if(pKeyPairTemp != NULL) {
		pKeyPairTemp->key = _key;
		pKeyPairTemp->value = value;
		pKeyPairTemp->anotherNode = NULL;
	}
#endif

#ifdef STR_HASH

#endif

#ifdef CRYPTO_HASH

#endif
}

/***************************
 * Inputs: void
 * OUTPUT: void
 * Description:
 * 	search for empty area, and insert at the next free slot
 * 	You need also to check if Hash+Counter > the maximum table size
 * 	If yes then start searching from the beginning
 * 	TEST: test infinite loop here
 * 	add elements for the hashtable to be full, then and try to make a collision!
 * 	OA: stands for Open Addressing
 *
 * */
#ifdef OPEN_ADDRESSING
PRIVATE KeyValue_Pair_T* OA_FindEmptySlot(const CHashTable_T* ptable, const uint16_t hash, const uint32_t step) {

	uint16_t counter = hash;
	while ( ptable->_nodes[counter].key != CHASH_INVALID ) {
		if( ((counter) >= ptable->size) )
			counter = 0; //Reset the counter, and start from the beginning


		counter += step;
	}

	return &(ptable->_nodes[counter]);
}
#endif

/***************************
 * Inputs: void
 * OUTPUT: void
 * Description: Function that wrappes the hashing function
 * */
PRIVATE uint16_t Hash( const uint16_t key  , size_t size){
	return GetHash(key, size);
}

/***************************
 * Inputs: void
 * OUTPUT: void
 * Description:
 * */
PRIVATE int destroy(KeyValue_Pair_T* obj){
	if(obj!= NULL){
		free(obj);
	}
	//FIX05: should have only one exit point
}

/***************************
 * Inputs: void
 * OUTPUT: number of elements copied
 * Description:
 *
 * */
PRIVATE int copy(const CHashTable_T* pOriginal, CHashTable_T* pEmptyTable) {
	//use public interface!, change it later
	size_t size = pOriginal->size;
	CHASH_CreateNew(pEmptyTable, size);

	if( pEmptyTable != NULL )
	{
		for(uint16_t index = (uint16_t) size ; index > 0 ; index--) {
			pEmptyTable->_nodes[index].key = pOriginal->_nodes[index].key;
			pEmptyTable->_nodes[index].value = pOriginal->_nodes[index].value;
			pEmptyTable->_nodes[index].anotherNode = pOriginal->_nodes[index].anotherNode;
		}

		return size;
	}else{
		return -1;
	}
}

/***************************
 * Inputs: void
 * OUTPUT: void
 * Description: Function to init all the nodes
 * :: IMPORTANT CAUTION :: to be used with CreateNew Only
 * */
PRIVATE int reset_nodes(CHashTable_T* _ptable){
	if(_ptable != NULL){
		CHashTable_T* temp_ptable = _ptable;
		KeyValue_Pair_T* pKeyPair_iterator;

		for(int i = 0; i < _ptable->size ; i++)
		{
			pKeyPair_iterator = &(_ptable->_nodes[i]);
			pKeyPair_iterator->key = (uint16_t) CHASH_INVALID;
			pKeyPair_iterator->value = CHASH_STR_NULL;
			pKeyPair_iterator->anotherNode = NULL; //Set it to NULL
		}
	}
	//FIX05: should have only one exit point
}

PRIVATE uint8_t get_loadfactor(const CHashTable_T* ptable) {
	if(ptable->count == CHASH_EMPTY) {
		return 0; //the result will be infinite
	}else {
		//TODO: double loadfactor = (double) ( (double) ptable->count / (double) ptable->size )  ;
		uint8_t loadfactor = ptable->size / ptable->count; //FIX04: note the above TODO
		return (uint8_t) loadfactor; // it might return value more the 100%!, see the above fix
	}
}

