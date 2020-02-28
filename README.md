# TJEDuktape
An extension of the [TJE framework](https://github.com/jagenjo/TJE_Framework) from @jagenjo using [DukTape](https://github.com/svaarala/duktape/) and [DukGlue](https://github.com/Aloshi/dukglue) for scripting. 
![](https://i.imgur.com/hPpSWqH.png)

Includes a basic console with some scripts preloaded. 
Those scripts are listed on init.js inside data/scripts folder.
The "gluing" issue between C++ and JS is done inside the scripts.cpp file.

Features included:
* Most ES5
* console redone to work
* "require" working for local files

Features missing:
* Any JS IO is implemented by DukTape.
* XMLHttpRequest
* "let" for defining variables
* arrow functions "arg => {}"
* "Promises"
