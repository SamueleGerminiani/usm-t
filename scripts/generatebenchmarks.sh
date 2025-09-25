#!/bin/bash

# Directory containing the .sh files
directory="tool/synthetic_gen/miningChallengesGenerator/"

# Loop through each .sh file in the directory
for script in "$directory"/*.sh; 
do
    # Launch the script with parameter 1
    echo "Launching $script"
    bash "$script" 0
done