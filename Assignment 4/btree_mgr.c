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


#include <string.h>
#include <stdlib.h>
#include "btree_mgr.h"
#include "record_mgr.h"
#include "buffer_mgr.h"
#include "storage_mgr.h"

typedef struct RM_BtreeNode {
    void **ptrs;
    Value *keys;
    struct RM_BtreeNode *paptr;
    int KeyCounts;
    int pos; 
    bool isLeaf;
} RM_BtreeNode;

typedef struct RM_bTree_mgmtData{
    int maxKeyNum;
    int numEntries;
    BM_BufferPool *bp;
}RM_bTree_mgmtData;

typedef struct RM_BScan_mgmt{
    int totalScan;
    int index;
    RM_BtreeNode *cur;
}RM_BScan_mgmt;

RM_BtreeNode *root = NULL;
int numNodValue = 0;
int sizeofNod = 0;
int globalPos = 0;
char *sv = NULL;
char *sv2 = NULL;
Value empty;

//****************Customized functions*********************//

bool strNoLarger(const char *c1, const char *c2) {
    return strcmp(c1, c2) <= 0;
}


//This function creates a new general node, which can be adapted to serve as a leaf/internal/root node.
RM_BtreeNode *createNewNod() {
    RM_BtreeNode *newNode = (RM_BtreeNode *)malloc(sizeof(RM_BtreeNode));

    if (newNode != NULL) {
        newNode->ptrs = malloc(sizeofNod * sizeof(void *));
        newNode->keys = malloc((sizeofNod - 1) * sizeof(Value));

        if (newNode->ptrs != NULL && newNode->keys != NULL) {
            newNode->paptr = NULL;
            newNode->KeyCounts = 0;
            newNode->isLeaf = FALSE;

            numNodValue += 1;

            return newNode;
        } else {
            // Handle memory allocation failure
            free(newNode->ptrs);
            free(newNode->keys);
            free(newNode);
        }
    }

    return NULL;  // Memory allocation failed
}


//--> This function inserts a new node (leaf or internal node) into the B+ tree.
//--> It returns the root of the tree after insertion.
RC insertParent(RM_BtreeNode *left, RM_BtreeNode *right, Value key) {
    RM_BtreeNode *paptr = left->paptr;
    int index = 0;
    int i = 0;

    if (paptr == NULL) {
        // Create parent
        RM_BtreeNode *NewRoot = createNewNod();

        if (NewRoot != NULL) {
            NewRoot->keys[0] = key;
            NewRoot->KeyCounts = 1;
            NewRoot->ptrs[0] = left;
            NewRoot->ptrs[1] = right;
            left->paptr = NewRoot;
            right->paptr = NewRoot;

            root = NewRoot;
            return RC_OK;
        } else {
            // Memory allocation failed
            return RC_IM_MEMORY_ERROR;  // Corrected the error code
        }
    } else {
        // Then it has a parent
        while (index < paptr->KeyCounts && paptr->ptrs[index] != left) index++;

        if (paptr->KeyCounts < sizeofNod - 1) {
            // Have an empty slot
            for (int i = paptr->KeyCounts; i > index; i--) {
                paptr->keys[i] = paptr->keys[i - 1];
                paptr->ptrs[i + 1] = paptr->ptrs[i];
            }
            paptr->keys[index] = key;
            paptr->ptrs[index + 1] = right;
            paptr->KeyCounts += 1;

            return RC_OK;
        }
    }

    // Then no empty space, split the node
    i = 0;
    int middleLoc;
    RM_BtreeNode **tempNode, *newNode;
    Value *tempKeys;

    tempNode = malloc((sizeofNod + 1) * sizeof(RM_BtreeNode *));
    tempKeys = malloc(sizeofNod * sizeof(Value));

    if (tempNode != NULL && tempKeys != NULL) {
        for (i = 0; i < sizeofNod + 1; i++) {
            if (i < index + 1)
                tempNode[i] = paptr->ptrs[i];
            else if (i == index + 1)
                tempNode[i] = right;
            else
                tempNode[i] = paptr->ptrs[i - 1];
        }
        for (i = 0; i < sizeofNod; i++) {
            if (i < index)
                tempKeys[i] = paptr->keys[i];
            else if (i == index)
                tempKeys[i] = key;
            else
                tempKeys[i] = paptr->keys[i - 1];
        }

        if (sizeofNod % 2 == 0)
            middleLoc = sizeofNod / 2;
        else
            middleLoc = sizeofNod / 2 + 1;

        paptr->KeyCounts = middleLoc - 1;
        for (i = 0; i < middleLoc - 1; i++) {
            paptr->ptrs[i] = tempNode[i];
            paptr->keys[i] = tempKeys[i];
        }
        paptr->ptrs[i] = tempNode[i];

        newNode = createNewNod();
        if (newNode != NULL) {
            newNode->KeyCounts = sizeofNod - middleLoc;
            for (i = middleLoc; i <= sizeofNod; i++) {
                newNode->ptrs[i - middleLoc] = tempNode[i];
                newNode->keys[i - middleLoc] = tempKeys[i];
            }
            newNode->paptr = paptr->paptr;

            Value t = tempKeys[middleLoc - 1];
            free(tempNode);
            tempNode = NULL;
            free(tempKeys);
            tempKeys = NULL;

            return insertParent(paptr, newNode, t);
        } else {
            // Memory allocation failed
            free(tempNode);
            free(tempKeys);
            return RC_IM_MEMORY_ERROR;  // Corrected the error code
        }
    } else {
        // Memory allocation failed
        free(tempNode);
        free(tempKeys);
        return RC_IM_MEMORY_ERROR;  // Corrected the error code
    }
}

