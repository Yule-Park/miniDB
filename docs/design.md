The purpose of the Disk Manager is to manage the physical storage of the pages. To allocate space for each page, a page allocation function was provided, where it will allocate space only at the end of the file. In doing so, it avoids shifting already existing pages which keeps page locations stable. Each new page is zero-initialized to ensure that new pages just allocated start in a known state, and each page is located by multiples of 4096, since each page is size 4096 bytes. More specifically, byte offset can be calculated by multiplying page id by 4096, which would locate each page by page id. A read and write method was provided to access and write into each page. Errors are handled by throwing exceptions of different types so the caller knows what went wrong, while incomplete files are rejected because existing files whose sizes are not divisible by 4096 violates storage format and may indicate a truncated or corrupted file. We also maintain data integrity by implementing the DiskManager such that files will be preserved even when the program is not running so that existing content on the database is not erased. Finally, DiskManager's only responsibility is to allocate pages and transfer their bytes between the RAM and the database file, other functions such as organizing keys and pointers are the responsibility of other components. 

Testing: 
- DiskManager is validated using 9 Catch2 test cases that cover the following: 
- Page allocation and file growth sequentially
- Zero initialization of allocated new pages
- Reading/writing pages
- Pages do not influence each other
- Invalid page ID rejection
- Persistent data across restarts
- Persistent allocations across restarts
- Corrupted database file rejection
- Repeated input/output handling