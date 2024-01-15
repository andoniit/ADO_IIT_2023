Jai Shree Ram !!!
################### TEAM INFO ##################################################

Team Number-6

-Mugdha Atul Kulkarni - A20551539

-Anirudha Kapileshwari - A20549352

-Anjali Jagdish Tavhare - A20550996

-Aditya Ramchandra Kutre - A20544809



################################################################################

Involvement of the team -

- Mugdha Atul Kulkarni - 25%
- Anirudha Kapileshwari - 25%
- Anjali Jagdish Tavhare - 25%
- Aditya Ramchandra Kutre - 25%

################### BUFFER MANAGER IMPLEMENTATION ############################## 

Buffer Manager: A Module that can easily acccess blocks from a buffer pool after reading them from a file on disk for read or write operations.In addition, when the buffer pool is full and a fresh page is accessed, employing the appropriate page replacement algorithms and writing dirty data back to disk are also recommended.

###################  LIST OF FILES AND THE FILE DETAILS ################### 

The existing files layout has been designed such that comments have been added to each section of the code to make code maintenance easier and find bugs faster.List of files names present:

- README.txt
- buffer_mgr_stat.h
- buffer_mgr_stat.c
- buffer_mgr.h
- buffer_mgr.c
- dberror.c
- dberror.h
- dt.h
- storage_mgr.c
- storage_mgr.h
- test_assign2_1.c
- test_assign2_2.c
- test_helper.h
- Makefile

###################  LIST OF STEPS/ACTIONS TO BE RUN ################### 

Step 1: Open terminal and clone our repository from BitBucket using the terminal and keep it in your desire location.

Step 2: Go to your step 1 location, you will get "assign2_buffer_manager" folder.
Be right there in terminal.

Step 3:Use make command to execute the Makefile by typing "make test1" and "make test2"

Step 4:The above actions will create an output file "test_assign2_1.exe" and 
"test_assign2_2.exe"
        Then, run "make run_test1" and "make run_test2" to execute all test cases.
  
-- The code will take some time ro run all the test cases successfully, please wait for sometime.(It might take upto 3-4 mins to run)
-- The code executed successfully, once you see this message “OK: finished test”
-- test_assign2_2 is for extras credit.

Step 5: For re-running the entire project, execute "make clean" which will clean the executable files generated for previous instance and repeat the step from step 3


################### OVERVIEW OF THE ACTUAL IMPLEMENTATION ################### 

We already have few interfaces declared in buffer_mgr.h. Its implementation is written in buffer_mgr.c.

The interpretation/specification of each fuction/method is as listed below:

Method 1: getNumReadIO()
- The getNumReadIO function returns the number of pages that have been read from disk since a buffer pool has been initialized.

Method 2: getNumWriteIO()
- The getNumWriteIO method returns the number of pages written to the page file since the pool buffer has been initialized.

Method 3: unpinPage()
-After the client is done using the page, it is set free, for example, the fixcount of the particular page is decremented which is referred as "UNPINNING".
-The method takes a PageHandle object that needs to be unpinned.
-It traverses the entire bufferpool until the requried page the needs to be unpinned is found.
-If the page is found, It's fixCount is reduced by one and RC_OK is returned.

Method 4: pinPage()
-The pinPage function pins the page with given page number "pageNum".
- The process of putting a page onto the bufferpool is referred to as "PINNING" a page where each page is put in a pageFrame first, and then that is put onto the bufferpool.
- This method has support for 3 page Replacement policies which is includes 
---FIFO(First In First Out)
---LRU(Least Recently Used)
---CLOCK->(For Extra Credit)

Method 4.1: executeFIFO()
        -FIFO(First In First Out) Page Replacement strategy Implementations.
        --------------------FIFO Implementaion-------------------
        -This method checks if pageFrame is already used in Bufferpool using isPagePresentInBufferPool().
        -if the page is present then checks for empty spaces and fills those frames using occupyExistingSpaceInBufferPool().
        -if there is no empty space in buffer pool then the first page with no empty is take out using FIFO method.


Method 4.2: executeLRU()
        -LRU(Least Recently Used) page Replacement Implementations
        i.e. the page which is least Recently used will be replaced, with the new page.
