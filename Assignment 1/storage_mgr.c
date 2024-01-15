

/* ########################### TEAM ########################### 

$ Team Number-6

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


############################################## OVERVIEW OF THE ACTUAL IMPLEMENTATION ############################################

We already have few interfaces declared in storage_mgr.h and dberror.h which we use for the actual implementation and successfully accomplish the storage_mgr.c

The interpretation/specification of each fuction/method is as listed below:

################################################# LIST OF STEPS/ACTIONS TO BE RUN ######################################################

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

*/


#include"storage_mgr.h"
#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include"dberror.h"




//FILE *newfile;
//int pgread;

//############################ MANUPULATING PAGE FILES  ################################
//creating Storage Manager 
void initStorageManager(void){
    printf("\n\n$$$$$$$$$$$$$$ Storage manager created $$$$$$$$$$$$$$\n\n");
    printf("A Storage Manager is a program that provides \n an interface between the data stored in the database and the queries received.\n We have implemented a module that is capable of reading \nblocks from a file on disk into memory and writing blocks from memory to a file on disk.\n\n");
}


RC createPageFile (char *fileName)
{
    FILE *pFile = fopen(fileName,"w");
    
    SM_PageHandle str = malloc(PAGE_SIZE); //apply for a page using malloc in C
    if (str==NULL)
        return RC_WRITE_FAILED;
    memset(str,'\0',PAGE_SIZE); //fill a page full of '\0'
    
    fprintf(pFile,"%10d\n",1); //write total pages=1
    fwrite(str, sizeof(char), PAGE_SIZE, pFile);
    
    free(str);
    str=NULL;
    fclose(pFile);
    return RC_OK;
}



RC openPageFile (char *fileName, SM_FileHandle *fHandle)
{
    FILE *pFile = fopen(fileName,"r+");
    if (!pFile)
        return RC_FILE_NOT_FOUND;
    //write fHandle
    int total_pages;
    fscanf(pFile,"%d\n",&total_pages);
    
    fHandle->fileName=fileName;
    fHandle->totalNumPages=total_pages; //total numbers, not offset
    fHandle->curPagePos=0;  //but this is an offset
    fHandle->mgmtInfo=pFile; //POSIX file descriptor
    
    return RC_OK;
}

RC closePageFile(SM_FileHandle *fHandle) {
    // Free any dynamically allocated memory for fileName (if applicable)
    // Example: if (fHandle->fileName != NULL) free(fHandle->fileName);

    // Reset the members of the SM_FileHandle structure
    fHandle->fileName = NULL; // Set it to NULL or a default value
    fHandle->curPagePos = 0;
    fHandle->totalNumPages = 0;
    return RC_OK;
}


//Distroy
RC destroyPageFile (char *fileName){
    if(remove(fileName)==0){
        return RC_OK;

    }
    else{
        return RC_FILE_NOT_FOUND;
    }
}

//############################ READING THE BLOCK FROM DISC ################################

RC readBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  if (pageNum > fHandle->totalNumPages - 1 || pageNum < 0)
    return RC_READ_NON_EXISTING_PAGE;

  if (fseek(fHandle->mgmtInfo, (pageNum + 1) * PAGE_SIZE, SEEK_SET) != 0)
    return RC_READ_NON_EXISTING_PAGE;

  fread(memPage, sizeof(char), PAGE_SIZE, fHandle->mgmtInfo);
  fHandle->curPagePos = pageNum;

  return RC_OK;
}



int getBlockPos(SM_FileHandle *fHandle)
{
    if (fHandle != NULL)
    {
        return fHandle->curPagePos;
    }
    else
    {
        return RC_FILE_HANDLE_NOT_INIT;
    }
}



RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  return (readBlock(0, fHandle, memPage) == RC_OK) ? RC_OK : RC_READ_NON_EXISTING_PAGE;
}

RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage){
    return (readBlock(fHandle->curPagePos-1, fHandle, memPage)== RC_OK) ? RC_OK : RC_READ_NON_EXISTING_PAGE;
}

RC readCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
  RC result = readBlock(getBlockPos(fHandle), fHandle, memPage);
  return (result == RC_OK) ? RC_OK : RC_READ_NON_EXISTING_PAGE;
}



RC readNextBlock(SM_FileHandle *fHandle, SM_PageHandle memPage){
    int nextBlockPos=getBlockPos(fHandle) + 1;
    return (readBlock(nextBlockPos, fHandle, memPage)==RC_OK) ? RC_OK : RC_READ_NON_EXISTING_PAGE;
}



RC readLastBlock(SM_FileHandle *fHandle, SM_PageHandle memPage){
    int lastPagePos= (fHandle->totalNumPages>0) ? (fHandle->totalNumPages-1) :0;

    if((*fHandle).curPagePos==lastPagePos){
        return RC_OK;
    }

    return RC_READ_NON_EXISTING_PAGE;
}






//######################### WRITING BLOCKS TO THE PAGE FILE #########################
RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    //Chack if 'PageNum' is out of bounds
    if(pageNum>fHandle->totalNumPages-1 || pageNum<0){
        return RC_READ_NON_EXISTING_PAGE;
    }

    //Seek the specified page in the file using fseek
     if(fseek(fHandle->mgmtInfo,(pageNum+1)*PAGE_SIZE,SEEK_SET)!=0){

        return RC_WRITE_FAILED;
    }

    //Write the content of 'memPage' to the file
    if(fwrite(memPage,PAGE_SIZE,1 ,fHandle->mgmtInfo)!=1){

        return RC_WRITE_FAILED;
    }
    //Update the current page position in thr=e file handle
    fHandle->curPagePos = pageNum;
    return RC_OK;
}





RC writeCurrentBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
{
    //get the current block position
    int currentBlockPos = getBlockPos(fHandle);

    //Write 'memPage' to the current block position
    return (writeBlock(currentBlockPos, fHandle, memPage) == RC_OK) ? RC_OK : RC_WRITE_FAILED;
}





RC appendEmptyBlock (SM_FileHandle *fHandle)
{
    SM_PageHandle str = (char *) calloc(PAGE_SIZE,1); //apply for a page filled with zero bytes
    //append the page
    int temp =fHandle->curPagePos;
    
    fHandle->curPagePos=fHandle->totalNumPages;
    fHandle->totalNumPages+=1;
    
    RC return_value = writeBlock(fHandle->curPagePos, fHandle, str);
    //if append new, totalNumPages can be used as an offset
    if (return_value!=RC_OK)
    {
        free(str);
        str=NULL;
        return return_value;
    }
    
    fHandle->curPagePos=temp;
    //change total pages in file
    rewind(fHandle->mgmtInfo);//reset file pointer
    fprintf(fHandle->mgmtInfo,"%d\n",fHandle->totalNumPages);
    fseek(fHandle->mgmtInfo,5+(fHandle->curPagePos)*PAGE_SIZE,SEEK_SET); //recover file pointer
    
    free(str);
    str=NULL;
    return RC_OK;
}





RC ensureCapacity(int numberOfPages, SM_FileHandle *fHandle)
{
    if (fHandle == NULL)
        return RC_FILE_HANDLE_NOT_INIT;

    if (fHandle->mgmtInfo != NULL)
    {
        // Check if total number of pages is less than number of pages
        if (fHandle->totalNumPages < numberOfPages)
        {
            while (fHandle->totalNumPages != numberOfPages)
            {
                appendEmptyBlock(fHandle); // Append empty block
            }
        }
        return RC_OK;
    }

    return RC_FILE_NOT_FOUND;
}