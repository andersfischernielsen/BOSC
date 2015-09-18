# BOSH features implemented
## Printing of hostname on every shell line.
The file `/proc/sys/kernel/hostname` is opened using the `fopen` call with read-only rights.

The content is then scanned using the `fscanf` function reading the entire first line (the hostname) and saving that into the memory area denoted by the hostname-pointer.

`fscanf` is chosen, because it can read directly from a file-stream.

The hostname is then returned.

