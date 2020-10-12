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
cd build
rm -r *
cmake -D arm=true ..
make
```
The executable won't run on your machine, you will need to scp it to the arm platform and run it there. 
