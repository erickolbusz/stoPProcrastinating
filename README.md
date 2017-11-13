# stoPProcrastinating
For Windows 10 because I still don't use linux at home

## What it does
- `work.c` should be run when working and keeps track of how much time is spent working
- `killer.c` kills any specified programs when they are launched (i.e. video games)
    - stops killing if a certain amount of time has been spent working since the daily reset time

## How to use it
- `killer.c`
	- change `PROGRAMS[2]` to all programs to possibly kill
	- change `RESET_TIME[3]` to the time of day where the killer resets
	- change `UNLOCK_TIME[3]` to the time needed to stop killing
	- add a shortcut to `killer.exe` in the Windows startup folder *(Win+R "shell:startup")*
- `work.c`
    - run `work.exe` as a stopwatch for working
    - `SIGINT` *(Ctrl+C)* and `SIGHUP` *(Xing out)* are normal exits and will save your time
- `log.txt` contains lifetime time information
- `data` is for internal use just ignore it

## Considered changes
- timer resets on computer startup/shutdown
- working X time earns you kX kill-free time (resource intensive)
- killing stops if you have X work time in the past Y time (bad - killer reactivates without user knowledge)
