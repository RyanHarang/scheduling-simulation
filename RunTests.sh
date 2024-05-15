#!/bin/bash

# Settings 
program="./Simulation"
datafile="giant_tests.txt"
outputfile="results.txt"

# Ranges to test for A, B, and Preemption
values_A=(18 22 26)
values_B=(6 9 12)
values_Preemption=(0 1)

# Clear existing output file
echo "" > $outputfile

# Loop through all combinations
echo "| Q A | Q B | Pre. | End T | Avg W | Max W | Min W |" >> $outputfile
for A in "${values_A[@]}"; do
    for B in "${values_B[@]}"; do
        for Preemption in "${values_Preemption[@]}"; do
            # Run the simulation
            results=$($program $datafile $A $B $Preemption)

            # Extract relevant values using regular expressions
            # end_time=$(echo "$results" | grep -oP 'Start/end time: \K\d+(?=,)')
            end_time=$(echo "$results" | grep -oP 'Start/end time: \K\d+, \d+' | cut -d ',' -f2 | tr -d ' ')
            avg_wait=$(echo "$results" | grep -oP 'Ready time average: \K\d+\.\d+')
            max_wait=$(echo "$results" | grep -oP 'Ready time maximum: \K\d+')
            min_wait=$(echo "$results" | grep -oP 'Ready time minimum: \K\d+')

            echo "| $A  | $B    | $Preemption   | $end_time     | $avg_wait     | $max_wait     | $min_wait |" >> $outputfile
            # Write results to output file
        done
    done
done