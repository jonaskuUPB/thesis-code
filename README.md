# thesis-code

## Scope of this implementation

## Setting up and compiling

In order to build this code you will need to install QtCreator. The code is tested under QtCreator 3.5.1 (based on Qt 5.5.1) but should work with newer versions as well.

The source code provided in this repository depends on a number of external libraries as well. Namely these are Box2D, Qt and my custom library for NSGA2. This repository provides my working implementations for Box2D and NSGA2 (in form of .zip archives) simply extract and build these folders.

If you want to get the latest versions, try the following commands in the terminal

Box2D:

```sh
$ cd Box2D/Box2D
$ mkdir -p Build
$ cd Build
$ cmake ..
$ make
```

NSGA2:

```sh
$ cd NSGA2
$ mkdir Build
$ make libnsga2
```

Make sure to use American-English locale (alternative: C format) for numbers! Otherwise you will have problems with the analysis and NSGA2.

If everything is set up run qmake to generate the Makefile. Once you build the source code you will receive the executable file Simulator.

## Acknowledgements


## Misc notes

```sh
sudo apt-get install xorg-dev libglu1-mesa-dev
```

(already done)
create folder results
- add default settings


