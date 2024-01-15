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

#include "buffer_mgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "dberror.h"
#include "storage_mgr.h"
#include <math.h>

// Structure for BufferPool's PageFrame
typedef struct BM_PageFrame
{
  SM_PageHandle pageData; // Data of page
  PageNumber eachPageNum; // Each Page Number in Page Frame
  int dirtyFlag;          // determines if page is Dirty
  int fixCount;           // gives count of current users accessing the page
  int hitCount;           // Used by LRU algorithm to get the least recently used page
} BM_PageFrame;

int totalPageRead = 0; // stores the total number of pages read from the buffer pool

int totalPageWritten = 0; // stores the total number of pages written into the buffer pool

int clockRefBit = 0; // Reference variable used in CLOCK pin page strategy

int lruBit = 0; // determines least recently added page into the buffer pool

int bmBufferSize = 0; // size of the buffer pool - MAX page frames can be stored in BufferPool

bool globalObject;

void addPageInController(BM_PageFrame *buffer, int i)
{
  buffer[i].pageData = NULL; // calloc(PAGE_SIZE, sizeof(char *));
  buffer[i].eachPageNum = -1;
  buffer[i].dirtyFlag = 0;
  buffer[i].fixCount = 0;
  buffer[i].hitCount = 0;
}


extern RC initBufferPool(BM_BufferPool *const bm, const char *const pageFileName, const int numPages, ReplacementStrategy strategy, void *stratData)
{
  bm->pageFile = (char *)pageFileName;
  bm->strategy = strategy;
  bm->numPages = numPages;

  // Reserver memory space = number of pages x space required for one page
  BM_PageFrame *buffer = malloc(sizeof(BM_PageFrame) * numPages);

  // Buffersize is the total number of pages in memory or the buffer pool.
  bmBufferSize = numPages;
  int i = 0;
    while (i < bmBufferSize)
    {
      addPageInController(buffer, i);
      i++;
    }
    globalObject = true;
    printf("Buffer Initialization Successful !!!");

  bm->mgmtData = buffer;
  totalPageWritten = clockRefBit = 0;
  return RC_OK;
}

//FIFO (First In First Out) Page Replacement Strategy Implementations
extern RC executeFIFO(BM_BufferPool *const bm, BM_PageFrame *page)
{
  printf("\nFIFO Started");
  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;

  int i = 0;
  int readPageCount = totalPageRead % bmBufferSize;

  // Interating through all the page frames in the buffer pool
  while (i < bmBufferSize)
  {
    if (frame[readPageCount].fixCount != 0)
    {
      // If the current page frame is being used by some client, we move on to the next location
      readPageCount++;
      readPageCount = (readPageCount % bmBufferSize == 0) ? 0 : readPageCount;
      // If page in memory has been modified (dirtyBit = 1), then write page to disk
    }
    else
    {
      if (frame[readPageCount].dirtyFlag == 1)
      {
         SM_FileHandle fh;
        openPageFile(bm->pageFile, &fh);
        writeBlock(frame[readPageCount].eachPageNum, &fh, frame[readPageCount].pageData);

        // Increase the writeCount which records the number of writes done by the buffer manager.
        totalPageWritten++;
      }

      // Setting page frame's content to new page's content
      frame[readPageCount].fixCount = page->fixCount;
      frame[readPageCount].pageData = page->pageData;
      frame[readPageCount].dirtyFlag = page->dirtyFlag;
      frame[readPageCount].eachPageNum = page->eachPageNum;
      break;
    }
    i++;
  }
  printf("\nFIFO END");
  return RC_OK;
}


//The shutdownBufferPool function writes all the pages to the disk and destroys the buffer pool object.
extern RC shutdownBufferPool(BM_BufferPool *const bm)
{
  // object not initialized
  if (globalObject == false)
    return RC_ERROR;

  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;
  
  forceFlushPool(bm);

  int i = 0;
  while (i < bmBufferSize)
  {
    if (frame[i].fixCount != 0)
    {
      return RC_PINNED_PAGES_IN_BUFFER;
    }
    i++;
  }
  free(frame);
  bm->mgmtData = NULL;
  return RC_OK;
}

