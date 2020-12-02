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
	
	# check for error
	let err=$(($? >> 7))
	if [ $err -eq 1 ]
	then 
		echo "Error returned during benchmark, exitng now"
		exit
	fi
done

echo " "

# var to track the number of seg faults/other errors
let FAILS=0 
let TIMEOUTS=0

# run test case for each image NUM_TESTS times
for img in ./images/*.jpeg;
do
	for counter in $(seq 1 $NUM_TESTS);
	do
		echo "--- running test $counter for $img ---"
		timeout 30m -s SIGTERM ./Harris_test $img
		
		# check for timeout 
		if [ $? -eq 124 ]
		then
			let TIMEOUTS++
			echo "Run timed out, there have been $TIMEOUTS timeouts so far..."
		fi

		# check for seg fault or other error
		let err=$(($? >> 7))
		if [ $err -eq 1 ]
		then 
			let FAILS++
			echo "Run Failed, there have been $FAILS seg faults so far..."
		fi
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
printf "tests completed in %d hours %02d minutes and %02d seconds\n" $HOURS $MINUTES $SECONDS
printf "%d seg faults occured\n" $FAILS
printf "%d timeouts occurred\n\n" $TIMEOUTS

exit
