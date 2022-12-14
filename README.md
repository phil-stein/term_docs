
# term docs

** [!] not finished see [todo](#todo) for planned features **
** [!] only tested on windows **

c documentation for the terminal <br>
works offline and is customizable <br>
also has seqarch utility functions <br>
most documentation is based on [tutorialspoint](https://www.tutorialspoint.co://www.tutorialspoint.com/c_standard_library/index.htm)

|doc    |search  |
|:-----:|:------:|
| <img src="https://github.com/phil-stein/term_docs/blob/main/screenshots/screenshot_doc01.png" alt="logo" width="600"> | <img src="https://github.com/phil-stein/term_docs/blob/main/screenshots/screenshot_search01.png" alt="logo" width="400"> |


## table of contents
  - [features](#features)
  - [example](#example)
  - [instalation](#instalation)
  - [troubleshoot](#troubleshoot)
  - [customization](#customization)
  - [todo](#todo)


## features
  - search documentation for standard c functions
  - search function definitions in specified dir
  - syntax highlighting


## example

```c

doc -h -help    -> help
doc -c -color   -> disable syntax highlighting  

'-abc' modifiers always at the end
example:
  doc malloc -h -c

doc [keyword]  -> documentation
example:
  doc malloc

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

doc [dir] [keyword]   -> search
example:
  doc ../code function 

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
    -> type `make` into terminal in root dir
  3. add root/build directory to your path

## troubleshoot
  - output text is messed up
    try adding '-c' to your command to disable syntax highlighting, <br>
    some terminals don't support it, like command prompt on windows <br>
    on windows the new windows terminal, found in microsoft stor, supports it

## customization

  - [custom documentation](#custom-documentation)
  - [custom executable name](#custom-executable-name)

### custom documentation
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

to include a '#' symbol in your documentation you need to escape it ``\#define`` <br>
the same applied to `` ! ~ ? ``  `` \! \~ \? `` hides them in the documentation


### custom executable name
in the makefile in the root directory at the top there is a variable called NAME `NAME = doc.exe` <br>
change this and call `make clean` and `make` in the root directory

## todo 
  - search structure definitions in specified dir
  - search function / structure references
  - incomplete search, i.e. func_ -> func_a, func_b, ...
  - standard c documentation
    - [ ] basics e.g. types, macros, flow control, etc. [wip]
    - [x] ctype.h
    - [x] stdlib.h
    - [x] string.h
    - [ ] stdio.h
    - [ ] math.h
    - [ ] assert.h
    - [ ] stdarg.h
    - [ ] time.h
    - [ ] float.h
    - [ ] erno.h 
    - [ ] limits.h
    - [ ] locale.h
    - [ ] signal.h
    - [ ] setjmp.h
    - [ ] compiler e.g. make, gcc, clang, msvc
    - [ ] vim cheatsheet


