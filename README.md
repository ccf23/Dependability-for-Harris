# Dependability-for-Harris
DCA research project (academic document)

## Quick Start
```
mkdir build
cd ./build
cmake ..
make
./Harris ../images/landslides_in_cerbers_fossae.jpeg
```

## Compile For Local
The cmake file is configured for local or arm compiling. The default will compile for the local machine's architecture. Follow the instructions in the quick start section to compile for the local machine. 

## Compile for ARM
To successfully compile for arm you need to have `arm-linux-gnueabihf-g++` installed and have ***statically*** compiled openCV for arm, otherwise it will not build and run successfully.

For deployent to an arm based device run the following commands from the `build` directory:

```
rm -r *
cmake -D arm=true ..
make
```
The executable won't run on your machine, you will need to scp it to the arm platform and run it there.

## Dependencies

- OpenCV, statically cross-compiled for ARM
- ITPP, statically cross-compiled for ARM (see steps below)

### Steps to cross-compiling ITPP library statically for ARM:
 
1. Download and unzip itpp 4.3.1 library from here: https://sourceforge.net/projects/itpp/files/latest/download
2. `cd itpp-4.3.1`
3. `mkdir build && cd build`
4. `mkdir /usr/local/itpp-arm` (path to install itpp statically compiled for arm)
5. Run this `cmake -DITPP_SHARED_LIB=off -DCMAKE_INSTALL_PREFIX=/usr/local/itpp-arm -DCMAKE_C_COMPILER=/usr/bin/arm-linux-gnueabihf-gcc -DCMAKE_CXX_COMPILER=/usr/bin/arm-linux-gnueabihf-g++ -DCMAKE_STRIP=/usr/bin/arm-linux-gnueabihf-strip -DCMAKE_FIND_ROOT_PATH=/usr/arm-linux-gnueabihf -DCMAKE_CXX_LINK_EXECUTABLE=/usr/bin/arm-linux-gnueabihf-ld ..`
6. `sudo make && sudo make install`

Resources:
	http://itpp.sourceforge.net/4.3.1/installation.html

