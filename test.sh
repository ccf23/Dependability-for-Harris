#! /bin/bash

# parse user input values
if [ $# -eq 1 ]
then
	NUM_TESTS=$1
	echo "configured to run $NUM_TESTS tests for each image"
	echo ""
else
	echo "usage: ./test [number of tests]"
	exit
fi

# start time to calc test run time
STARTTIME=`date +%s`

# run benchmark for each image
for img in ./images/*.jpeg;
do
	echo "--- running benchmark for $img ---"
	./Harris_bench $img benchmark
	sleep 1
done

echo " "

# run test case for each image NUM_TESTS times
for img in ./images/*.jpeg;
do
	for counter in $(seq 1 $NUM_TESTS);
	do
		echo "--- running test $counter for $img ---"
		./Harris_test $img
		sleep 1
	done 
	echo "--------------------------------------------------------------------------"
done

# determine runtime parameters
ENDTIME=`date +%s`
let DURATION=ENDTIME-STARTTIME
let HOURS=DURATION/3600
let MINUTES=(DURATION/60)%60
let SECONDS=DURATION%60

echo ""
printf "tests completed in %d hours %02d minutes and %02d seconds\n\n" $HOURS $MINUTES $SECONDS

exit