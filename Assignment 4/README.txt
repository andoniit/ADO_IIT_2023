RUNNING THE SCRIPT
=======================================

1) Go to Project root (assign4) using Terminal.

2) Type ls to list the files and check that we are in the correct directory.

3) Type "make clean" to delete old compiled .o files.

4) Type "make" to compile all project files including "test_assign4_1.c" file 

5) Type ./test_assin4 to run the project


1. CUSTOM B+ TREE FUNCTIONS (btree_creator.h)
=================================================
These functions have bee defined to perform insert/delete/find/print operations on our B+ Tree.



createNewNod(...)
--> This function creates a new general node, which can be adapted to serve as a leaf/internal/root node.


insertParent(...)
--> This function inserts a new node (leaf or internal node) into the B+ tree.
--> It returns the root of the tree after insertion.



deleteNode(...)
--> This function deletes the the entry/record having the specified key.




2. INITIALIZE AND SHUTDOWN INDEX MANAGER
=================================================

These functions are used to initialize the index manager and shut it down, freeing up all the acquired resources.

initIndexManager(...)
--> This function initializes the index manager.
--> We call initStorageManager(...) function of Storage Manager to initialize the storage manager. 

shutdownIndexManager(...)
--> This function shuts down the index manager and de-allocates all the resources allocated to the index manager.
--> It free up all resources/memory space being used by the Index Manager.
--> We set the treeManager data structure pointer to NULL and call C function free() to de-allocate memory space


2. B+ TREE INDEX RELATED FUNCTIONS
=================================================

These functions are used to create or delete a b-tree index.

createBtree(...)
--> This function creates a new B+ Tree.
--> It initializes the TreeManager structure which stores additional information of our B+ Tree.
--> We initialize the buffer manager, create a buffer pool using Buffer Manager and also create a page with the specified page name "idxId" using Storage Manager.

openBtree(...)
--> This function opens an existing B+ Tree which is stored on the file specified by "idxId" parameter.
--> We retrieve our TreeManager and initialize the Buffer Pool.

closeBtree(...)
--> This function closes the B+ Tree.
--> It marks all the pages dirty so that they can be written back to disk by the Buffer Manager.
--> It then shuts down the buffer pool and frees up all the allocated resources.

deleteBtree(....)
--> This function deletes the page file having the specified file name "idxId" in the parameter. It uses Storage Manager for this purpose.


3. ACCESS INFORMATION ABOUT OUR B+ TREE
=================================================

These functions help us get information about our B+ Tree like number of nodes and keys in our tree.

getNumNodes(...)
--> This function returns the number of nodes present in our B+ Tree.
--> We store this information in our TreeManager structure in "numNodes" variable. So, we just return this data.

getNumEntries(...)
--> This function returns the number of entries/records/keys present in our B+ Tree.
--> We store this information in our TreeManager structure in "numEntries" variable. So, we just return this data.

getKeyType(...) 
--> This function returns the datatype of the keys being stored in our B+ Tree.
--> We store this information in our TreeManager structure in "keyType" variable. So, we just return this data.


4. ACCESSING B+ TREE FUNCTIONS
=========================================

These functions are used to find, insert, and delete keys in/from a given B+ Tree. Also, we can scan through all entries of a B+ Tree in sorted order using the openTreeScan, nextEntry, and closeTreeScan methods.

findKey(...)
--> This method searches the B+ Tree for the key specified in the parameter.
--> If an entry with the specified key is found, we store the RID (value) for that key in the memory location pointed by "result" parameter.
--> We call findRecord(..) method which serves the purpose. If findRecord(..) returns NULL, it means the key is not there in B+ Tree and we return error code RC_IM_KEY_NOT_FOUND.

insertKey(...)
--> This function adds a new entry/record with the specified key and RID.
--> We first, search the B+ Tree for the specified key. If it is found, then we return error code RC_IM_KEY_ALREADY_EXISTS.
--> If not found, then we create a NodeData structure which stores the RID.
--> We check if root of the tree is empty. If it's empty, then we call createNewTree(..) which creates a new B+ Tree and adds this entry to the tree.
--> If our tree has a root element i.e. tree was already there, then we locate the leaf node where this entry can be inserted.
--> Once the leaf node has been found, then we check if it has room for the new entry. If yes, then we call insertIntoLeaf(...) which performs the insertion.
--> If the leaf node is full, the we call insertIntoLeafAfterSplitting(...) which splits the leaf node and then inserts the entry.

deleteKey(...)
--> This function deletes the entry/record with the specified "key" in the B+ Tree.
--> We call our B+ Tree method delete(...) as explained above. This function deletes the entry/key from the tree and adjusts the tree accordingly so as to maintain the B+ Tree properties.

openTreeScan(...)
--> This function initializes the scan which is used to scan the entries in the B+ Tree in the sorted key order.
--> This function initializes our ScanManager structure which stores extra information for performing the scan operation. 
--> If the root node of the B+ Tree is NULL, then we return error code RC_NO_RECORDS_TO_SCAN.

nextEntry(...)
--> This function is used to traverse the entries in the B+ Tree.
--> It stores the record details i.e. RID in the memory location pointed by "result" parameter.
--> If all the entries have been scanned and there are no more entries left, then we return error code RC_IM_NO_MORE_ENTRIES;

closeTreeScan(...)
--> .This function closes the scan mechanism and frees up resources


5. DEBUGGING AND TEST FUNCTIONS
=========================================

These functions are used for debugging purpose.  

printTree(...)
--> This function prints the B+ Tree

=======================================
