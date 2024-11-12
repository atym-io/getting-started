Getting Started with Atym
==========================
This repository contains materials and sample code for building applications with the Atym Platform.


## Prerequisites 
The Atym toolchain for C/C++ is provided as a Docker container image.  This requires the Docker Desktop or similar to be installed on your development machine.  This can be run interactively or as a Dev Container which can be used with Visual Studio Code.  For use with Visual Studio Code, the [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) extension is required.  The dev container can also be run directly in GitHub via [CodeSpaces](https://docs.github.com/en/codespaces).

## Development with Visual Studio Code
A Development container, or dev continer, is a Docker containers specifically configured to provide a complete development environment.  When working with a dev container, the VS Code editor runs locally on your development machine and the Atym toolchain runs withing the container.  

To develop with a dev container follow these steps:
1. Install all of the prerequisites as specified above
2. Clone the [`atym-io/getting-started`](https://github.com/atym-io/getting-started) repository locally
3. Start VS Code
4. Run the `Dev Containers: Open Folder in Container...` command and select the local folder.

The VS Code window will then reload, but since the container does not exist yet, VS Code will create one and clone the sample repository into an isolated container volume. This will take some time and progress will be displayed in the lower right corner of the VS Code window.  This step only needs to be done once.  

Opening a terminal will give you console access to the container environment.  From there, the samples can be built.  Each of the sample applications has a CMake file and can be built as follows:
```
cd samples/hello-world
mkdir build && cd build
cmake -DWASI_SDK_DIR=/opt/wasi-sdk -DCMAKE_TOOLCHAIN_FILE=/opt/wamr-sdk/app-sdk/wamr_toolchain.cmake ..
make
```

Additional documentation and a step-by-step guide for working with dev containers can be found [here](https://code.visualstudio.com/docs/remote/containers-tutorial).

## Interactive Development
The toolchain container can be launch and used interactively as follows:
```
cd samples
docker run --rm -it -v $(pwd):/home/atym ghcr.io/atym-io/atym-c-toolchain:latest
```

Each of the sample applications has a CMake file and can be built as follows:
```
cd hello-world
mkdir build && cd build
cmake -DWASI_SDK_DIR=/opt/wasi-sdk -DCMAKE_TOOLCHAIN_FILE=/opt/wamr-sdk/app-sdk/wamr_toolchain.cmake ..
make
```
