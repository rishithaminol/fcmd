# Find commands in the system PATH #

This command finds commands in the system PATH using regular expressions

## Installation ##
```
make
make install
```
fcmd executable is installed into the `/usr/bin`

## Files and description ##
 - scripts/ - simple shell scripts for managing the project.
 - uncrustify/ - uncristify configuration file used in.
 - Makefile  - fcmd compile recipe.

## Contributing ##
 - You can use your own debugging tools but I recommend valgrind and gdb to debug your executable(s)
 - To beautify the code you can use uncrustify(The code beautifier for C). On some distributions this command does not exist. I recommend to use uncrustify/linux.cfg as the default uncrustify config because it is a small derivative of uncrustify examples with a litle customizations.
 - You can download the uncrustify source and compile it yourself. <http://sourceforge.net/projects/uncrustify/files/>
 - There are small scripts in the scripts/ directory you can use them to manage
code yourself