-----------------------LRU Implementation------------------------------
        -This method checks if pageFrame already in buffer pool using isPagePresentInBufferPool().
        -If the page is already present in the buffer, return RC_OK else proceed further.
        -If there are empty spaces in the bufferpool, then fill in those frames first using occupyExistingSpaceInBufferPool().
        -Else if there are no empty spaces in the bufferpool, replace pages from pageFrame using LRU page replacement technique.

########################Extra credit ##################################

Method 4.3: executeClock()
        - CLOCK Page Replacement Strategy Implementations
        it implements circular queue for page replacement using clock strategy.

------------------- CLOCK IMPLEMENTATION -------------------------------
This method checks if pageFrame already in buffer pool using isPagePresentInBufferPool().
        -If the page is already present in the buffer, return RC_OK else proceed further.
        -If there are empty spaces in the bufferpool, then fill in those frames first using occupyExistingSpaceInBufferPool().
        -Else if there are no empty spaces in the bufferpool, replace pages from pageFrame using CLOCK replacement Strategy.

Method 5: getFrameContents
       -This method returns an array of PageNumbers(of size numPages).
       -where the ith element is the number of page stored in the ith page frame.
       - An empty page frame is represented using the constatnt NO PAGE.

Method 6: getDirtyFlags
       - This method returns an array of bools(of size numPages)
       - where the ith element is TRUE if the page stored in the ith page frame is dirty.
       - empty page frames are considered as clean.

Method 7: getFixCounts
       -This method returns an array of ints (of size numPages)
       -where the ith element is the fix count of the page stored in the ith page frame.
       - Return 0 for empty page frames.

Method 8: initBufferPool()

        Performs the following operations:
        1. openPageFile()- opens the existing page file using storage_mgr.c file.
        2. createPageFrame()- this method is defined to create empty page frame in the buffer pool.
        3. closePageFile()- closes the pageFile. This definition of this method is in storage_mgr.c file.
        4. addPageInController()- Defined to set the nodes to head node and amke links.
        5. initController()- Defined to initialize the values and store the data.


Method 9: shutdownBufferPool()
       -Defined to free all the memory allocated in the page frames.
       - calls forceFlushPool() to write all the dirty pages to pageFile.

Method 10: forceFlushPool()
       -Method is defined to check if FixCount is set to 0.
       -Checks if the dirtyFlag is set or not.
       -write the dirty page to the pageFile.

Method 11: markDirty()
       -marks dirty bits to page by pagenum

Method 12: forcePage()
       -calls writeBlock() from storage_mgr.c
       -writes the current content of the page back to the page file.
       - and resets the page as dirtyPage indentified by pageNum.


################### FILES PROVIDED AS PART OF ASSIGNMENT/ALREADY EXISTING FILE ################### 

File 1:buffer_mgr_stat.h
 -This file just contains the methods for printing the page and cool contents.

File 2: buffer_mgr_stat.c
 -This file provides implementation of methods provided in buffer_mgr_stat.h file for printing data to user.

file 3: storage_mgr.h
 -This is a header file for storage manager implemented in assignment 1.

file 4: storage_mgr.c
 -This is the actual implementation w.r.t storage_mgr.h
 -This file implements the basic storage manager functionality as in a module that is capable of reaing blocks from a file on disk into memory and writing blocks from memory to a file on disk.

File 5:dberror.h
- This file contains page size for entire module.
- This file contains set of macros defined in this file which can be used during the sotrage manager implementation since all function returns an integer return code
- These return code inturn help us determine or evaluate that whether an operation performed was successful and if not what type of error had occurred, 
which is being handled by a method - "printError".

File 6:dberror.c
- This is in sync with the dberror.h file, and has the implementation of the methods specified in the dberror.h file.
-This file is the implementation of that printError() and errorMessage() methods of dberror.h.file.

File 7:test_assign2_1.c
- This is our main file OR entry point of this module function running all tests.
- We are performing two tests end to end in this file.
1)testCreatingAndReadingDummyPages()
2)testReadPage()
3)testFIFO()
4)testLRU()

File 8: test_assign2_2.c
-This is our second test file for executing the optional (### Extra Credit ###) CLOCK page replacement strategy.