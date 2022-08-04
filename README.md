
# term docs

c documentation for the terminal
works offline and is very customizable

```

doc malloc           -> documentation

doc ../code function -> search

```
```
stdlib.sheet|malloc -------------------------------

|malloc|
|allocate|memory|dynamic memory|
void* malloc(size_t size)
    allocate the specified amount of memory.
    ! needs to be freed using [free]
    ! size is in bytes
    ! returns NULL on fail
    ~ included in <stdlib.h>
    example:
      // array now has space for 10 int's
      int* array = malloc(10 * sizeof(int));

  < https://www.tutorialspoint.com/c_standard_library/c_function_malloc.htm >

--------------------------------------------------
```



## features
  - search documentation for standard c functions
  - search function definitions in specified dir

## todo 
  - search structure definitions in specified dir
  - search function / structure references


