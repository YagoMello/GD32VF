# GD32VF
## TLDR;

Every C, C++ or Asm file in the src folder will be compiled (but files in subfolders will not).
Just throw some code in there (`src`) and run make (in `build` folder). The `bin` folder contains the output (`elf` and `bin`).

# Compiling
The RISCV32 compiler is not found in pre compiled packages like avr-gcc and arm-none-gcc.
But the compilation from source is simple and takes just a few steps and a few GB of disk space.

`[install directory]` should be some place like /opt/riscv-gcc/.

The `Makefile` is configured to use a toolchain installed to `/opt/riscv-gcc/gd32vf103/`.

To build the toolchain, in the command line, use:
``` shell
git clone --recursive https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain
mkdir build
cd build
../configure --with-arch=rv32im --prefix=[install directory]
make
```
The toolchain binary is installed inside `[install directory]/bin`

# Make
## MCU version selectionm
Open the `Makefile` (in build folder) with a text editor and uncomment the `.lds` with your mcu version on it.

Also verify the toolchain (compiler, linker, etc) path.

You can change the output (elf, binary) name by changing the NAME variable in the `Makefile` too. 

## release (default)
Uses a set of optimizations to speed up code and reduce its size, but make almost impossible to debug.
``` shell
make release
```

## debug
Adds a lot of information to the executable, improving the debug experience
``` shell
make debug
```

## clean
Delete all temporary files, the elf and binary.
``` shell
make clean
```

# Flashing the MCU
## dfu-util
Why use programmers when one can use the usb directly? (I forgot to buy the programmer/debugger)

The dfu-util uses a protocol designed to allow the device firmware update (dfu). It doesn't require the clasic bootloader (software), so a bad firmware can't brick the mcu.
Some older versions of dfu-util fail to flash the gd32vf mcu.

I recommend building it from source, just [download](http://dfu-util.sourceforge.net/) the newer (as of october 2020) version and follow the instructions on `INSTALL`.
On OpenSUSE the program did not install to /usr/bin, but to /usr/local/bin, and to use it from the command line I need to write
``` shell
sudo /usr/local/bin/dfu-util
```
The device must be in the dfu mode. In some boards which have the buttons `RESET` and `BOOT`,
first hold the `BOOT`, and whthout releasing it, press `RESET`, then release the `BOOT` button.

To find the device, use the `dfu` command with `-l` or `--list`
``` shell
sudo /usr/local/bin/dfu-util --list
```
The program will return something like
```
Found DFU: [28e9:0189] ver=0100, devnum=52, cfg=1, intf=0, path="1-4", alt=0, name="@Internal Flash  /0x08000000/128*001Kg", serial="3CBJ"
```
The first field, `[28e9:0186]`, identifies your mcu.
Then, to write the program:
``` shell
sudo /usr/local/bin/dfu-util -d 28e9:0189 -a 0 --dfuse-address 0x08000000:leave -D prog.bin
```
The option after `-d` is the identifier found previously.

The option after `-D` is the binary to be writen to the mcu.

`--dfuse-address 0x08000000` Indicates the beginning of the flash. This value can be found in the linker script (`.lds`) in the `ext` folder.

# Libraries
The firmware lib is in the Firmware folder.

It is well docummented (for today standards of no docummentation), and the user manual is almost as good as the Microchip PIC series manuals.
The docummentation can be found [here](http://www.gd32mcu.com/en/download/0?kw=GD32VF1).

I fixed some bugs (like `bool` being redefined, missing `__` before `__cplusplus`, missing `ifdef __cplusplus` then `extern "C" {`, etc.).

GigaDevices gives us a complete linker script. This script contains the memory map and where to put the executable sections, the variables, etc.
They also give the initialization assembly, which sets the entry point of the code, the stack pointer, interrupt tables, cpp support and etc.
Some manufacturers (looking at you ST) don't give any of them (ST bundles it with the ridiculously bloated HAL, and only works with the HAL).
