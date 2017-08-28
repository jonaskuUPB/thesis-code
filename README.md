# thesis-code

$ git clone https://github.com/flyover/Box2D
$ (cd Box2D && git submodule init && git submodule update --recursive)

sudo apt-get install xorg-dev libglu1-mesa-dev

cd FANN
cmake . -D CMAKE_INSTALL_PREFIX:PATH=/path/to/folder
make install

extract folder NSGA2 and run
cd NSGA2
mkdir Build
make libnsga2

(already done)
create folder results
- add default settings

make sure to use American-English locale (alternative: C format) for numbers !! (otherwise you will have problems with the analysis and NSGA2)

