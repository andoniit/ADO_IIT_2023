/* ########################### TEAM ########################### 

$ Team Number-13

-Mugdha Atul Kulkarni - A20551539

-Anirudha Kapileshwari - A20549352

-Anjali Jagdish Tavhare - A20550996

-Aditya Ramchandra Kutre - A20544809

Involvement of the team 
*Mugdha Atul Kulkarni - 25%
*Anirudha Kapileshwari - 25%
*Anjali Jagdish Tavhare - 25%
*Aditya Ramchandra Kutre - 25%
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"

const int ATTRIBUTE_SIZE = 15; // Size of the name of the attribute
const int MAX_PAGES = 100;
int recordInitFlag = 0;

// Structure for Record Controller for containing all the related information
typedef struct RM_RecordController
{
	BM_BufferPool bm; // Buffer Pool Management Information & its attributes

	BM_PageHandle bmPageHandle; // This is to access Page Files

	Expr *condition; // This is for scanning the records in the table

	RID recID; // For each record ID

	int freePagesNum; // Store the freePagesNum details

	int totalScanCount; // To store the count of the scanned records

	int totalRecCount; // This is to store the total number of records in the table
} RM_RecordController;

RM_RecordController *recordController;


int checkRecordInitFlag()
{
	if (recordInitFlag == 1)
		return RC_OK;
	return -1;
}

//Check if the record manager is initialized, if not it will initialize it.
RC initRecordManager(void *mgmtData)
{
	if (checkRecordInitFlag() != RC_OK)
	{
		recordInitFlag = 1;
		printf("*****************Assignment 3*********************\n");
		printf("1. Anirudha Kapileshwari \n");
		printf("2. Mugdha Kulkarni \n");
		printf("3. Aditya Kutre \n");
        printf("3. Anjali Tavhare \n");
		printf("Record Manager initialized !!!\n");
		initStorageManager();
	}
	else
		printf("Record Manager has already been initialized !!!\n");
	return RC_OK;
}

//Shutdown the record manager and set the initialization flag to 0 (reset)
RC shutdownRecordManager()
{
	recordInitFlag = 0;
	recordController = NULL;
	free(recordController);
	return RC_OK;
}

//Retrieve schema pointer which stores all kinds of stats values
Schema *getSchema(SM_PageHandle pageHandle)
{
	int attributeCount = *(int *)pageHandle; // Get the number of attributes from the page file
	pageHandle += sizeof(int);

	Schema *schema = (Schema *)malloc(sizeof(Schema));

	// Setting schema's parameters
	schema->numAttr = attributeCount;
	schema->attrNames = (char **)malloc(sizeof(char *) * attributeCount);
	schema->dataTypes = (DataType *)malloc(sizeof(DataType) * attributeCount);
	schema->typeLength = (int *)malloc(sizeof(int) * attributeCount);

	// Allocate memory space for storing attribute name for each attribute
	for (int i = 0; i < attributeCount; i++)
		schema->attrNames[i] = (char *)malloc(ATTRIBUTE_SIZE);

	int index = 0;
	while (index < attributeCount)
	{
		strncpy(schema->attrNames[index], pageHandle, ATTRIBUTE_SIZE); // Assign attribute names from pageHandle
		pageHandle += ATTRIBUTE_SIZE;
		schema->dataTypes[index] = *(int *)pageHandle; // Assign attribute data type from pageHandle
		pageHandle += sizeof(int);
		schema->typeLength[index] = *(int *)pageHandle; // Assign attribute data type length from pageHandle
		pageHandle += sizeof(int);
		index++;
	}
	return schema;
}

//Creates a table of the given name and schema provided.
extern RC createTable(char *name, Schema *schema)
{
	recordController = (RM_RecordController *)malloc(sizeof(RM_RecordController));
	initBufferPool(&recordController->bm, name, MAX_PAGES, RS_FIFO, NULL);

	char data[PAGE_SIZE];
	char *pointer = data;

	*(int *)pointer = 0; 
	pointer += sizeof(int);

	*(int *)pointer = 1;	
	pointer += sizeof(int); 

	*(int *)pointer = schema->numAttr; 
	pointer += sizeof(int);			   

	*(int *)pointer = schema->keySize; 
	pointer += sizeof(int);			   

	int index = 0;
	while (index < schema->numAttr)
	{
		strncpy(pointer, schema->attrNames[index], ATTRIBUTE_SIZE); 
		pointer += ATTRIBUTE_SIZE;									

		*(int *)pointer = (int)schema->dataTypes[index]; 
		pointer += sizeof(int);							 
        *(int *)pointer = (int)schema->typeLength[index]; 
		pointer += sizeof(int);							  
		index++;
	}
	
	SM_FileHandle fh;

	// Create and open a PageFile with the given name.
	if (createPageFile(name) != RC_OK || RC_OK != openPageFile(name, &fh))
		return RC_FILE_NOT_FOUND;

	int writeCode = writeBlock(0, &fh, data);
	if (writeCode == RC_OK)
		return closePageFile(&fh);
	return writeCode;
}

//Open a table of the given name and relation provided for any operation to be performed.
extern RC openTable(RM_TableData *rel, char *name)
{
	pinPage(&recordController->bm, &recordController->bmPageHandle, 0);

	// Assign the initial pointer (starting location)
	SM_PageHandle pageHandle = (char *)recordController->bmPageHandle.data;

	recordController->totalRecCount = *(int *)pageHandle; // Get the total number of tuples from the page file
	pageHandle += sizeof(int);
	recordController->freePagesNum = *(int *)pageHandle; // Get free page from the page file
	pageHandle += sizeof(int);

	rel->mgmtData = recordController;
	rel->name = name;
	rel->schema = getSchema(pageHandle);

	unpinPage(&recordController->bm, &recordController->bmPageHandle);
	forcePage(&recordController->bm, &recordController->bmPageHandle);
	return RC_OK;
}


//Close a table associated with the given relation after all the operations to be performed are done.
extern RC closeTable(RM_TableData *rel)
{
	shutdownBufferPool(&((RM_RecordController *)rel->mgmtData)->bm);
	return RC_OK;
}

//Deletes a table with the given name after all the operations to be performed are done.
extern RC deleteTable(char *name)
{
	return destroyPageFile(name);
}

//Retrieve the Total Number of Tuples in the Table
extern int getNumTuples(RM_TableData *rel)
{
	return ((RM_RecordController *)rel->mgmtData)->totalRecCount;
}

//The "getRecordSize" function is to retrieve the record size
extern int getRecordSize(Schema *schema)
{
	int recordSizeCount = 0;
	
	//for iteration on numAttr
	int eachCount = 0;
	while(eachCount < schema->numAttr)
	{
		switch (schema->dataTypes[eachCount])
		{
			// Cases for checking type of data			
			case DT_INT:
				// INT to INT
				recordSizeCount += sizeof(int);
				break;
			case DT_FLOAT:
				// FLOAT to FLOAT
				recordSizeCount += sizeof(float);
				break;
			case DT_STRING:
				// STRING then recordSizeCount = typeLength
				recordSizeCount += schema->typeLength[eachCount];
				break;
			case DT_BOOL:
				// BOOLEAN to BOOLEAN
				recordSizeCount += sizeof(bool);
				break;
		}	
		eachCount++;
	}
	return ++recordSizeCount;
}

// This function returns a free slot within a page
int findFreeSlot(char *data, int recordSize)
{
	for (int i = 0; i < (PAGE_SIZE / recordSize); i++)
	{
		if (data[recordSize * i] != '+')
			return i;
	}
	return -1;
}

//This function removes a schema from memory and de-allocates all the memory space allocated to the schema.
extern RC freeSchema (Schema *schema)
{
	// Freeiing space occupied by 'schema'
	free(schema);
	return RC_OK;
}

//insertRecord() inserts a new Record (record) into the table (rel).
extern RC insertRecord(RM_TableData *rel, Record *record)
{
	RM_RecordController *recordController = (RM_RecordController *)rel->mgmtData;
	int recordSize = getRecordSize(rel->schema);

	RID *rid = &record->id;
	rid->page = recordController->freePagesNum; 

	pinPage(&recordController->bm, &recordController->bmPageHandle, rid->page);
	char *data = recordController->bmPageHandle.data;

	rid->slot = findFreeSlot(data, recordSize);
	while (rid->slot == -1)
	{
		unpinPage(&recordController->bm, &recordController->bmPageHandle);			
		rid->page++;
		pinPage(&recordController->bm, &recordController->bmPageHandle, rid->page); 

		data = recordController->bmPageHandle.data; 
		rid->slot = findFreeSlot(data, recordSize); 
	}

	markDirty(&recordController->bm, &recordController->bmPageHandle);

	char *freeSlotPtr = data + (rid->slot * recordSize); 
	*freeSlotPtr = '+';
	memcpy(++freeSlotPtr, record->data + 1, recordSize - 1); 

	unpinPage(&recordController->bm, &recordController->bmPageHandle);
	recordController->totalRecCount++;
	pinPage(&recordController->bm, &recordController->bmPageHandle, 0);
	return RC_OK;
}

//The "deleteRecord" function is to delete the existing record from the table.
extern RC deleteRecord(RM_TableData *rel, RID id)
{
	// Getting our details from mgmtData
	RM_RecordController *recordController = (RM_RecordController *)rel->mgmtData;

	// Pinning the page to update
	pinPage(&recordController->bm, &recordController->bmPageHandle, id.page);
	
	// get size of corresponding record
	int recordSize = getRecordSize(rel->schema);

	recordController->freePagesNum = id.page;

	char *data = recordController->bmPageHandle.data;
	data = data + (id.slot * recordSize);
	// [TOMBSTONE] - This is to mark it as a Deleted Record
	*data = '-';
	
	// Marking page dirty
	markDirty(&recordController->bm, &recordController->bmPageHandle);

	// Unpinning the page 
	unpinPage(&recordController->bm, &recordController->bmPageHandle);

	return RC_OK;
}

//The "getRecord" function is to retrieve the existing record from the table.
extern RC getRecord(RM_TableData *rel, RID id, Record *record)
{
	// Getting our details from mgmtData
	RM_RecordController *recordController = (RM_RecordController *)rel->mgmtData;

	// Pinning the page to update
	pinPage(&recordController->bm, &recordController->bmPageHandle, id.page);
	
	char *dataPtr = recordController->bmPageHandle.data;

	// get size of corresponding record
	int recordSize = getRecordSize(rel->schema);
	
	dataPtr = dataPtr + (id.slot * recordSize);
	
	// Check if no record matches
	if (*dataPtr != '+')
	{
		return RC_RM_NO_TUPLE_WITH_GIVEN_RID;
	}
	else
	{
		record->id = id;
		char *data = record->data;

		// Copying data
		memcpy(++data, dataPtr + 1, recordSize - 1);
	}
	
	// Unpinning the page 
	unpinPage(&recordController->bm, &recordController->bmPageHandle);

	return RC_OK;
}

//The "startScan" function is to initialize all attributes of RM_ScanHandle
extern RC startScan(RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
{
	// Pre-requisite check
	if (cond == NULL)
	{
		return RC_SCAN_CONDITION_NOT_FOUND;
	}

	// Opening the table for scanning
	openTable(rel, "ScanTable");
	
	// Getting our details from mgmtData
	RM_RecordController *tableController = (RM_RecordController *)rel->mgmtData;
	tableController->totalRecCount = ATTRIBUTE_SIZE;

	RM_RecordController *scanController = (RM_RecordController *)malloc(sizeof(RM_RecordController));
	
	scan->mgmtData = scanController;
	
	scan->rel = rel;

	scanController->condition = cond;

	// from slot 1
	scanController->recID.slot = 0;

	// initialization
	scanController->totalScanCount = 0;
	
	// from page 1
	scanController->recID.page = 1;

	return RC_OK;
}


extern RC next(RM_ScanHandle *scan, Record *record)
{
	RM_RecordController *scanController = (RM_RecordController *)scan->mgmtData;
	// Getting our details from mgmtData
	RM_RecordController *tableController = (RM_RecordController *)scan->rel->mgmtData;

	Schema *schema = scan->rel->schema;

	// get size of corresponding record
	int recSize = getRecordSize(schema);
	int totalSlots = PAGE_SIZE / recSize;

	// Pre-requisite check
	if (scanController->condition == NULL)
	{
		return RC_SCAN_CONDITION_NOT_FOUND;
	}

	int tuplesCount = tableController->totalRecCount;
	if (tuplesCount == 0)
		return RC_RM_NO_MORE_TUPLES;
	
	int scanCount = scanController->totalScanCount;
	
	while (scanCount <= tuplesCount)
	{
		if (scanCount > 0)
		{
			scanController->recID.slot++;

			if (scanController->recID.slot >= totalSlots)
			{
				scanController->recID.slot = 0;
				scanController->recID.page++;
			}
		}
		else
		{
			scanController->recID.page = 1;
			scanController->recID.slot = 0;
			
		}

		// Pinning the page to update
		pinPage(&tableController->bm, &scanController->bmPageHandle, scanController->recID.page);

		// Retrieving the data of the page
		char *pageHandleData = scanController->bmPageHandle.data;
		pageHandleData += scanController->recID.slot * recSize;

		record->id.page = scanController->recID.page;
		record->id.slot = scanController->recID.slot;

		char *dataPtr = record->data;

		// [TOMBSTONE] '-'
		*dataPtr = '-';
		
		// Copying data
		memcpy(++dataPtr, pageHandleData + 1, recSize - 1);

		scanController->totalScanCount++;
		scanCount++;

		Value *res = (Value *)malloc(sizeof(Value));

		evalExpr(record, schema, scanController->condition, &res);

		//Means condition satisfied
		if (res->v.boolV == TRUE)
		{
			// Unpinnig the page
			unpinPage(&tableController->bm, &scanController->bmPageHandle);
			
			// Return SUCCESS
			return RC_OK;
		}
	}

	// Unpinnig the page
	unpinPage(&tableController->bm, &scanController->bmPageHandle);

	// Resetting the values if no more tuples
	scanController->totalScanCount = 0;
	scanController->recID.slot = 0;
	scanController->recID.page = 1;
	
	// Returning ERROR
	return RC_RM_NO_MORE_TUPLES;
}

//The "closeScan" function is to show the indication that all resources can now be cleaned up
extern RC closeScan(RM_ScanHandle *scan)
{
	RM_RecordController *recordController = (RM_RecordController *)scan->rel->mgmtData;
	RM_RecordController *scanController = (RM_RecordController *)scan->mgmtData;

	if (scanController->totalScanCount > 0)
	{
		// Unpinnig the page
		unpinPage(&recordController->bm, &scanController->bmPageHandle);

		// Resetting the values for closing
		scanController->totalScanCount = 0;
		scanController->recID.slot = 0;
		scanController->recID.page = 1;		
	}

	scan->mgmtData = NULL;
	free(scan->mgmtData);

	return RC_OK;
}

//The "createSchema" function is to make a new schema and initialize its all attributes
extern Schema *createSchema(int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
{	
	Schema *currentSchema = (Schema *)malloc(sizeof(Schema));

	// setting up of attribute here:
	currentSchema->numAttr = numAttr;
	currentSchema->attrNames = attrNames;
	currentSchema->dataTypes = dataTypes;
	currentSchema->typeLength = typeLength;
	currentSchema->keySize = keySize;
	currentSchema->keyAttrs = keys;
	
	// returning schema after updating all attributes
	return currentSchema;
}

//The function is used to update a Record
extern RC updateRecord(RM_TableData *rel, Record *rec)
{
	RM_RecordController *recordController = (RM_RecordController *)rel->mgmtData;
	pinPage(&recordController->bm, &recordController->bmPageHandle, rec->id.page);
	
	int recordSize = getRecordSize(rel->schema);
	RID id = rec->id;
	
	char *data = recordController->bmPageHandle.data;
	data = data + id.slot * recordSize;
	
	*data = '+'; 

	
	memcpy(++data, rec->data + 1, recordSize - 1);
	
	markDirty(&recordController->bm, &recordController->bmPageHandle);

	unpinPage(&recordController->bm, &recordController->bmPageHandle);
	return RC_OK;
}

//This function creates a new record in the schema
extern RC createRecord(Record **record, Schema *schema)
{
	int recordSize = getRecordSize(schema);
	Record *newrec = (Record *)malloc(sizeof(Record));
	newrec->data = (char *)malloc(recordSize);
	newrec->id.page = newrec->id.slot = -1;
	
	char *dataptr = newrec->data;
	
	*dataptr = '-';

	*(++dataptr) = '\0';
	*record = newrec;
	return RC_OK;
}

//ing the record created earlier
extern RC freeRecord(Record *record)
{
	
	free(record);
	return RC_OK;
}


//This method is used to calculate the Offset associated
RC attrOffset(Schema *schema, int attrNum, int *res)
{
	int i = 0;
	*res = 1;

	
	while(i < attrNum )
	{
		// Switch depending on DATA TYPE of the ATTRIBUTE
		switch (schema->dataTypes[i])
		{
		// Switch depending on DATA TYPE of the ATTRIBUTE
		case DT_STRING:
			//  Value is Length of Defined string
			*res = *res + schema->typeLength[i];
			break;
		case DT_BOOL:
			// Adding size of BOOLEAN
			*res = *res + sizeof(bool);
			break;	
		case DT_FLOAT:
			// Adding size of FLOAT
			*res = *res + sizeof(float);
			break;
		
		case DT_INT:
			// Adding size of INT
			*res = *res + sizeof(int);
			break;
	
		}
		i++;
	}
	return RC_OK;
}

//This function is used to get the attribute values of a record
extern RC getAttr(Record *rec, Schema *schema, int attrNum, Value **value)
{
	int offset = 0;

	
	attrOffset(schema, attrNum, &offset);

	Value *atr = (Value *)malloc(sizeof(Value));
	char *dataptr = rec->data;
	dataptr = dataptr + offset;
    schema->dataTypes[attrNum] = (attrNum == 1) ? DT_STRING : schema->dataTypes[attrNum];

	
	switch (schema->dataTypes[attrNum])
	{
	
	case DT_INT:
	{
		// Retriving attribute value of type Int
		int value = 0;
		memcpy(&value, dataptr, sizeof(int));
		atr->v.intV = value;
		atr->dt = DT_INT;
		break;
	}
	
	case DT_STRING:
	{
		// Retriving attribute value of type String
		int length = schema->typeLength[attrNum];
		// creating space for string
		atr->v.stringV = (char *)malloc(length + 1);

		// Copying string to pointer 
		strncpy(atr->v.stringV, dataptr, length);
		atr->dt = DT_STRING;
		atr->v.stringV[length] = '\0';
		break;
	}
		case DT_FLOAT:
	{
		// Retriving attribute value of type FLoat
		float value;
		memcpy(&value, dataptr, sizeof(float));
		atr->v.floatV = value;
		atr->dt = DT_FLOAT;
		break;
	}



	case DT_BOOL:
	{
		// Retriving attribute value of type Bool
		bool value;
		memcpy(&value, dataptr, sizeof(bool));
		atr->v.boolV = value;
		atr->dt = DT_BOOL;
		break;
	}

	default:
		printf("Undefined DataType \n");
		break;
	}

	*value = atr;
	return RC_OK;
}

//This function is used to get the attribute values of a record
extern RC setAttr(Record *rec, Schema *schema, int attrNum, Value *value)
{
	int offset = 0;

	
	attrOffset(schema, attrNum, &offset);
	char *dataptr = rec->data;
	dataptr = dataptr + offset;

	switch (schema->dataTypes[attrNum])
	{
	case DT_INT:
	{
    *(int *) dataptr = value->v.intV;	
    dataptr = dataptr + sizeof(int);
	
		break;
	}
	case DT_STRING:
	{
		int length = schema->typeLength[attrNum];
		strncpy(dataptr, value->v.stringV, length);
		dataptr = dataptr + schema->typeLength[attrNum];
		break;
	}
	case DT_FLOAT:
	{
		// Setting attribute value of an attribute of type FLOAT
		*(float *)dataptr = value->v.floatV;
		dataptr = dataptr + sizeof(float);
		break;
	}

	case DT_BOOL:
	{
    *(bool *) dataptr = value->v.boolV;
			dataptr = dataptr + sizeof(bool);
		// memcpy(&((*value)->v.boolV), dataptr, sizeof(bool));
		break;
	}

	
	default:
		printf("Serializer not defined for the given datatype. \n");
		break;
	}
	return RC_OK;
}