//The forceFlushPool function writes all the pages to the disk where dirtyFlag is set and fix count is 0
extern RC forceFlushPool(BM_BufferPool *const bm)
{
  
  // object not initialized
  if (globalObject == false)
    return RC_ERROR;

  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;

  int eachpage = 0;
  while (eachpage < bmBufferSize)
  {
    if (frame[eachpage].fixCount == 0 && frame[eachpage].dirtyFlag == 1)
    {
      SM_FileHandle fh;
  openPageFile(bm->pageFile, &fh);
      writeBlock(frame[eachpage].eachPageNum, &fh, frame[eachpage].pageData);
      frame[eachpage].dirtyFlag = 0;
      totalPageWritten++;
    }
    eachpage++;
  }
  return RC_OK;
}

//he markDirty function marks the specified page as dirty.
extern RC markDirty(BM_BufferPool *const bm, BM_PageHandle *const page)
{
  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;

  int eachpage = 0;
  // Iterating through all the pages in the buffer pool
  while (eachpage < bmBufferSize)
  {
    // If the current page is the page to be marked dirty, then set dirtyBit = 1 (page has been modified) for that page
    if (frame[eachpage].eachPageNum == page->pageNum)
    {
      frame[eachpage].dirtyFlag = 1;
      return RC_OK;
    }
    eachpage++;
  }
  return RC_ERROR;
}

//The forcePage function writes the specified page to the disk
extern RC forcePage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;
  int eachpage = 0;
      
  while (eachpage < bmBufferSize)
  {
    if (frame[eachpage].eachPageNum == page->pageNum)
    {
      SM_FileHandle fh;
      openPageFile(bm->pageFile, &fh);
      writeBlock(frame[eachpage].eachPageNum, &fh, frame[eachpage].pageData);
      frame[eachpage].dirtyFlag = 0;
      totalPageWritten++;
    }
    eachpage++;
  }
  return RC_OK;
}

//Setting the replacement page's content to buffer pool's frame contents
void assignReplacememtPageContentsToFrame(BM_PageFrame *frame, int minHitIndex, BM_PageFrame *page)
{
  frame[minHitIndex].hitCount = page->hitCount;
  frame[minHitIndex].fixCount = page->fixCount;
  frame[minHitIndex].eachPageNum = page->eachPageNum;
  frame[minHitIndex].pageData = page->pageData;
  frame[minHitIndex].dirtyFlag = page->dirtyFlag;
}

//function that implements Clock Page Replacement Strategy.
extern RC executeCLOCK(BM_BufferPool *const bm, BM_PageFrame *page)
{
  BM_PageFrame *pageFrameObj = (BM_PageFrame *)bm->mgmtData;
  
  while (1)
  {
    clockRefBit = (clockRefBit % bmBufferSize == 0) ? 0 : clockRefBit;

    if (pageFrameObj[clockRefBit].hitCount == 0)
    {
      // If page in memory has been modified (dirtyFlag = 1), then write page to disk
      if (pageFrameObj[clockRefBit].dirtyFlag == 1)
      {
        SM_FileHandle fileHandle;
        openPageFile(bm->pageFile, &fileHandle);
        writeBlock(pageFrameObj[clockRefBit].eachPageNum, &fileHandle, pageFrameObj[clockRefBit].pageData);

        // Increase the totalPageWritten which records the number of writes done by the buffer manager.
        totalPageWritten++;
      }

      // Setting page frame's content to new page's content
      assignReplacememtPageContentsToFrame(pageFrameObj, clockRefBit, page);
      clockRefBit++;
      break;
    }
    else
    {
      // Incrementing clockRefBit so that we can check the next page frame location.
      // We set hitCount = 0 so that this loop doesn't go into an infinite loop.
      pageFrameObj[clockRefBit++].hitCount = 0;
    }
  }
  //closePageFile(&fileHandle);
  return RC_OK;
}

//function returns an array of PageNumbers (of size numPages)
extern PageNumber *getFrameContents(BM_BufferPool *const bm)
{
  BM_PageFrame *pageFrameObj = (BM_PageFrame *)bm->mgmtData;
  PageNumber *pageNumberArr = malloc(sizeof(PageNumber) * bmBufferSize);

  int eachPage;
  for (eachPage = 0; eachPage < bmBufferSize; eachPage++)
  {
    pageNumberArr[eachPage] = (pageFrameObj[eachPage].eachPageNum != -1) ? pageFrameObj[eachPage].eachPageNum : NO_PAGE;
  }
  return pageNumberArr;
}

