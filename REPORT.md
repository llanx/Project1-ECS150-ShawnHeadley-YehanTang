
# ECS 150: Project #1 - Simple Shell


### Authors
- Shawn Headley
- Yehan Tang


### Project Implementation

After reviewing the suggested phases provided to us on the project overview 
page, we decided to start breaking up those suggested phases into smaller 
and more testable chunks.


#### Starting Off (Built-ins)
Initially, we started off by writing a very simple parser using the example 
skeletoncode given to us in the project resources. We took some time to 
implement functions to handle the built-in commands, and statically called 
them from the parser.


#### Parsing Commands
Once we were confident in the basic built-in features working (after testing 
them individually), we began to create a larger function that could handle:

1. Parsing the incoming command
2. Determining if the command was built-in, or dynamic, and calling it 
correctly.

Parsing the incoming command was a lot more difficult than we had initially 
thought. Calling back to a project we had worked on during some early 
college courses, we decided to use a tokenizer via `strtok` to try to 
break up the incoming commands and delimit any additional arguments.

This process worked really well initially, until we had to consider 
handling output direction and piping. Since we decided to start small,
we decided to try to handle those additional cases when we got to them.


#### Executing Commands
Once the parsing was finished, we were able to create another function
that would control all of the logic for each command that could be called.
This was super straighforward, since we already had the built-in functions
defined beforehand.

After this, we dove into creating the fork-exec-wait process to launch
custom/installed commands from the shell. This initially was difficult to
fully nail down, as we ran into some really strange bugs trying to 
correctly wait and handle the forking of the process (explained further
in "Observations and Issues").

Thankfully, we were able to nail down all of the bugs with the forking
and waiting for the child run processes. With all of the commands working
(and command argruments working), we decided to start implementing some
basic error handling.

The error handling was fairly straightforward, as we could use `perror()`
on any system/os related errors that arised. Initially we tried to safely
handle this within the code, but we couldn't write a clean implementation,
so we were thankful to use `exit(1)` instead. 

With our implementation of the exec-fork-wait process, we didn't have to
do any additional work handling execution errors from commands that were
called (non built-in). We simply were able to allow those commands to 
write out to the stream and already have it logged out correctly.

We did have to go through and write some validation checks/blocks to handle
some of the internal tokenizing/built-in errors that were present. However,
since we had already tested these implementations, it was really 
straightforward for us to add the error output in.

Finally, once we were confident in all of the previous functionality, we
started working on adding Output Redirection into the parser and command
executor portions of the code.


#### Output Redirection

Since we were already tokenizing the command arguments, it was fairly 
easy for us to add some additional logic within the tokenizer to look for a
'<' character within the command arguments. From there, we were able to 
assume that any additional token after that would be the redirect file, 
and if there were any tokens after the filename, we needed to throw an error.

The logic for correctly determining the output redirection is admittedly 
not as clean as we would have hoped. Initially we believed that we could use
a null/empty 'output_file' variable to determine if the parser had found a '<'.
However, if there was no value after the '<' character, it wouldn't be easy
to know if the '<' was missing the filename, or if it was somehow apart of
another command.

To fix this, we included an extra bool (int) flag that the parser would set,
so that any callee function could double-check if the parser was confident 
the '<' character existed within the command arguments. With this, we were
able to pretty accurately determine where the redirect operator was located
(if it existed) within the command arguments string.

Finally, we were able to use `fopen()` and `fd2()` to append to the redirect
file (or create one if it didn't exist). This was fairly straightforward to
implement, and we were able to verify this functionality within the shell.


#### Piping

Unfortunately, even with the time we were able to put into this project, we
didn't have enough time to fully insert/complete logic to implement piping,
but we were fairly close. 

Ideally, if we had more time to restructure our command parsing, and use 
structs to segment each pipe section of the command, we could have used the
standard library's piping commands, along with the fork-exec-wait, to get the
functionality working.

This required a lot of changes to the way we implemented command parsing, and
those changes severely broke our working implementation. Because of this, we
pulled out this additional logic so we could have more clean/easier to read 
code for our submission.


### Obserations and Issues

We ran into a fun bug/issue during the middle of the development of the
fork-exec-wait portion of the code. We inadvertently created a fork-bomb
within our code by not correctly handling the wait portion of the code 
within the parent process.

If you remove the wait portion of the logic within the parent, and run
the shell, then run 'make' WHILE still being in the shell, the make 
command will continue to cascade and completely runoff until hundreds of
thousands of zombied/exec'd PIDs are running, and will just completely
crash the machine.

Thankfully, we didn't lose any data while this was happening (thanks 
autosave!!!), but this would cause us a ton of headaches for a day, 
having to constantly restart the whole machine if you accidentially
recursively called 'make' within the shell!


