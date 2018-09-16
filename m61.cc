#define M61_DISABLE 1
#include "m61.hh"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include <iostream>

using namespace std;

m61_statistics stats;


/// m61_malloc(sz, file, line)
///    Return a pointer to `sz` bytes of newly-allocated dynamic memory.
///    The memory is not initialized. If `sz == 0`, then m61_malloc must
///    return a unique, newly-allocated pointer value. The allocation
///    request was at location `file`:`line`.

void* m61_malloc(size_t sz, const char* file, long line)
{
    (void) file, (void) line;   // avoid uninitialized variable warnings
    
     if (sz > 4294967145)
      {
	stats.fail_size += sz;
	stats.nfail +=1;
	return 0;
      }
    
    stats.ntotal += 1;
    
    stats.total_size += sz;
        
    mallocPtrs* ptr = (mallocPtrs*) base_malloc(sz);

    ptr->allocsz = sz;
    ptr->validptrflag =  1;
    printf("%p\n", ptr);
    printf("%d\n", ptr->validptrflag);
    printf("%lu\n", ptr->allocsz);
    

    //printf("%d\n", ptr->allocsz);
    
    uintptr_t statptr = (uintptr_t) ptr;

    statptr += sz;
    
    if (stats.heap_min == 0)
      stats.heap_min = (char*) ptr;
    
    if (stats.heap_max == 0)
      stats.heap_max = (char*) statptr;
                                               
    if ((char*) ptr < stats.heap_min)
      stats.heap_min = (char*) ptr;
    
    if ((char*) statptr > stats.heap_max)
      stats.heap_max = (char*) statptr;

    printf("%p\n", ptr);
    printf("%d\n", ptr->validptrflag);
    printf("%lu\n", ptr->allocsz);

    
    return ptr;
    
   
}


/// m61_free(ptr, file, line)
///    Free the memory space pointed to by `ptr`, which must have been
///    returned by a previous call to m61_malloc. If `ptr == NULL`,
///    does nothing. The free was called at location `file`:`line`.

void m61_free(void* ptr, const char *file, long line) {
  //(void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.

  mallocPtrs* ptr2 = (mallocPtrs*) ptr;

              printf("%p\n", ptr2);

	      printf("%d\n", ptr2->validptrflag);

              printf("%lu\n", ptr2->allocsz);

    
   if (ptr2 == NULL)
     return;

   if (stats.heap_min <= (char*) ptr2)
	{
	  if (stats.heap_max >= (char*) ptr2)
	    {
	      /*mallocPtrs* ptr2 = (mallocPtrs*) ptr;
	      printf("%p\n", ptr2);

	      printf("%d\n", ptr2->validptrflag);

	      printf("%d\n\n", ptr2->allocsz);*/
	      
	      if (ptr2->validptrflag == 1)
		{
		  stats.frees += 1;

		  stats.frees_size += ptr2->allocsz;
		  printf("%llu\n\n", stats.frees_size);

		  ptr2->validptrflag = 0;
	    
		  base_free(ptr2);
		}
	      else
		
		  printf("MEMORY BUG: test%d.cc:%lu: invalid free of pointer ???, not allocated\n", *file, line);
	    }
	  else
	    printf("MEMORY BUG: test%d.cc:%lu: invalid free of pointer ???, not in heap\n");

	}
   else
     printf("MEMORY BUG: test%d.cc:%lu: invalid free of pointer ???, not in h\
eap\n");

        
}


/// m61_calloc(nmemb, sz, file, line)
///    Return a pointer to newly-allocated dynamic memory big enough to
///    hold an array of `nmemb` elements of `sz` bytes each. If `sz == 0`,
///    then must return a unique, newly-allocated pointer value. Returned
///    memory should be initialized to zero. The allocation request was at
///    location `file`:`line`.

void* m61_calloc(size_t nmemb, size_t sz, const char* file, long line) {
    // Your code here (to fix test016).
 
  if (nmemb < 500000000)
    {
      void* ptr = m61_malloc(nmemb * sz, file, line);
        if (ptr) {
          memset(ptr, 0, nmemb * sz);
	}
	return ptr;
    }
  else
    {
      stats.fail_size += (nmemb * sz);
      stats.nfail += 1;
      void* ptr = NULL;
      return ptr;
    }
}


/// m61_getstatistics(stats)
///    Store the current memory statistics in `*stats`.

void m61_getstatistics(m61_statistics* stat) {

  stat->heap_min = stats.heap_min;
  stat->heap_max = stats.heap_max;
}


/// m61_printstatistics()
///    Print the current memory statistics.

void m61_printstatistics() {
  
    stats.nactive = stats.ntotal - stats.frees;
    
    stats.active_size = stats.total_size - stats.frees_size;
    
    printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
	   stats.active_size, stats.total_size, stats.fail_size);
}

/// m61_printleakreport()
///    Print a report of all currently-active allocated blocks of dynamic
///    memory.

void m61_printleakreport() {
    // Your code here.
}


thread_local const char* m61_file = "?";
thread_local int m61_line = 1;

void* operator new(size_t sz) {
    return m61_malloc(sz, m61_file, m61_line);
}
void* operator new[](size_t sz) {
    return m61_malloc(sz, m61_file, m61_line);
}
void operator delete(void* ptr) noexcept {
    m61_free(ptr, m61_file, m61_line);
}
void operator delete(void* ptr, size_t) noexcept {
    m61_free(ptr, m61_file, m61_line);
}
void operator delete[](void* ptr) noexcept {
    m61_free(ptr, m61_file, m61_line);
}
void operator delete[](void* ptr, size_t) noexcept {
    m61_free(ptr, m61_file, m61_line);
}