//function returns an array of bools (of size numPages)
extern bool *getDirtyFlags(BM_BufferPool *const bm)
{
  BM_PageFrame *pageFrameObj = (BM_PageFrame *)bm->mgmtData;
  bool *boolArr = malloc(sizeof(bool) * bmBufferSize);

  int eachPage;
  for (eachPage = 0; eachPage < bmBufferSize; eachPage++)
  {
    boolArr[eachPage] = (pageFrameObj[eachPage].dirtyFlag == 1) ? true : false;
  }
  return boolArr;
}

//function returns an array of ints (of size numPages)
extern int *getFixCounts(BM_BufferPool *const bm)
{
  BM_PageFrame *pageFrameObj = (BM_PageFrame *)bm->mgmtData;
  int *fixCountArr = malloc(sizeof(int) * bmBufferSize);

  int eachPage;
  for (eachPage = 0; eachPage < bmBufferSize; eachPage++)
  {
    fixCountArr[eachPage] = (pageFrameObj[eachPage].fixCount != -1) ? pageFrameObj[eachPage].fixCount : 0;
  }
  return fixCountArr;
}

//The getNumReadIO function returns the number of pages that have been read from disk
extern int getNumReadIO(BM_BufferPool *const bm)
{
  return (totalPageRead + 1); // Adding one because totalPageRead starts with 0.
}

//The getNumWriteIO method returns the number of pages written to the page file
extern int getNumWriteIO(BM_BufferPool *const bm)
{
  return totalPageWritten;
}

//After the client is done using the page, it is set free
extern RC unpinPage(BM_BufferPool *const bm, BM_PageHandle *const page)
{
  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;
  int counter = 0;
  while (counter < bmBufferSize)
  {
    // If the buffer manager page number matches with the page that needs to be unpinned,
    // then decrease fixCount (which means client is done using the page and it is set free)
    if (frame[counter].eachPageNum == page->pageNum)
    {
      frame[counter].fixCount--;
      break;
    }
    counter++;
  }
  return RC_OK;
}

//LRU (Least Recently Used) Page Replacement Strategy Implementations
extern RC executeLRU(BM_BufferPool *const bm, BM_PageFrame *page)
{
  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;
  int minHitIndex, minHitCount;

  // Traverse all frames in the buffer pool to find a page frame with fixCount = 0
  int index = 0;
   
  while (index < bmBufferSize)
  {
    if (frame[index].fixCount == 0)
    {
      minHitCount = frame[index].hitCount;
      minHitIndex = index;
      break;
    }
    index++;
  }

  // Traverse all frames in the buffer pool to find a page frame with minimum hitCount (LRU) page
  index = minHitIndex + 1;
  while (index < bmBufferSize)
  {
    if (frame[index].hitCount < minHitCount)
    {
      minHitCount = frame[index].hitCount;
      minHitIndex = index;
    }
    index++;
  }

  // If page in memory has been modified (dirtyBit = 1), then write page to disk
  if (frame[minHitIndex].dirtyFlag == 1)
  {
    SM_FileHandle fileHandle;
    openPageFile(bm->pageFile, &fileHandle);
    writeBlock(frame[minHitIndex].eachPageNum, &fileHandle, frame[minHitIndex].pageData);
    totalPageWritten++;
  }

  assignReplacememtPageContentsToFrame(frame, minHitIndex, page);
  return RC_OK;
}

//Checking if buffer pool is empty and this is the first page to be pinned
RC occupyFirstSpaceInBufferPool(BM_BufferPool *const bm, BM_PageHandle *page, const PageNumber pageNum)
{
  
  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;
  if (frame[0].eachPageNum == -1)
  {
    SM_FileHandle fileHandle;
    openPageFile(bm->pageFile, &fileHandle);
    frame[0].pageData = (SM_PageHandle)malloc(PAGE_SIZE);
    ensureCapacity(pageNum, &fileHandle);
    readBlock(pageNum, &fileHandle, frame[0].pageData);
    frame[0].fixCount++;
    frame[0].eachPageNum = pageNum;
    frame[0].hitCount = totalPageRead = lruBit = 0;
    
    page->pageNum = pageNum;
    page->data = frame[0].pageData;
    
    return RC_OK;
  }
  
  return RC_ERROR;
}