//This function deletes the the entry/record having the specified key.
RC deleteNode(RM_BtreeNode *bTreeNode, int index) {
    int position, i, j;
    RM_BtreeNode *brother;

    // Reduce the number of key values
    bTreeNode->KeyCounts--;
    int NumKeys = bTreeNode->KeyCounts;

    if (bTreeNode->isLeaf) {
        // Remove
        free(bTreeNode->ptrs[index]);
        bTreeNode->ptrs[index] = NULL;

        // Re-order
        for (i = index; i < NumKeys; i++) {
            bTreeNode->keys[i] = bTreeNode->keys[i + 1];
            globalPos = bTreeNode->pos;
            bTreeNode->ptrs[i] = bTreeNode->ptrs[i + 1];
        }

        bTreeNode->keys[i] = empty;
        bTreeNode->ptrs[i] = NULL;
    } else {
        // Not leaf
        for (i = index - 1; i < NumKeys; i++) {
            bTreeNode->keys[i] = bTreeNode->keys[i + 1];
            globalPos = bTreeNode->pos;
            bTreeNode->ptrs[i + 1] = bTreeNode->ptrs[i + 2];
        }

        bTreeNode->keys[i] = empty;
        bTreeNode->ptrs[i + 1] = NULL;
    }

    // Finished removal and re-order
    int halfSize = (bTreeNode->isLeaf) ? sizeofNod / 2 : (sizeofNod - 1) / 2;
    if (NumKeys >= halfSize) {
        return RC_OK;
    }

    // Deal with underflow
    if (bTreeNode == root) {
        if (root->KeyCounts > 0) {
            return RC_OK;
        }

        // Root has no key left
        RM_BtreeNode *newRoot = NULL;

        if (!root->isLeaf) {
            // Only one child left
            newRoot = root->ptrs[0];
            newRoot->paptr = NULL;
        }

        free(root->keys);
        root->keys = NULL;
        free(root->ptrs);
        root->ptrs = NULL;
        free(root);
        root = NULL;
        numNodValue -= 1;
        root = newRoot;

        return RC_OK;
    }

    // Not root
    RM_BtreeNode *parentNode = bTreeNode->paptr;
    position = 0;

    while (position < parentNode->KeyCounts && parentNode->ptrs[position] != bTreeNode) {
        position++;
    }

    if (position == 0) {
        // Leftmost
        brother = parentNode->ptrs[1];
    } else {
        // Regular
        brother = parentNode->ptrs[position - 1];
    }

    int brotherSize = (bTreeNode->isLeaf) ? sizeofNod - 1 : sizeofNod - 2;

    // If can merge two nodes
    if (brother->KeyCounts + NumKeys <= brotherSize) {
        // Merging
        if (position == 0) {
            RM_BtreeNode *temp = bTreeNode;
            bTreeNode = brother;
            brother = temp;
            position = 1;
            NumKeys = bTreeNode->KeyCounts;
        }

        i = brother->KeyCounts;

        if (!bTreeNode->isLeaf) {
            brother->keys[i] = parentNode->keys[position - 1];
            i += 1;
            NumKeys += 1;
        }

        for (j = 0; j < NumKeys; j++) {
            brother->keys[i] = bTreeNode->keys[j];
            globalPos = brother->pos;
            brother->ptrs[i] = bTreeNode->ptrs[j];
            bTreeNode->keys[j] = empty;
            bTreeNode->ptrs[j] = NULL;
            i++;
        }

        brother->KeyCounts += NumKeys;
        brother->ptrs[sizeofNod - 1] = bTreeNode->ptrs[sizeofNod - 1];

        numNodValue -= 1;
        free(bTreeNode->keys);
        bTreeNode->keys = NULL;
        free(bTreeNode->ptrs);
        bTreeNode->ptrs = NULL;
        free(bTreeNode);
        bTreeNode = NULL;

        return deleteNode(parentNode, position);
    }

    // Get one from brother
    int brotherNumKeys;

    if (position != 0) {
        // Get one from left
        if (!bTreeNode->isLeaf) {
            bTreeNode->ptrs[NumKeys + 1] = bTreeNode->ptrs[NumKeys];
        }

        // Shift to right by 1
        for (i = NumKeys; i > 0; i--) {
            bTreeNode->keys[i] = bTreeNode->keys[i - 1];
            globalPos = bTreeNode->pos;
            bTreeNode->ptrs[i] = bTreeNode->ptrs[i - 1];
        }

        // i=0
        if (bTreeNode->isLeaf) {
            brotherNumKeys = brother->KeyCounts - 1;
            bTreeNode->keys[0] = brother->keys[brotherNumKeys];
            parentNode->keys[position - 1] = bTreeNode->keys[0];
        } else {
            brotherNumKeys = brother->KeyCounts;
            bTreeNode->keys[0] = parentNode->keys[position - 1];
            parentNode->keys[position - 1] = brother->keys[brotherNumKeys - 1];
        }

        bTreeNode->ptrs[0] = brother->ptrs[brotherNumKeys];
        brother->keys[brotherNumKeys] = empty;
        brother->ptrs[brotherNumKeys] = NULL;
    } else {
        int temp = brother->KeyCounts;

        // Get one from right
        if (bTreeNode->isLeaf) {
            bTreeNode->keys[NumKeys] = brother->keys[0];
            bTreeNode->ptrs[NumKeys] = brother->ptrs[0];
            parentNode->keys[0] = brother->keys[1];
        } else {
            bTreeNode->keys[NumKeys] = parentNode->keys[0];
            bTreeNode->ptrs[NumKeys + 1] = brother->ptrs[0];
            parentNode->keys[0] = brother->keys[0];
        }

        // Shift to left by 1
        for (i = 0; i < temp; i++) {
            brother->keys[i] = brother->keys[i + 1];
            globalPos = brother->KeyCounts;
            brother->ptrs[i] = brother->ptrs[i + 1];
        }

        brother->ptrs[brother->KeyCounts] = NULL;
        brother->keys[brother->KeyCounts] = empty;
    }

    bTreeNode->KeyCounts++;
    brother->KeyCounts--;
    return RC_OK;
}


