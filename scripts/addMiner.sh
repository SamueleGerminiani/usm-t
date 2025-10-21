#!/bin/bash
# Script to create or remove directory structures for miners.

# Default to create mode
CREATE=1

# Argument parsing
if [ "$#" -eq 0 ]; then
    echo "Usage: $0 <miner_name> [create]"
    echo "  miner_name: The name of the miner."
    echo "  create: 1 to create (default), 0 to remove."
    exit 1
fi

MINER_NAME=$1
if [ "$#" -eq 2 ]; then
    CREATE=$2
fi

MINER_DIR="miners/tools/$MINER_NAME"

# --- Directory Creation ---
if [ "$CREATE" -eq 1 ]; then
    echo "Creating directory structure for miner: $MINER_NAME"

    if [ -d "$MINER_DIR" ]; then
        echo "Error: Directory '$MINER_DIR' already exists."
        exit 1
    fi

    # Create base directory
    mkdir -p "$MINER_DIR"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create directory '$MINER_DIR'."
        exit 1
    fi

    # Create subdirectories
    mkdir -p "$MINER_DIR/configurations"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create directory '$MINER_DIR/configurations'."
        # Clean up base directory on failure
        rm -r "$MINER_DIR"
        exit 1
    fi

    mkdir -p "$MINER_DIR/docker"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create directory '$MINER_DIR/docker'."
        # Clean up created directories on failure
        rm -r "$MINER_DIR"
        exit 1
    fi

    mkdir -p "$MINER_DIR/runs"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to create directory '$MINER_DIR/runs'."
        # Clean up created directories on failure
        rm -r "$MINER_DIR"
        exit 1
    fi

    echo "Successfully created directory structure for miner '$MINER_NAME'."

# --- Directory Removal ---
elif [ "$CREATE" -eq 0 ]; then
    echo "Removing directory structure for miner: $MINER_NAME"

    if [ ! -d "$MINER_DIR" ]; then
        echo "Error: Directory '$MINER_DIR' does not exist."
        exit 1
    fi

    rm -r "$MINER_DIR"
    if [ $? -ne 0 ]; then
        echo "Error: Failed to remove directory '$MINER_DIR'."
        exit 1
    fi

    echo "Successfully removed directory structure for miner '$MINER_NAME'."

else
    echo "Error: Invalid create flag. Use 1 to create or 0 to remove."
    exit 1
fi

exit 0
