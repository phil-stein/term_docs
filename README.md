
# term docs

[!] see [todo](#todo) for planned features<br>
[!] only tested on windows<br>

c documentation for the terminal <br>
works offline and is customizable <br>
also has search utility functions <br>
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
    - [custom documentation](#custom-documentation)
    - [custom executable name](#custom-executable-name)
    - [config file](#config-file)
  - [todo](#todo)


## features
  - search documentation
    - basic c functionality/types/etc. 
    - all standard c functions
    - git
    - gcc
    - [stb_ds](https://github.com/nothings/stb)
    - my own: [global](https://github.com/phil-stein/global)
  - search function definitions in specified dir
  - syntax highlighting & markup language
  - config file


## example

```c

doc -h -help    -> help
doc -c -color   -> disable syntax highlighting
doc -d          -> search for func defenition
doc -config     -> print config file

modifier can have arbitrary position

example:
  doc malloc -c
  doc -c malloc
  doc -c malloc -config stdlib

doc [keyword1] [keyword2] ...  -> documentation
example:
  doc git log
  -> finds all docs with both git and log tags
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

doc [dir] [keyword] -d  -> search
example:
  doc ../code function -d 
  doc -d ../code function 

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
  2. use cmake & make / vs19 to build <br>
    -> in root call `cmake_build`
    -> make: call `build`
    -> vs19: go to `build/vs19/doc.sln`
      -> compile
  4. add root/build directory to your [path](https://stackoverflow.com/questions/44272416/how-to-add-a-folder-to-path-environment-variable-in-windows-10-with-screensho)

## troubleshoot
  - output text is messed up
    try adding '-c' to your command to disable syntax highlighting, <br>
    some terminals don't support it, like command prompt on windows <br>
    on windows the new windows terminal, found in microsoft store, supports it <br>
    see [config](#config) to permanently disable highlighting <br>
  - documentation doesnt show up, check if its surrounded by `#`<br>
    and the tags are surrounded in `|`

## customization

  - [custom documentation](#custom-documentation)
  - [custom executable name](#custom-executable-name)
  - [config file](#config-file)
  
### custom documentation
the documentation in in the '.sheet' file in the 'sheets' folder <br>
adding a new file here lets you add any documentation, using a custom markup style <br>
you can add more paths in the [config file](#config-file) <br>

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

to include a '#' symbol in your documentation you need to escape it ``\\#``, <br>
or escape once for macros ``\#define`` and tags ``|\#|``<br>
the same applies to `` ! ~ ? |``  `` \! \~ \? \|``  ``a = a \!= b \? c : d; -> a = a != b ? c : d;`` <br>

to color/style text use `$...$` command and `$$` to reset
```
    $red$red$$ $green$green$$ $yellow$yellow$$ $blue$blue$$
    $purple$purple$$ $cyan$cyan$$
    $r$red $g$green $y$yellow $b$blue $p$purple $c$cyan $$
    $R$red $G$green $Y$yellow $B$blue $P$purple $C$cyan $$

    changing $italic$mode italic $red$color$white$ hello $$ normal
    $dim$mode dim,$$ $italic$$dim$ italic dim$$
    $underline$underline$$ normal
    $/$italic$$ normal $_$underline$$ normal $%$dim$$ \? $$
        
    $~$set info style$$ normal again
    $!$set warn style$$ normal again
    $?$set link style$$ normal again
    $|$set tag style$$ normal again
    $|$not_tag$|$ $$ $not_tag$ finds this
    $|$\|tag-name\|$$ -> |tag-name|
```

for more help on custom sheets or general usage use command ``doc -h`` in terminal, <br>
or ``doc sheet-syntax-examples`` to see all usecases for syntax <br>


### custom executable name
open `build/make/CMakeLists.txt` change `set(NAME "...")` at the top
now repeat the [instalation](#instalation) steps

### config file
config file is `root/config.doc` <br>
```
[syntax] true
[syntax] false

// comment

[sheet_dir_rel] src/sheets

[sheet_dir] C:\custom_sheets
```
`[syntax]` enables or disables highlighting <br>
can be set to `1, true, 0, false` <br>
`[sheet_dir]` and [sheet_dir_rel] add a new path to check for .sheet files <br>
`[sheet_dir_rel]` is relative to root dir <br>
max is 8 right now, view `doc -config` for current max <br>
use `//` for comments <br>

use `-config` modifier to print config file

## buggs
  - [ ] -d doesnt find program_start in bovengine
  - [ ] highlights return in 'returns: ' in opengl.sheet glGetError
    - the : at the end makes it highlight as return, like return 0;

## todo
  - [ ] maybe change -d to just using keyword with () at the end
  - [ ] search structure/enum definitions in specified dir
  - [ ] search function / structure references
  - [ ] incomplete search, i.e. func_ -> func_a, func_b, ...
  - [ ] load keys for style.c from file, for custamization
  - [ ] c-syntax in macros, for numbers, strings, etc.
  - [ ] make sure all sheets use '-' as space in tags
  - [ ] replace file_io.c/.h with the one in bovengine
  - [ ] more documentation
    - [ ] docs (finish functionality first)
      - [ ] custom .sheet
      - [ ] custom name
    - [ ] make
    - [ ] cmake
    - [ ] stb
      - [x] stb_ds
      - [ ] stb_image
      - [ ] stb_truetype
    - [ ] opengl 
      - [ ] every func in debug_opengl.h `WIP`
      - [ ] macros in glad.h ( GL_TEXTURE0, etc.)
    - [ ] glfw
      - [ ] everything in window.c
      - [ ] everything in input.c
    - [ ] vim cheatsheet
    - [ ] my stuff ?
      - [x] global.h
      - [ ] serialization
      - [ ] text
      - [ ] math
 

