# regEdit
A very simple tool to read and write registers of an embedded Linux platform.

Usage: ./regEdit [options]

Options:
-d | --device name   Video device name [/dev/mem]
-h | --help          Print this message
-w | --write         write register with value
-r | --read          read register
-b | --base          base address
-f | --offset        offset from base address
