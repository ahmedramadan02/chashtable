//The main file
#include <stdio.h>
#include "CHashTable.h"
#include "UTest/UTest.h"
#include "UTLs/HashFunctions.h"
#include <limits.h>

void Display(const CHashTable_T*);

int main(int argc, char **argv) {

	//Init a new test
	UTEST_init( ":::: CHASH Test :::" );

	//Arrange
	CHashTable_T* ptable = NULL;
	CHashTable_T table, table2;
	KeyValue_Pair_T* pPairTemp = NULL;
	const uint32_t invalidSize = UINT16_MAX + 1;

	//Expected
	const int expectedSize = 50;

	//CreateNew
	CHASH_CreateNew( &table, (size_t) expectedSize );
	UTEST_testcase( "#01 CreateNew - CheckSize [ShouldWork] ", AssertEquals(table.size, expectedSize) );
	UTEST_testcase( "#02 CreateNew - PassInvalidSize [ShouldFail]", AssertEquals( CHASH_CreateNew( &table2, invalidSize ) , CHASH_FAILED ) ); //ShouldFix03: Test case failed, because the handling is to set the value to MAX_Size
	UTEST_testcase( "#03 CreateNew - PassZero [ShouldFail]", AssertEquals( CHASH_CreateNew( &table2, 0 ) , CHASH_FAILED ) );

	//AddNew
	UTEST_testcase( "#04 AddNew - SendEmptyPointer [ShouldFail] ", AssertIsNULL( (KeyValue_Pair_T*) CHASH_AddNew(ptable, 500, "Key500") ) );

	CHASH_AddNew(&table, 156, "Hello");
	CHASH_AddNew(&table, 156, "World!");
	uint16_t hash = GetHash( 156, table.size );
	uint16_t expectedCount = 2;
	uint16_t expectedLoadFactor = 25;

	UTEST_testcase( "#05 AddNew - Add correct value [ShouldWork] ", AssertIsStrSame ( table._nodes[hash].value, "Hello" ) );
	UTEST_testcase( "#06 AddNew - Check the count [ShouldFail]", AssertEquals( table.count , expectedCount ) ); //failed, FIXED
	UTEST_testcase( "#15 AddNew - Check the loadfactor [ShouldFail]", AssertEquals( CHASH_GetLoadFactor(&table) , expectedLoadFactor ) ); //failed, temporary fix


	CHASH_AddNew(&table, 50, "Check"); //50 is the size of the array
	CHASH_AddNew(&table, 0, "ActualCollision");

#ifdef QUADRATIC_PROBING
	UTEST_testcase( "#07 AddNew - CheckCollisionHappended [ShouldWork] ", AssertIsStrSame ( table._nodes[4].value, "ActualCollision" ) ); //4 because we use QUADRATIC_PROBING
#endif

#ifdef LINEAR_PROBING
	UTEST_testcase( "#08 AddNew - CheckCollisionHappended [ShouldWork] ", AssertIsStrSame ( table._nodes[1].value, "ActualCollision" ) );
#endif

	//Find
	int findkey = 0;
	pPairTemp = CHASH_Find(&table, findkey);
	const char* findExpected = "ActualCollision";
	UTEST_testcase( "#09 Find - CheckTofind [ShouldWork] ", AssertIsStrSame ( pPairTemp->value, findExpected ) );

	//Update & Delete
	//Arrange delete
	const int countBeforeDelete = table.count;

	CHASH_Update(&table, 0, "World");
	CHASH_Delete(&table, 50);


#ifdef QUADRATIC_PROBING
	UTEST_testcase( "#10 Update - Update correct value [ShouldWork] ", AssertIsStrSame ( table._nodes[4].value, "World" ) ); //4 because we use QUADRATIC_PROBING
#endif

	UTEST_testcase( "#11 Delete - Check the functionality [ShoulWork] ", AssertEquals( table._nodes[0].key, CHASH_INVALID ) );
	UTEST_testcase( "#16 Delete - Check the count [ShouldWork]", AssertEquals( table.count , countBeforeDelete - 1 ) );

	CHashTable_T dest;
	CHASH_Copy(&table,&dest);
	UTEST_testcase( "#12 Copy - Check if the values copied [ShouldWork] ", AssertIsStrSame( dest._nodes[4].value, "World" ) );

	//Get Report
	UTEST_report();

	//Display(&dest);

	CHASH_ReleaseObj(&table);
	return 0;
}


void Display(const CHashTable_T* ptable){
	if(ptable != NULL){
		KeyValue_Pair_T* pPairTemp;
		for(int i = 0; i < ptable->size ; i++) {
			pPairTemp = (KeyValue_Pair_T*) &(ptable->_nodes[i]);

			printf("Key %u Value: %s \n",pPairTemp->key, pPairTemp->value);
		}
	}
}
