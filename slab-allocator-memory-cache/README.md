Memory management library for Diameter protocol stack.  Implementation was inspired by the Linux kernel's slab allocator, where frequently used objects (e.g., user sessions, callouts) are cached and managed as a pool of objects.  Having caches of commonly and frequently used objects in an initialized state available for use eliminates overhead spent on allocating, initializing, and freeing memory, thereby improving overall performance.  Written in C.