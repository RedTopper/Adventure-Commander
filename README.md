# Adventure Commander
A text base adventure game written in C++

## Author
Aaron Walter - ajwalter - 792829065

## Build using the Makefile
#### Ubuntu
1. Install ncurses on Ubuntu: `sudo apt install libncursesw5-dev`
2. Go to Common

#### Fedora
1. Install ncurses on Fedora: `sudo yum install ncurses-devel`
2. Go to Common

#### MinGW
1. Navigate to the included PDCurses wingui library `cd library/PDCurses/wingui`
2. Build the library with `make -f Makefile.mng pdcurses.a WIDE=Y UTF8=Y`
3. Navigate to the project directory `cd ../../../`
4. Go to Common

#### Common
1. Run `make` to compile everything
2. Run `./game` to launch the game
3. Press `TAB` for in game help!

## Build using Visual Studio
1. Open the `x64 Native Tools Command Prompt for VS 2017`*
2. Navigate to the `library\PDCurses\wingui` folder.
3. Build the library with `nmake -f Makefile.vc WIDE=Y UTF8=Y`
4. Open up Visual Studio and choose `File > Open > Folder...`
5. Switch to the x64 build and press the play button
3. Press `TAB` for in game help!

## Features
- Completely written in C++
- Cross platform with Linux and Windows (Portable)
- Uses UTF-8 characters with Emoji support that works cross platform
- No usages of `new` and `delete`, automatic memory management with `std::shared_ptr<>`


## Console setup
This game uses UTF-8 to make it look nice! Please read this if your terminal 
looks messed up (ex: question marks or blank output):

- If you are using `gnome-terminal` (called Terminal), UTF-8 works with emoji. 
- If you are using `xterm`, press CTRL + RIGHT CLICK and enable TrueType fonts.
  (Use the --lame switch to disable emoji)

In any case, your locale should be set to "en_US.UTF-8". You can check with the
command `locale`. SSH environments can sometimes break this, so you might need
to put these in your `~/.profile` if you are using SSH and see question marks:

- `export LC_ALL=en_US.UTF-8`
- `export LANG=en_US.UTF-8`