//---------------------------------------------------------------------------------------------------------------------------
// init and shutdown index manager
//--> This function initializes the index manager.
//--> We call initStorageManager(...) function of Storage Manager to initialize the storage manager. 
RC initIndexManager (void *mgmtData)
{
    root = NULL;
    numNodValue = 0;
    sizeofNod = 0;
    empty.dt = DT_INT;
    empty.v.intV = 0;
    return RC_OK;
}

//This function shuts down the index manager and de-allocates all the resources allocated to the index manager.
RC shutdownIndexManager ()
{
    return RC_OK;
}


//This function creates a new B+ Tree.
//It initializes the TreeManager structure which stores additional information of our B+ Tree.
RC createBtree(char *idxId, DataType keyType, int n) {
    if (idxId == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    RC rc;

    // Create a page file
    rc = createPageFile(idxId);
    if (rc != RC_OK) {
        return rc;
    }

    // Open the page file
    SM_FileHandle fhandle;
    rc = openPageFile(idxId, &fhandle);
    if (rc != RC_OK) {
        return rc;
    }

    // Allocate a buffer for the metadata
    SM_PageHandle pageData = (SM_PageHandle)malloc(PAGE_SIZE);
    if (pageData == NULL) {
        closePageFile(&fhandle);
        return RC_MEMORY_ALLOCATION_ERROR;
    }

    // Copy metadata into the buffer
    memcpy(pageData, &keyType, sizeof(DataType));
    memcpy(pageData + sizeof(DataType), &n, sizeof(int));

    // Write the metadata to the first page of the file
    rc = writeBlock(0, &fhandle, pageData);
    free(pageData);

    // Close the page file
    closePageFile(&fhandle);

    return rc;
}


//This function opens an existing B+ Tree which is stored on the file specified by "idxId" parameter.
//We retrieve our TreeManager and initialize the Buffer Pool.
RC openBtree(BTreeHandle **tree, char *idxId) {
    if (idxId == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    RC rc;

    // Create a new BTreeHandle
    *tree = (BTreeHandle *)malloc(sizeof(BTreeHandle));
    if (*tree == NULL) {
        return RC_MEMORY_ALLOCATION_ERROR;
    }

    // Create and initialize a buffer pool
    BM_BufferPool *bm = MAKE_POOL();
    rc = initBufferPool(bm, idxId, 10, RS_CLOCK, NULL);
    if (rc != RC_OK) {
        free(*tree);
        return rc;
    }

    // Pin the first page
    BM_PageHandle *page = MAKE_PAGE_HANDLE();
    rc = pinPage(bm, page, 0);
    if (rc != RC_OK) {
        free(*tree);
        shutdownBufferPool(bm);
        return rc;
    }

    // Extract metadata from the first page
    int type;
    memcpy(&type, page->data, sizeof(int));
    (*tree)->keyType = (DataType)type;

    page->data += sizeof(int);
    int n;
    memcpy(&n, page->data, sizeof(int));
    page->data -= sizeof(int);

    // Create and set up BTree management data
    RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)malloc(sizeof(RM_bTree_mgmtData));
    if (bTreeMgmt == NULL) {
        free(*tree);
        free(page);
        shutdownBufferPool(bm);
        return RC_MEMORY_ALLOCATION_ERROR;
    }

    bTreeMgmt->numEntries = 0;
    bTreeMgmt->maxKeyNum = n;
    bTreeMgmt->bp = bm;
    (*tree)->mgmtData = bTreeMgmt;

    // Clean up and return
    free(page);
    return RC_OK;
}


//This function closes the B+ Tree.
//It marks all the pages dirty so that they can be written back to disk by the Buffer Manager.
RC closeBtree(BTreeHandle *tree) {
    if (tree == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    RC rc;

    tree->idxId = NULL;
    RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)tree->mgmtData;

    rc = shutdownBufferPool(bTreeMgmt->bp);
    if (rc != RC_OK) {
        return rc;
    }

    free(bTreeMgmt);
    free(tree);
    free(root);  // Assuming root is a global variable
    root = NULL;

    return RC_OK;
}

//This function deletes the page file having the specified file name "idxId" in the parameter. It uses Storage Manager for this purpose.
RC deleteBtree(char *idxId) {
    if (idxId == NULL) return RC_IM_KEY_NOT_FOUND;

    // Destroy the page file
    RC rc = destroyPageFile(idxId);
    if (rc != RC_OK) return rc;

    return RC_OK;
}


// access information about a b-tree


//This function returns the number of nodes present in our B+ Tree.
RC getNumNodes(BTreeHandle *tree, int *result) {
    if (tree == NULL || result == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    *result = numNodValue;

    return RC_OK;
}

//This function returns the number of entries/records/keys present in our B+ Tree.
RC getNumEntries(BTreeHandle *tree, int *result) {
    if (tree == NULL || result == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    *result = ((RM_bTree_mgmtData *)tree->mgmtData)->numEntries;

    return RC_OK;
}

//This function returns the datatype of the keys being stored in our B+ Tree.
RC getKeyType(BTreeHandle *tree, DataType *result) {
    if (tree == NULL || result == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    *result = tree->keyType;

    return RC_OK;
}


// index access
//This method searches the B+ Tree for the key specified in the parameter.
RC findKey(BTreeHandle *tree, Value *key, RID *result) {
    if (tree == NULL || key == NULL || root == NULL) {
        return RC_IM_KEY_NOT_FOUND;
    }

    RM_BtreeNode *leaf = root;
    int i = 0;

    // Find leaf
    while (!leaf->isLeaf) {
        while (i < leaf->KeyCounts && strNoLarger(serializeValue(&leaf->keys[i]), serializeValue(key))) {
            i++;
        }
        leaf = (RM_BtreeNode *)leaf->ptrs[i];
        i = 0;
    }

    // Search within the leaf
    while (i < leaf->KeyCounts && strcmp(serializeValue(&leaf->keys[i]), serializeValue(key)) != 0) {
        i++;
    }

    if (i >= leaf->KeyCounts) {
        return RC_IM_KEY_NOT_FOUND;
    } else {
        result->page = ((RID *)leaf->ptrs[i])->page;
        result->slot = ((RID *)leaf->ptrs[i])->slot;
        return RC_OK;
    }
}

//This function adds a new entry/record with the specified key and RID.
RC insertKey (BTreeHandle *tree, Value *key, RID rid)
{
    if ((tree == NULL)||(key == NULL)) return RC_IM_KEY_NOT_FOUND;
    
    RM_BtreeNode *leaf;
    int i = 0;
    
    //findleaf
    leaf = root;
    if (leaf != NULL)
    {
        while (!leaf->isLeaf)
        {
            //--
            sv = serializeValue(&leaf->keys[i]);
            sv2 = serializeValue(key);
            while ((i < leaf->KeyCounts) && strNoLarger(sv, sv2)){
                free(sv);
                sv = NULL;
                i++;
                if (i < leaf->KeyCounts) sv = serializeValue(&leaf->keys[i]);
            }
            free(sv);
            sv = NULL;
            free(sv2);
            sv2 = NULL;
            //--
            leaf = (RM_BtreeNode *)leaf->ptrs[i];
            i = 0;
        }
    }
    
    RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)tree->mgmtData;
    bTreeMgmt->numEntries += 1;
    
    if (!leaf)
    {
        sizeofNod = bTreeMgmt->maxKeyNum + 1;
        
        root = createNewNod(root);
        RID *rec = (RID *) malloc (sizeof(RID));
        rec->page = rid.page;
        rec->slot = rid.slot;
        root->ptrs[0] = rec;
        root->keys[0] = *key;
        root->ptrs[sizeofNod - 1] = NULL;
        root->isLeaf = true;
        root->KeyCounts++;
    }
    else
    {
        int index = 0;
        //--
        sv = serializeValue(&leaf->keys[index]);
        sv2 = serializeValue(key);
        while ((index < leaf->KeyCounts) && strNoLarger(sv, sv2)){
            free(sv);
            sv = NULL;
            index++;
            if (index < leaf->KeyCounts) sv = serializeValue(&leaf->keys[index]);
        }
        free(sv);
        sv = NULL;
        free(sv2);
        sv2 = NULL;
        //--
        
        if (leaf->KeyCounts < sizeofNod - 1)
        {
            //empty slot
            for (int i = leaf->KeyCounts;i > index;i--){
                leaf->keys[i] = leaf->keys[i-1];
                globalPos = leaf->pos;
                leaf->ptrs[i] = leaf->ptrs[i-1];
            }
            RID *rec = (RID *) malloc (sizeof(RID));
            rec->page = rid.page;
            rec->slot = rid.slot;
            leaf->keys[index] = *key;
            leaf->ptrs[index] = rec;
            leaf->KeyCounts += 1;
        }
        else
        {
            RM_BtreeNode *newLeafNod;
            RID **NodeRID;
            Value *NodeKeys;
            int middleLoc = 0;
            NodeRID = malloc(sizeofNod * sizeof(RID *));
            NodeKeys = malloc(sizeofNod * sizeof(Value));
            //full node
            for (i=0;i < sizeofNod;i++)
            {
                if (i == index){
                    RID *newValue = (RID *) malloc (sizeof(RID));
                    newValue->page = rid.page;
                    newValue->slot = rid.slot;
                    NodeRID[i] = newValue;
                    NodeKeys[i] = *key;
                }
                else if (i < index){
                    NodeRID[i] = leaf->ptrs[i];
                    globalPos = NodeRID[i]->page;
                    NodeKeys[i] = leaf->keys[i];
                    middleLoc = sizeofNod % 2 == 0;
                }
                else{//i > index
                    middleLoc = globalPos;
                    NodeRID[i] = leaf->ptrs[i-1];
                    globalPos = NodeRID[i]->page;
                    NodeKeys[i] = leaf->keys[i-1];
                }
            }
            
            middleLoc = sizeofNod / 2 + 1;
            //old leaf
            for (i=0;i < middleLoc;i++)
            {
                leaf->ptrs[i] = NodeRID[i];
                leaf->keys[i] = NodeKeys[i];
            }
            //new leaf
            newLeafNod = createNewNod(newLeafNod);
            newLeafNod->isLeaf = true;
            newLeafNod->paptr = leaf->paptr;
            newLeafNod->KeyCounts = sizeofNod - middleLoc;
            for (i=middleLoc;i < sizeofNod;i++){
                newLeafNod->ptrs[i - middleLoc] = NodeRID[i];
                newLeafNod->keys[i - middleLoc] = NodeKeys[i];
            }
            //add to link list
            newLeafNod->ptrs[sizeofNod - 1] = leaf->ptrs[sizeofNod - 1];
            leaf->KeyCounts = middleLoc;
            leaf->ptrs[sizeofNod - 1] = newLeafNod;
            
            free(NodeRID);
            NodeRID = NULL;
            free(NodeKeys);
            NodeKeys = NULL;
            
            RC rc = insertParent(leaf, newLeafNod, newLeafNod->keys[0]);
            if (rc != RC_OK)
                return rc;
        }
    }
    
    tree->mgmtData = bTreeMgmt;
    return RC_OK;
}

//This function deletes the entry/record with the specified "key" in the B+ Tree.
RC deleteKey (BTreeHandle *tree, Value *key)
{
    if ((tree == NULL)||(key == NULL)) return RC_IM_KEY_NOT_FOUND;
    RC rc;
    RM_bTree_mgmtData *bTreeMgmt = (RM_bTree_mgmtData *)tree->mgmtData;
    bTreeMgmt->numEntries -= 1;
    RM_BtreeNode *leaf;
    int i = 0;
    //findleaf then delete
    leaf = root;
    if (leaf != NULL)
    {
        while (!leaf->isLeaf)
        {
            //--
            sv = serializeValue(&leaf->keys[i]);
            sv2 = serializeValue(key);
            while ((i < leaf->KeyCounts) && strNoLarger(sv, sv2)){
                free(sv);
                sv = NULL;
                i++;
                if (i < leaf->KeyCounts) sv = serializeValue(&leaf->keys[i]);
            }
            free(sv);
            sv = NULL;
            free(sv2);
            sv2 = NULL;
            //--
            leaf = (RM_BtreeNode *)leaf->ptrs[i];
            i = 0;
        }
        //--
        sv = serializeValue(&leaf->keys[i]);
        sv2 = serializeValue(key);
        while ((i < leaf->KeyCounts) && (strcmp(sv, sv2)!=0)){
            free(sv);
            sv = NULL;
            i++;
            if (i < leaf->KeyCounts) sv = serializeValue(&leaf->keys[i]);
        }
        free(sv);
        sv = NULL;
        free(sv2);
        sv2 = NULL;
        //--
        if (i < leaf->KeyCounts)
        {
            rc = deleteNode(leaf, i);
            if (rc != RC_OK)
                return rc;
        }
    }
    
    tree->mgmtData = bTreeMgmt;
    return RC_OK;
}


//This function initializes the scan which is used to scan the entries in the B+ Tree in the sorted key order
RC openTreeScan(BTreeHandle *tree, BT_ScanHandle **handle) {
    if (tree == NULL) return RC_IM_KEY_NOT_FOUND;

    *handle = (BT_ScanHandle *)malloc(sizeof(BT_ScanHandle));
    if (*handle == NULL) return RC_MALLOC_FAILED;

    (*handle)->tree = tree;
    (*handle)->mgmtData = (RM_BScan_mgmt *)malloc(sizeof(RM_BScan_mgmt));
    if ((*handle)->mgmtData == NULL) {
        free(*handle);
        return RC_MALLOC_FAILED;
    }

    ((RM_BScan_mgmt *)(*handle)->mgmtData)->cur = NULL;
    ((RM_BScan_mgmt *)(*handle)->mgmtData)->index = 0;
    ((RM_BScan_mgmt *)(*handle)->mgmtData)->totalScan = 0;

    return RC_OK;
}

//This function is used to traverse the entries in the B+ Tree.
RC nextEntry (BT_ScanHandle *handle, RID *result)
{
    if(handle == NULL) return RC_IM_KEY_NOT_FOUND;
    RC rc;
    RM_BScan_mgmt *scanMgmt = (RM_BScan_mgmt *) handle->mgmtData;
    int totalRes = 0;
    
    rc = getNumEntries(handle->tree, &totalRes);
    if (rc != RC_OK)
        return rc;
    if(scanMgmt->totalScan >= totalRes){
        return RC_IM_NO_MORE_ENTRIES;
    }
    
    RM_BtreeNode *leaf = root;
    if(scanMgmt->totalScan == 0){
        while (!leaf->isLeaf)
            leaf = leaf->ptrs[0];
        scanMgmt->cur = leaf;
    }
    
    if(scanMgmt->index == scanMgmt->cur->KeyCounts){
        scanMgmt->cur = (RM_BtreeNode *)scanMgmt->cur->ptrs[((RM_bTree_mgmtData *)handle->tree->mgmtData)->maxKeyNum];
        scanMgmt->index = 0;
    }
    
    RID *ridRes = (RID *) malloc (sizeof(RID));
    ridRes = (RID *)scanMgmt->cur->ptrs[scanMgmt->index];
    scanMgmt->index++;
    scanMgmt->totalScan++;
    handle->mgmtData = scanMgmt;
    
    result->page = ridRes->page;
    result->slot = ridRes->slot;
    
    return RC_OK;
}

//This function closes the scan mechanism and frees up resources
RC closeTreeScan(BT_ScanHandle *handle) {
    if (handle == NULL) return RC_OK; // Already closed or invalid handle
    
    free(handle->mgmtData);
    free(handle);
    
    return RC_OK;
}


// debug and test functions

int DFS(RM_BtreeNode *bTreeNode) {
    if (bTreeNode == NULL) return 0;  // Check if the node is NULL
    
    bTreeNode->pos = globalPos++;
    
    if (!bTreeNode->isLeaf) {
        for (int i = 0; i <= bTreeNode->KeyCounts; i++) {
            DFS(bTreeNode->ptrs[i]);
        }
    }
    
    return 0;
}


int walk(RM_BtreeNode *bTreeNode, char *result) {
    char line[100];
    sprintf(line, "(%d)[", bTreeNode->pos);

    if (bTreeNode->isLeaf) {
        // Leaf Node
        for (int i = 0; i < bTreeNode->KeyCounts; i++) {
            // RID
            sprintf(line + strlen(line), "%d.%d,", ((RID *)bTreeNode->ptrs[i])->page, ((RID *)bTreeNode->ptrs[i])->slot);
            // Key
            sv = serializeValue(&bTreeNode->keys[i]);
            strcat(line, sv);
            free(sv);
            sv = NULL;
            strcat(line, ",");
        }

        if (bTreeNode->ptrs[sizeofNod - 1] != NULL) {
            // Pos
            sprintf(line + strlen(line), "%d", ((RM_BtreeNode *)bTreeNode->ptrs[sizeofNod - 1])->pos);
        } else {
            line[strlen(line) - 1] = 0; // Delete ","
        }
    } else {
        // Non-Leaf Node
        for (int i = 0; i <= bTreeNode->KeyCounts; i++) {
            sprintf(line + strlen(line), "%d,", ((RM_BtreeNode *)bTreeNode->ptrs[i])->pos);

            sv = serializeValue(&bTreeNode->keys[i]);
            strcat(line, sv);
            free(sv);
            sv = NULL;
            strcat(line, ",");
        }

        if (((RM_BtreeNode *)bTreeNode->ptrs[bTreeNode->KeyCounts]) != NULL) {
            sprintf(line + strlen(line), "%d", ((RM_BtreeNode *)bTreeNode->ptrs[bTreeNode->KeyCounts])->pos);
        } else {
            line[strlen(line) - 1] = 0; // Delete ","
        }
    }

    strcat(line, "]\n");
    strcat(result, line);

    if (!bTreeNode->isLeaf) {
        for (int i = 0; i <= bTreeNode->KeyCounts; i++) {
            walk(bTreeNode->ptrs[i], result);
        }
    }

    return 0;
}


char *printTree (BTreeHandle *tree)
{
    if (root == NULL)
        return NULL;
    globalPos = 0;
    int lenth = DFS(root);
    lenth = 1000;
    char *result = malloc(lenth*sizeof(char));
    walk(root, result);
    return result;
}