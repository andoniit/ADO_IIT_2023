Jai Shree Ram !!!
################### TEAM INFO ################### 

Team Number-6

-Mugdha Atul Kulkarni - A20551539

-Anirudha Kapileshwari - A20549352

-Anjali Jagdish Tavhare - A20550996

-Aditya Ramchandra Kutre - A20544809

-Vishwas Ramakrishna - A20552892

Involvement of the team 
*Mugdha Atul Kulkarni - 20%
*Anirudha Kapileshwari - 20%
*Anjali Jagdish Tavhare - 20%
*Aditya Ramchandra Kutre - 20%
*Vishwas Ramakrishna - 20%

################### STORAGE MANAGER IMPLEMENTATION ################### 

Storage Manager: A Storage Manager is a program that provides an interface between the data stored in the database and the queries received.
It is also known as a Database Control System. It maintains the consistency and integrity of the database by applying the constraints and 
executing the DCL statements

We have implemented a module that is capable of reading blocks from a file on disk into memory and writing blocks from memory to a file on disk.

###################  LIST OF FILES AND THE FILE DETAILS ################### 

The existing files layout has been designed such that comments have been added to each section of the code to make code maintenance easier and find bugs faster.List of files names present:

- README.txt
- dberror.c
- dberror.h
- storage_mgr.c
- storage_mgr.h
- test_assign1_1.c
- test_helper.h
- Makefile

###################  LIST OF STEPS/ACTIONS TO BE RUN ################### 

Action 1: Open terminal and clone our repository from BitBucket using the terminal and keep it in your desire location.

Action 2: Go to your Action 1 location, you will get "assign1_storage_manager" folder. Be right there in terminal.

