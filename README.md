# Adventure Commander
A text base adventure game written in C++

## Author
Aaron Walter - ajwalter - 792829065

## Features
- Completely written in C++
- Cross platform with Linux and Windows (Portable)
- Uses UTF-8 characters with Emoji support that works cross platform
- No usages of `new` and `delete`, automatic memory management with `std::shared_ptr<>`

## Build using the Makefile
#### Ubuntu
1. Install ncurses on Ubuntu: `sudo apt install libncursesw5-dev`
2. Go to Makefile Common

#### Fedora
1. Install ncurses on Fedora: `sudo yum install ncurses-devel`
2. Go to Makefile Common

#### Windows (Native with MinGW)
1. Navigate to the included PDCurses wingui library `cd library/PDCurses/wingui`
2. Build the library with `make -f Makefile.mng pdcurses.a WIDE=Y UTF8=Y`
3. Navigate to the project directory `cd ../../../`
4. Go to Makefile Common

#### Makefile Common
1. Run `make` to compile everything
2. Run `./game` to launch the game
3. Press `TAB` for in game help!

## Build using CMake
#### Windows (Native with Visual Studio 2017)
1. Open the `x64 Native Tools Command Prompt for VS 2017`*
2. Navigate to the `library\PDCurses\wingui` folder.
3. Build the library with `nmake -f Makefile.vc WIDE=Y UTF8=Y`
4. Open up Visual Studio and choose `File > Open > Folder...`
5. Make the folder `cmake-build-windows-64` in the root of the project
6. Go to CMake Common

#### Windows/Linux (Linux Build with CLion WSL Toolchain)
1. Install ncurses on WSL: `sudo apt install libncursesw5-dev`
2. Open the project with File > Open...
3. Set up your toolchain (if needed): https://www.jetbrains.com/help/clion/how-to-use-wsl-development-environment-in-clion.html
4. Go to CMake Common

#### Windows (Native with CLion Visual Studio Toolchain)
1. Open the project with File > Open...
2. Follow the first 3 steps of Build using CMake > Visual Studio
3. Go to CMake Common

#### CMake Common
1. Press the build button
2. Navigate to the build directory `cmake-build-debug`/`cmake-build-windows-64`
3. Copy the `resource` folder into this folder
4. Run the executable `./Adventure_Commander` if in CLion, or press Play in Visual Studio
5. Press `TAB` for in game help!

## Console setup
This game uses UTF-8 to make it look nice! Please read this if your terminal 
looks messed up (ex: question marks or blank output):

- If you are using `Native Windows Build`, PDCurses should take care of emoji.
- If you are using `WSL bash`, use the console emulator `Cmder` for emoji.
- If you are using `gnome-terminal` (called Terminal), UTF-8 works with emoji. 
- If you are using `xterm`, press CTRL + RIGHT CLICK and enable TrueType fonts.
  (Use the --lame switch to disable emoji)

In any case, your locale should be set to "en_US.UTF-8". You can check with the
command `locale`. SSH environments can sometimes break this, so you might need
to put these in your `~/.profile` if you are using SSH and see question marks:

- `export LC_ALL=en_US.UTF-8`
- `export LANG=en_US.UTF-8`
