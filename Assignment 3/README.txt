

*******************************************  RECORD MANAGER ******************************************

The record manager handles tables with a fixed schema. 
The record manager is used to handle table which can insert records, delete records, update records, and scan through the records.
A scan is associated with a search condition and only returns records that match the search condition.

******************************************* FILE DETAILS ******************************************

We have kept the same structure of all the 20 files, with proper comments in source code files. All files names are listed below:-

- README.txt
- buffer_mgr_stat.h
- buffer_mgr_stat.c
- buffer_mgr.h
- buffer_mgr.c
- dberror.c
- dberror.h
- dt.h
- expr.c
- expr.h
- record_mgr.c
- record_mgr.h
- rm_serializer.c
- storage_mgr.c
- storage_mgr.h
- tables.h
- test_assign3_1.c
- test_expr.c
- test_helper.h
- Makefile

************************************************ ----> HOW TO RUN <---- **************************************************

Step 1: Open terminal and clone our repository from BitBucket using the terminal and keep it in your desire location.

Step 2: Go to your Step 1 location, you will get "assign3_record_manager" folder. Be right there in terminal.

Step 3: Use make command to exucute the Makefile by typing "make".

Step 4: The above steps will create an output file "test_assign3_1.exe".
        Then, run "./test_assign3_1" to execute all test cases.

-- The code executed successfully, once you see this message “OK: finished test”

Step 5: Execute the below command to check for any memory leaks.
After successful execution this command should return 0 error messages and 0 memory leaks.
"valgrind --leak-check=full --track-origins=yes ./test_assign3_1"

Step 6: For re-running the entire project, execute "make clean" which will clean the executable files generated for previous instance and repeat the steps from step 3


************************************************ ----> DESCRIPTION <---- **************************************************

We have an interface declared in buffer_mgr.h. Its implementation is written in record_mgr.c. 
The description/implementation of each fuction/method is given below:

Method 1: initRecordManager()
- This function is used to initialize the record manager if the recordInitFlag is not already initialized.


Method 2: shutdownRecordManager()
- This function is used to shutdown the record manager by resetting the recordInitFlag back to 0.


Method 3: createTable()
- Creates a table of the given name and schema provided.
- This method creates and opens the pageFile.
- It serializes the schema and then it's written to the first postion of the page file.


Method 4: openTable()
- Open a table of the given name and relation provided for any operation to be performed.
- DeSerialize the schema, and store the related info in the Table Data.
- initialize buffer pool and pin the first page using default strategy RS_FIFO.
- The schema stored as a string in the page file is deserialized in the schema structure and stored in the RM_TableData structure.


Method 5: closeTable()
- Close a table associated with the given relation after all the operations to be performed are done.
- All the memory allocations are de-allocated, so as to avoid any memory leaks.
- This function calls shutdownBufferPool() method.


Method 6: deleteTable()
- Deletes a table with the given name after all the operations to be performed are done.
- destroyPageFile() function from the StorageManger takes care of deleting the table.


Method 7: getNumTuples (RM_TableData *rel)
	this function is used to get the count total number of tuples present in the table.


Records handling in a table
----------------------------
Method 8:insertRecord (RM_TableData *rel, Record *record)
	this function is used to insert  a record passed as a parameter in the RM_TableData and assigns RID to the page.
	
	
Method 9: deleteRecord (RM_TableData *rel, RID id)
- This is for deleting the record using record id which is passed through the parameter.
- In this method, we navigate the datapointer by pin the page and set "-" which represents that particular records is more needed.
- Ultimately, we make that page dirty and then unpin it.


Method 10: updateRecord (RM_TableData *rel, Record *record)
- This function is used to update the record in the RM_TableData.
- It checks for the space to update record 
- It sets the Record ID, navigates to the location where the record's data is stored.
- Copies the new data to existing data 
- Marks the page dirty and unpins the page.


Method 11: getRecord (RM_TableData *rel, RID id, Record *record)
- This method is to retrieve the existing record from the table using record id.
- It searches for the page, where that record is located and then pins that page.
- It assigns the record id and copies the data.
- Finally unpins it.


Method 12: startScan (RM_TableData *rel, RM_ScanHandle *scan, Expr *cond)
- This "startScan" function is to initialize all attributes of RM_ScanHandle
- It checks the condition first, if it is null then RC_SCAN_CONDITION_NOT_FOUND
- Else, start scanning and intialize the variables.


Method 13: next (RM_ScanHandle *scan, Record *record)
- This method is to find the next record of the satisfied condition
- It checks the condition first, if it is null then RC_SCAN_CONDITION_NOT_FOUND
- Also, it checks if there are no tuples in the table, we return error code RC_RM_NO_MORE_TUPLES
- Finally, if the result (v.boolV) is TRUE, means the tuple satisfies the condition, unpin the page and return RC_OK


Method 14: closeScan (RM_ScanHandle *scan)
- This method is just to close the scan operation


Methos 15: getRecordSize (Schema *schema)
- The "getRecordSize" function is to retrieve the record size of specified schema
- To capture the size, we declared "recordSizeCount" variable. We iterate each datatype and keep on adding the size.


Method 16: createSchema (int numAttr, char **attrNames, DataType *dataTypes, int *typeLength, int keySize, int *keys)
- The "createSchema" function is to make a new schema and initialize its all attributes
- This creates schema and allocates the memory to it.


Method 17: freeSchema (Schema *schema)
- Function is used to free the memory allocated in a schema.


Method 18: createRecord (Record **record, Schema *schema)
- This function is used to create a record.
- Memory is allocated to the data of the record.


Method 19: freeRecord (Record *record)
- Function is used to free the memory allocated for record.


Method 20: getAttr (Record *record, Schema *schema, int attrNum, Value **value)
- Function is used to retrieving the attribute from the given record in schema.
- Retrives the attribute value depending on data type


Method 21: setAttr (Record *record, Schema *schema, int attrNum, Value *value)
- Function is used to set the attribute value in the record in schema. 
- It sets the attribute value depending on datatype


Custom Method: attrOffset()
- Function is used to calculate the offsets
- Checks for all the attributes in the schema 
- Calculates according to the datatype.

