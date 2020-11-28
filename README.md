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

### benchmark generation
To generate a benchmark use the following command format
```
./Harris ../images/<image>.jpeg benchmark
```

### logging
logging will automatically be enabled if `DATA_COLLECTION_ON` is defined as `true`. The run statistics will be appended to the log file. The log filename has the format `<path to image>_log.csv`.

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


## Fault Injector

A simple fault injector is included with the project. The fault injector is designed to be embedded into the code to inject data bit flips into the code based on a predefined operating strategy or "mode". There are three primary modes:

- **SINGLE_DATA:** Injects a single bit flip into whatever data object is passed into the `inject()` function.

- **DOUBLE_DATA:** Injects two unique bit flips into whatever data object is passed into the `inject()` function.

- **PROB_DATA:** Injects errors into each bit in the data object based on the `bit_hit_prob` passed into the injector constructor or set using the `setBHP()` function. For example if `bit_hit_prob = .1` there is a 10% chance that a given bit will be flipped. this corresponds to a 56.9% chance of at least one fault in a given byte. 

### Example usage
Specific usage will vary but may look like this:
```
injector fi(PROB_DATA, 1e-3);
fi.enable();
...
fi.inject(aMatVar);
...
fi.inject(aDoubleVar, SINGLE_DATA); // uses SINGLE_DATA mode for this injection only
...
fi.disable()
cout<<fi.stats()<<endl;
```
<<<<<<< HEAD

### Testing
There is a simple testing script `test.sh` that can be used to run tests on the pynq board. The script expects the following:

*`Harris_bench`- The benchmark executable for harris. This is always run first to ensure the baseline is accurate.
*`Harris_test`- The test file with the desired FT configurations.
*`/images` - the subdirectory holding the ***.jpeg*** images (this is where the results will go)

Run the script with the following usage:

```
./test [number of tests per image]
```
=======
>>>>>>> c8ea4512fa02456b39c213d412a0814556c559a8
