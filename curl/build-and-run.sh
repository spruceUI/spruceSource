#!/bin/bash

# Build the Docker image
docker build -t curl-cross-compile .

# Run the container
docker run --rm -v $(pwd):/work curl-cross-compile

# Check if the binary was created
if [ -f "curl-7.88.1/src/curl" ]; then
    echo "Compilation successful. Binary located at curl-7.88.1/src/curl"
    # Optionally, move the binary to the current directory
    mv curl-7.88.1/src/curl ./curl
    echo "Binary moved to ./curl"
else
    echo "Compilation failed. Binary not found."
fi