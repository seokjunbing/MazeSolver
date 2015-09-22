README for Amazing Maze by c-sick

Contributors
------------
- Jun Bing
- Van Nguyen
- Sahil Seekond
- Matthew West

Final edit on 03/12/15
Amazing Maze was produced as a part of cs50 W15 final group project.

Intro
-----

The goal of this program is to congregate all the avatars at a position on a maze
with minimal amount of moves. This programs contains 3 different algorithms.(Refer to documentation for more explanation)

We realized too late that we were required to submit the entire log file with all moves recorded. Therefore, minimal_logs contains log files for all combinations of avatars and difficulties, but which list only the overall info about the run, not every move taken. The folder logs contains complete logs, but not quite all of them, since there was not sufficient time for all of them to run.

startup.c contains the main function that eventually calls all other
source and header files located in ./src/

Invoke make on ./Makefile to compile the program.
Or, alternatively, use gcc with the flag "-std=c11 -Wall -pedantic -I. -D_XOPEN_SOURCE"

Makefile
--------
  ./Makefile has functions to compile, clean, and test the program.
  'make' to compile the program
  'make test' to do unit tests on the program
  'make clean' to remove object files and .log files
  'make remove' to remove executable
  'make debug' to compile the program with "-ggdb3" flag

```
       Usage: startup -n nAvatars -d Difficulty -h Hostname [-a algorithm] [-l logLevel]
       nAvatars
       This is the number of avatars to be put in the maze. 1 to 10 are accepted.
       Difficulty
       This is the difficulty level of the maze to be solved. 0 to 9 are accepted.
       Hostname
       This is the address of the host server. pierce.cs.dartmouth.edu is used for this assignment.
       [algorithm]
       This is an optional flag used to specify the algorithm used to congregate the avatars together. 0 to 2 are accepted. If this option is not specified then the program will load algorithm 1 as default.
       [logLevel]    This is an optional flag used to specify the level of logging output. 0 to 6 are accepted with
       0 being the least verbose and 6 being the most verbose. Level 3 is set as the default.
        0 -> LVL_FATAL
        1 -> LVL_ERR
        2 -> LVL_WARN
        3 -> LVL_INFO
        4 -> LVL_DBG
        5 -> LVL_VERB
        6 -> LVL_TRACE
```

Output
------
  .log file that is named "Amazing_$USER_N_D.log" where $USER is the current userid, N is the value of nAvatars
  and D is the value of Difficulty. The first line of the file contains $USER, the MazePort, and the date & time.
  The content following the first line will depend on the level of logging parameter.


**Please refer to the attached PDF documentations in for more details.

