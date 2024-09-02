# BF-Tree implementation

A implementation of BF-Tree: A Modern Read-Write-Optimized Concurrent Larger-Than-Memory Range Index mentioned on VLDB 2024

# MiniPage

the MiniPage class manages the CRUD of a mini-page, it contains a buffer means the mini-page layout and meta pointer reference to the meta in the buffer, we set the record size in the meta, we can get the free kv meta offset by: sizeof(PageMeta) + recordCount * sizeof(KVMeta).

Also we record the offset in the class.