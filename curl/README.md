# Cross-Compile Static curl for ARM

This project provides a streamlined way to cross-compile a static version of curl for ARM architecture using Docker.

## Usage

Run the build script:
```
./build-and-run.sh
```

After successful compilation, you'll find a binary named `curl` in the current directory.

## What it does

1. Builds a Docker image with the necessary tools for cross-compilation.
2. Runs a container from this image, which:
   - Downloads the curl source code (version 7.88.1)
   - Configures it for minimal features and static linking
   - Compiles it for ARM architecture
   - Strips the binary to reduce size
3. Copies the resulting binary to your host machine.

## Customization and Debugging

### Entering the Docker Container

If you need to debug or customize the build process (e.g., adding HTTPS support), you can enter the Docker container interactively:

1. Build the Docker image (if not already built):
   ```
   docker build -t curl-cross-compile .
   ```

2. Run and enter the container:
   ```
   docker run -it --rm -v $(pwd):/work curl-cross-compile /bin/bash
   ```

3. You're now inside the container. The source code and compilation script are in the `/work` directory.

Remember to copy any changes you make to the host machine if you want to persist them.