Action 3: Go to test_assign1_1.c and comment the unrequired test cases for a particular test case to be executed/tested (i.e. if initStorageManager() 

needs to be tested then the other two testCreateOpenClose() and testSinglePageContent() needs to be commented

Action 4: Use make command to execute the Makefile by typing "make".

Action 5: The above actions will create an output file "test.exe". Then, run "./test" to execute all test cases.

-- The code executed successfully, once you see this message “OK: finished test”

Action 6: Execute the below command to check for any memory leaks.After successful execution, this command should return 0 error messages and 0 memory leaks.

"valgrind --leak-check=full --track-origins=yes ./test"

Action 7: For re-running the entire project, execute "make clean" which will clean the executable files generated for previous instance and repeat the actions from action 4


################### OVERVIEW OF THE ACTUAL IMPLEMENTATION ################### 

We already have few interfaces declared in storage_mgr.h and dberror.h which we use for the actual implementation and successfully accomplish the storage_mgr.c

The interpretation/specification of each fuction/method is as listed below:

################### Manipulating page files ################### 

Function 1: initStorageManager()

- This method initializes the Storage Manager if the initFlag is not already initialized.
- It also indicates the Stroage Manager creation and the page file creation.

Function 2: createPageFile()

- This method creates a new page file with given fileName. 
- The file size should be one page of 4096 Bytes.
- Further we allocate the memory using malloc for each page using the API malloc(). 
- Moreover the method fills this single page with '\0' bytes.
- Finally the opened file is closed.
- If the above steps work as expected, RC_OK return code is returned else RC_WRITE_FAILED is returned.

Function 3: openPageFile()

- Opens an existing page file, if the file does not exist then it returns RC_FILE_NOT_FOUND 
- The second parameter is an existing file handle which might or might not be an empty object.
- If the file opening is successful, then the file fields of this file handle should be initialized with the information
about the opened file such as filename, total number of Pages after traversing through the entire file.
- Further the current page position in the file handle needs to be set to 0, because when a user reads that file, it should start from beginning.
- It also sets a pointer object to the entire file reference.
- If everything goes well, RC_OK is returned.

Function 4: closePageFile()

- It is used to close an open page file.
- This function will accept fileHandle object and closes the file using fclose API.
- If everything goes well, RC_OK is returned.

Function 5: destroyPageFile()

- Here we remove a page file.
- Taking a fileName object as a parameter and removing the file using remove API.
- This method returns a RC_OK if page file is closed as expected or returns RC_FILE_NOT_FOUND in case of issue.


################### 	READING THE BLOCKS ################### 

Function 6: readBlock()

- This function is responsible for reading a specific block of data from file to memory.
- The function starts by checking whether the provided pageNum is within the total no of pages in the provided file.
- If the above condtion is true then, it returns an error code 'RC_READ_NON_EXISTING_PAGE'
- Further we start reading the block using fseek by positioning to the appoporate position , such that if the page number is valid and if fseek returns non zero value then the page exists and if not then it returns 'RC_READ_NON_EXISTING_PAGE'. 
- Next we start reading data using the fread to read the data from the file into the memory 'memPage'.
- If the operation is successful then it returns 'RC_OK'.
  

Funnction 7: getBlockPos()

- The primary purpose of this function is to only fetch the current page position of an open file.
- Next we check if the file handle is initiated and if it is initiated as expected then we assign the file handle withh the current page postion to if not we return RC_FILE_HANDLE_NOT_INIT.
- Additionally we do check if the specified file exists and return RC_FILE_NOT_FOUND in case of non existence.
- The position obtained from this method is being used in the subsequent functions to perform various tasks.


Function 8: readFirstBlock()

- This method reads the first block of data from the file into the memory.
- It does so by calling the existing readBlock function with pagenum(0th position).
- If the read happens as expected then it returns 'RC_OK', If not its returns 'RC_READ_NON_EXISTING_FILE'.


Function 10: readPreviousBlock()

- The core intent of this method is to read the previous block of data from the file into the memory.
- The existing readBlock is being passed with  getBlockPos(fHandle) - 1(i.e -> fHandle->curPagePos -1) to retrieve the previous block and reads the data from previous block.
-  If the read happens as expected then it returns 'RC_OK', If not its returns 'RC_READ_NON_EXISTING_FILE'.


Function 9: readCurrentBlock()

- This main function reads the content of the current block of data from the file into the memory.
- If the current block is read as expected then it returns 'RC_OK', If not its returns 'RC_READ_NON_EXISTING_FILE'


Function 11: readNextBlock()

- This method is used to read the contents of the next block of data from the file into the memory.
- The already exist readBlock function intialzies getBlockPos(fHandle) + 1(i.e -> fHandle->curPagePos + 1) to get the next block which reads the data from next block.
- If the next block is read as expected then it returns 'RC_OK', If not its returns 'RC_READ_NON_EXISTING_FILE'.

Function 12: readLastBlock()

- This function reads the contents of the last block of data from the file into the memory
- We start of by checking if the total pages are greater than zero if so then we read the last block mapping the fHandle->totaNumPages -1 which reads the last block finally assigning it to the pagepos.
- Next we check if the last page is being read as expected and if everything is as expected then we return 'RC_OK',If not its returns 'RC_READ_NON_EXISTING_FILE'


################### WRITING BLOCK TO PAGE ################### 


Function 13: writeBlock()

- This method is for writing a page to disk using an absolute position.
- Initally we check the existance of the page,page handler intialization and the mempage if any of them null it returns RC_READ_NON_EXISTING_PAGE.
- Further we check if the beginning of the page is specified if not we return RC_WRITE_FAILED.
- Once all the above conditions are validated, we check if there is failure in writng the block if so then it returns RC_WRITE_FAILED.
- Further we move the pointer to appropriate location.
- Then, we start the process of writing, we use "fwrite" function of C to do so.
- Finally if everything goes as expected, we will get RC_OK return code.

Function 14: writeCurrentBlock()

- This method is used for writing a page to disk using the current position.
- Initally we check if the file file pointer is valid if not then it returns "Error: Invalid file handle" 
- If the file pointer is valid we wirte the current block to file using the fwrite.
- Finally, we check if the block transfered are not greater than the PAGE_SIZE, and if there is any then it returns "Error: Failed to write the current to the file \n"

Function 15: appendEmptyBlock()

- Allocates memory for an empty page of size PAGE_SIZE.
- Temporarily stores the current page position (curPagePos) from the file handle.
- Updates the file handle's curPagePos to point to the end of the file.
- Increments the total number of pages in the file.
- Calls writeBlock to write the empty page to the newly appended block.
- If the write operation fails, frees memory, sets to NULL, and returns an error code.
- Restores the original page position in the file handle.
- Updates the total number of pages in the file by writing it to the file.
- Properly deallocates the memory and returns RC_OK on success.

Function 16: ensureCapacity()

- This method is for checking if the file has less than numberOfPages pages then increase the size to numberOfPages.
- Checks if the provided file handle (fHandle) is valid; if not, returns an error code.
- Verifies if the file represented by fHandle exists; if not, returns an error code.
- Checks if the total number of pages in the file is less than the desired numberOfPages.
- If more pages are needed, it appends empty blocks to the file until the desired capacity is reached.
- Returns RC_OK to indicate that the capacity has been ensured or that the file already met the desired capacity


################### FILES PROVIDED AS PART OF ASSIGNMENT/ALREADY EXISTING FILE ################### 

File 1: dberror.h

- This file contains page size for entire module.
- This file contains set of macros defined in this file which can be used during the sotrage manager implementation since all function returns an integer return code
- These return code inturn help us determine or evaluate that whether an operation performed was successful and if not what type of error had occurred, 
which is being handled by a method - "printError".

File 2: dberror.c

- This is in sync with the dberror.h file, and has the implementation of the methods specified in the dberror.h file, which will help in genrating the output in readable format.


File 3: test_assign1_1.c
- This is our main file OR entry point of this module function running all tests.
- We are performing two tests end to end in this file.


i- testCreateOpenClose (Try to create, open, and close a page file)
ii- testSinglePageContent  (Test for single page content)

The second test case had a calloc memory allocation in the end which was not freed. Hence, we have freed that ph page handle object declared using malloc in order to avoid any memory leaks in the system.
  
 ph = (SM_PageHandle)malloc(PAGE_SIZE);
 free(ph);
