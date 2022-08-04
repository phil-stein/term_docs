
# term docs

**[not finished see [todo](#todo) for planned features]**

c documentation for the terminal <br>
works offline and is customizable <br>


## table of contents
  - [features](#features)
  - [example](#example)
  - [instalation](#instalation)
  - [troubleshoot](#troubleshoot)
  - [custimization](#custimization)
  - [todo](#todo)


## features
  - search documentation for standard c functions
  - search function definitions in specified dir
  - syntax highlighting


## example

```c

doc -h -help    -> help
doc -c -color   -> disable syntax highlighting  

doc malloc  -> documentation

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

doc ../code function -> search

function -----------------------------------------

// comment
void function(int arg);
  -> file: 'src/file0.h' line: 34
  
void function(char arg);
  -> file: 'external/file1.h' line: 92

--------------------------------------------------

```

## instalation
  1. clone git repo
  2. use gcc & make to compile <br>
    -> type ``make`` into terminal in root dir
  3. add root/build directory to your path

## troubleshoot
  - output text is messed up
    try adding '-c' to your command to disable syntax highlighting, <br>
    some terminals don't support it, like command prompt on windows <br>
    on windows the new windows terminal, found in microsoft stor, supports it

## custimization
the documentation in in the '.sheet' file in the 'sheets' folder <br>
adding a new file here lets you add any documentation, using a custom markup style <br>

```
#
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

?< https://www.tutorialspoint.com/c_standard_library/c_function_malloc.htm >?
#
```
'#' starts and ends a section of documentation <br>
'|' starts and ends a tag which is what the search engine looks for <br>
'!' warning / hint <br>
'~' info <br>
'?' link <br>


## todo 
  - search structure definitions in specified dir
  - search function / structure references


