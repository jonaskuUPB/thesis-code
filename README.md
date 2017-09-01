# thesis-code

## Scope of this implementation

##Setting up and compiling

The source code provided in this repository depends on a number of external libraries as well. Namely these are Box2D, Qt and my custom library for NSGA2. This repository provides my working implementations for Box2D and NSGA2 (in form of .zip archives) simply extract and build these folders.

If you want to get the latest versions, try the following commands in the terminal

Box2D:

```sh
$ git clone https://github.com/flyover/Box2D

$ (cd Box2D && git submodule init && git submodule update --recursive)
```

NSGA2:

```sh
$ cd NSGA2
$ mkdir Build
$ make libnsga2
```

make sure to use American-English locale (alternative: C format) for numbers !! (otherwise you will have problems with the analysis and NSGA2)

##Acknowledgements


##Misc notes

sudo apt-get install xorg-dev libglu1-mesa-dev

(already done)
create folder results
- add default settings


