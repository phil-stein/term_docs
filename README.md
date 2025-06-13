
# term docs

[!] see [todo](#todo) for planned features<br>
[!] only tested on windows<br>

c documentation for the terminal <br>
works offline and is customizable <br>
also has search utility functions <br>
works in [neovim](#neovim) <br>
most c std library documentation is based on [tutorialspoint](https://www.tutorialspoint.co://www.tutorialspoint.com/c_standard_library/index.htm) and [cppreference](https://en.cppreference.com)

|                                                             doc                                                             |                                                             search                                                             |
|:---------------------------------------------------------------------------------------------------------------------------:|:------------------------------------------------------------------------------------------------------------------------------:|
| <img src="https://github.com/phil-stein/term_docs/blob/main/files/screenshots/screenshot_doc01.png" alt="logo" width="600"> | <img src="https://github.com/phil-stein/term_docs/blob/main/files/screenshots/screenshot_search01.png" alt="logo" width="400"> |


## table of contents
  - [features](#features)
  - [example](#example)
  - [instalation](#instalation)
  - [troubleshoot](#troubleshoot)
  - [customization](#customization)
    - [custom documentation](#custom-documentation)
    - [custom executable name](#custom-executable-name)
    - [config file](#config-file)
  - [vim | neovim](#vim--neovim)
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
doc ... -loc    -> print location of documentation

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
      - in root call `cmake_build` 
      - make: in root call `build`
      - vs19: go to `build/vs19/doc.sln`
        - compile
  4. add `root/bin` directory to your [path](https://stackoverflow.com/questions/44272416/how-to-add-a-folder-to-path-environment-variable-in-windows-10-with-screensho)
      - `root/bin/Debug` or `root/bin/Release` for vs19

## troubleshoot
  - output text is messed up
    - try adding '-c' to your command to disable syntax highlighting, <br>
      some terminals don't support it, like command prompt on windows <br>
      on windows the new windows terminal, found in microsoft store, supports it <br>
      see [config-file](#config-file) to permanently disable highlighting <br>

    - disable [utf8] and [icons] in config.doc see [config-file](#config-file) <br>
      
  - custom documentation doesnt show up, check if its surrounded by `#`<br>
    and the tags are surrounded in `|` <br>

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
to include icons properly use:
```
  $icon::icon-text$ -> tunrns to '' if [icons] and [utf8] in config.doc are true, else 'icon-text'
```

for more help on custom sheets or general usage use command ``doc -h`` in terminal, <br>
or ``doc sheet-syntax-examples`` to see all usecases for syntax <br>


### custom executable name
open `build/make/CMakeLists.txt` change `set(NAME "...")` at the top
now repeat the [instalation](#instalation) steps

### config file
config file is `root/config.doc` <br>
```
// comment
[syntax] true
[location] true
[utf8]  true
[icons] true

// black, red, green, yellow, blue, purple, cyan, white
[title_color] red 
// [border] ascii: " " "-" "_" "." nerdfont: "" "" "" "" "󰥛" "󱑻" "󱑼
[border] ""
[seperator_left]  ""
[seperator_right] ""
// [seperator_left]  ""
// [seperator_right] ""
[title_spacing] 0.1

// builtin sheets
[sheet_dir_rel] "sheets/builtin_sheets/"
[sheet_dir_rel] "src/sheets"
[sheet_dir] "C:\custom_sheets"
```
__boolean__: can be `true` or `false`, `TRUE` or `FALSE` <br>
  `[syntax]` enables or disables highlighting <br>
  `[location]` enables or disables printing location of .sheet file and line <br>
  `[utf8]` enables or disables using utf8 character <br>
  `[icons]` enables or disables using [nerdfont-icons](https://www.nerdfonts.com/) <br>
__colors__: can be black, red, green, yellow, blue, purple, cyan, white <br>
  `[title_color]` color of the title
__number__:  <br>
  [title_spacing] 0.0 means title is left, 0.5 center, 1.0 right, etc. <br>
__string__: -> "..." <br>
  `[border]` char used to draw border above below docs, can be " " <br>
  `[seperator_left]` char used at left of title  <br>
  `[seperator_right]` char used at right of title <br>
  `[error_icon]` set a string to replace ! in error messages, ! by default <br>
  `[warning_icon]` set a string to replace ! in docs, ! by default <br>
  `[info_icon]` set a string to replace ~ in docs, ~ by default <br>
  `[link_icon]` set a string to replace ? in docs, ? by default <br>
  `[sheet_dir]` add a new path to check for .sheet files <br>
  `[sheet_dir_rel]` is relative to root dir <br>
                max is 8 right now, view `doc -config` for current max <br>
use `//` for comments <br>

use `-config` modifier to print config file <br>
run `>doc config-file` for more help <br>


## vim | neovim
### vim
 __i havent tested this__ <br>
in .vimrc / _vimrc add, view reference [link](https://dev.to/dlains/create-your-own-vim-commands-415b)<br>
`command -nargs=1 -complete=file -bar Doc :split | :term <args>` <br>

### neovim
add either one to your `nvim/init.lua` to add the command <br>
### open in split
open doc in split, using :Doc ... command <br>
change split to vsplit for vertical split <br>
```lua
  vim.api.nvim_create_user_command('Doc',
    function(opts) vim.cmd('split | term '..opts.fargs[1]) end, 
    {  nargs = 1, desc = ''})
```
### open in popup
open doc documentation in [nui.nvim](https://github.com/MunifTanjim/nui.nvim/tree/main) popup using :Doc ... command <br>
closes using `:q`, just pressing `q` or `esc`, can be remapped <br>
  <details>
    <summary>image</summary>
    <img src="https://github.com/phil-stein/term_docs/blob/main/files/screenshots/screenshot_nvim01.png" alt="logo" width="1000">
  </details>
  
<details>
  <summary>nvim lua code</summary>
  
```lua
  vim.api.nvim_create_user_command('Doc',
    function(opts)
      local Popup = require("nui.popup")
      local popup = Popup({
        position = "50%",
        size = { width  = 0.4, height = 0.65 },
        enter = true,
        focusable = true,
        zindex = 50,
        relative = "editor",
        border = {
          padding = {top = 2, bottom = 2, left = 3, right = 3 },
          style = "rounded",
          text = { top = " doc: "..opts.fargs[1].." ", top_align = "center" },
        },
        buf_options = { modifiable = false, readonly = true },
        win_options = { winhighlight = "Normal:Normal,FloatBorder:Normal" },
      })
      popup:show()

      -- close popup when leaving it
      local event = require("nui.utils.autocmd").event
      popup:on({ event.BufLeave, event.BufDelete, event.BufHidden },
        function()
          popup:unmount()
        end, { once = true })
      -- quit with esc or q
      popup:map("t", "<esc>", function() vim.cmd('q!') popup:unmount() end)
      popup:map("t", "q",     function() vim.cmd('q!') popup:unmount() end)
      -- open terminal
      vim.cmd('term doc '..opts.fargs[1])
      vim.cmd('startinsert')
    end, {  nargs = 1, desc = ''})
```
</details>

with this setup you could for example add <br>
`vim.keymap.set('n', '<C-h>', ':Doc neovim-mappings<CR>', {silent = true, desc = "show neovim mappings"})` <br>
to open a specific doc documentation via keybind <br>

## buggs
  - [ ] -d doesnt find program_start in bovengine
  - [ ] highlights return in 'returns: ' in opengl.sheet glGetError
  - [ ] GetModuleFileName in main.c only works on windows

## todo
  - [ ] update readme, example section, screenshots, etc.
  - [ ] make command to disable looking at any command or \$X$ to escape commands
  - [ ] search structure/enum definitions in specified dir
  - [ ] search function / structure references
  - [ ] incomplete search, i.e. func_ -> func_a, func_b, ...
  - [ ] load keys for style.c from file, for custamization
  - [ ] c-syntax in macros, for numbers, strings, etc.
  - [ ] make sure all sheets use '-' as space in tags 'WIP'
  - [ ] replace file_io.c/.h with the one in bovengine
  - [ ] make github release
  - [ ] have $g_$ for green and underscore
  - [ ] convert into some style format and then print that rather than directly print
    - i.e. use numbers not useb by ascii for styling
    - could then use in non terminal applications
  - [ ] online documentation
    - use curl or something
    - have 'offline' defenition in sheet file but use 
      linked documentation if online
    - use links or have git repo with .sheet files that are specified in config.doc ?
  - [ ] add $text$ ... $$ that just outputs raw text, no escaping or highlighting
    - have to change search for that
  - [ ] set background color 
  - [x] add nerdfont/devicon support -> `$icon:'✖':x$`
    - https://www.nerdfonts.com/cheat-sheet
    - https://dev.to/rdentato/utf-8-strings-in-c-1-3-42a4, https://dev.to/rdentato/utf-8-strings-in-c-2-3-3kp1, https://dev.to/rdentato/utf-8-strings-in-c-3-3-2pc7
  - [ ] make tabs be two spaces
  - [ ] convert image to terminal output ?
  - [ ] make string values in config.doc be abled to have "..." or '...'
  - [ ] add [print] or print() to config.doc
  - [ ] add $file:path/to/file.txt$ to print out contents of file
  - [ ] add $sheet:tag$ to print contents of sheet
  - [ ] more documentation
    - [ ] c keywords i dont have yet
      - [ ] c11/c23 stuff ?
        - [ ] constexpr
        - [ ] alignas
        - [ ] alignof
        - [ ] nullptr
        - [ ] etc.
    - [ ] <stdint.h> 
    - [ ] make
    - [ ] cmake
    - [ ] stb
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
      - [ ] serialization
      - [ ] text
      - [ ] math
      - [ ] bovengine
        - at least commonly reused stuff file_io etc.
 