//set hit counts of various algos as per strategy
 void setHitCounts(ReplacementStrategy strategy, BM_PageFrame *frame, int counter)
 {
   switch (strategy)
   {
  case RS_LRU:
     frame[counter].hitCount = lruBit;
     break;
   case RS_CLOCK:
     frame[counter].hitCount = 1;
     break;
     
     case RS_LRU_K:
     case RS_LFU:
     printf("The given strategy [%d] is not implemented !!!", strategy);
    break;
  case RS_FIFO:
  default:
     break;
   }
}

//If the Buffer Pool is Full, we need to replace an existing page using page replacement strategy
RC replacePageInBufferPool(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
  
  BM_PageFrame *newPage = (BM_PageFrame *)malloc(sizeof(BM_PageFrame));

  // Reading page from disk and initializing page frame's contents in the buffer pool
  SM_FileHandle fileHandle;
  newPage->pageData = (SM_PageHandle)malloc(PAGE_SIZE);

  openPageFile(bm->pageFile, &fileHandle);
  readBlock(pageNum, &fileHandle, newPage->pageData);
  newPage->eachPageNum = pageNum;
  newPage->dirtyFlag = 0;
  newPage->fixCount = 1;

  totalPageRead++;
  lruBit++;

 switch (bm->strategy)
   {
  case RS_LRU:
     newPage->hitCount = lruBit;
     break;
   case RS_CLOCK:
     newPage->hitCount = 1;
     break;
      case RS_LRU_K:
     case RS_LFU:
     printf("The given strategy [%d] is not implemented !!!", bm->strategy);
    break;
  case RS_FIFO:
  default:
     break;
   }
  
  page->pageNum = pageNum;
  page->data = newPage->pageData;

  switch (bm->strategy)
  {
  case RS_FIFO:
  
    return executeFIFO(bm, newPage);

  case RS_LRU:
    return executeLRU(bm, newPage);

  case RS_CLOCK:
    return executeCLOCK(bm, newPage);

  default:
    printf("The given strategy [%d] is not implemented !!!", bm->strategy);
    return RC_ERROR;
  }
}

//if pageFrame is already present in the buffer pool, increment all tha stats and hit counts
void ifPagePresentInBufferPool(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum,
                               BM_PageFrame *frame, int counter)
{
  
  // increment fixcount and lruBit to indicate one more client is using the page now
  frame[counter].fixCount++;
  lruBit++;
  clockRefBit++;

  setHitCounts(bm->strategy, frame, counter);
 page->pageNum = pageNum;
 page->data = frame[counter].pageData;
 
}

//The pinPage function pins the page with page number "pageNum".
extern RC pinPage(BM_BufferPool *const bm, BM_PageHandle *const page, const PageNumber pageNum)
{
  // object not initialized
   if (globalObject == false)
     return RC_ERROR;

  // Checking if buffer pool is empty and this is the first page to be pinned
  if (occupyFirstSpaceInBufferPool(bm, page, pageNum) == RC_OK)
    return RC_OK;

  bool isBufferFull = true;
  BM_PageFrame *frame = (BM_PageFrame *)bm->mgmtData;
  int counter = 0;

  for (; counter < bmBufferSize; counter++)
  {
    if (frame[counter].eachPageNum != -1)
    {
      if (frame[counter].eachPageNum == pageNum) // if the Page is already Present In Buffer Pool
      {
       
  // increment fixcount and lruBit to indicate one more client is using the page now
  frame[counter].fixCount++;
  lruBit++;
  clockRefBit++;
isBufferFull = false;
  setHitCounts(bm->strategy, frame, counter);
 page->pageNum = pageNum;
 page->data = frame[counter].pageData;
        break;
      }
    }
    else
    {
   
      SM_FileHandle fileHandle;
      openPageFile(bm->pageFile, &fileHandle);
      frame[counter].pageData = (SM_PageHandle)malloc(PAGE_SIZE);
      readBlock(pageNum, &fileHandle, frame[counter].pageData);

      frame[counter].fixCount = 1;
      frame[counter].eachPageNum = pageNum;
      lruBit++;
      totalPageRead++;

      setHitCounts(bm->strategy, frame, counter);
     
      page->pageNum = pageNum;
      page->data = frame[counter].pageData;

      isBufferFull = false;
      break;
    }
  }

  if (isBufferFull == true)
  {  return replacePageInBufferPool(bm, page, pageNum);
}
return RC_OK;
}
