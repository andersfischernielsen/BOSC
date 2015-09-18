# BOSH features implemented
## Printing of hostname on every shell line.
The file `/proc/sys/kernel/hostname` is opened using the `fopen` call with read-only rights.

The content is then scanned using the `fscanf` function reading the entire first line (the hostname) and saving that into the memory area denoted by the hostname-pointer.

`fscanf` is chosen, because it can read directly from a file-stream.

The hostname is then returned.

## Making BOSH not exit on CTRL+C
In the main function, bosh is instructed to ignore all SIGKILL.

When another process is created in order to execute another program, the default behaviour is set, to make it possible to kill that process, but not the BOSH-one.

Because it is reenabled in another process, the BOSH-process keeps ignoring this key-press.

The only way to exit BOSH is to write exit at the command line. This is handled by comparing the given command to the string "exit". If an exact match is found, the terminate-flag is set, and BOSH will terminate next time the flag is checked (which is instantly, as it is checked after every command is executed).
