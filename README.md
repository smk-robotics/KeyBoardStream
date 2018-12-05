# KeyBoardStream

## This is the console application that checks keyboard events and prints the button that was pressed/released. It uses only typical unistd library.

### It is necessary to set the correct `deviceInputAdress` in **KeyBoardStream.cpp**.

#### Should be run as **root**.

To find correct keydoard event-file you can use console commands:

`$sudo cat /sys/class/input/input<x>/name` - shows name of <X> device. <X> - numbers of input devices from (e.g. 4 or 6)

`sudo cat /dev/input/event<X>` - shows input stream for <X> device (ASCII codes for pressed keyboard button).
