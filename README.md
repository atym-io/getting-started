Getting Started with Atym
==========================
This repository contains materials and sample code for building applications with the Atym Platform, including the Atym toolchain for C/C++ via a **dev container**.

[Dev containers](https://code.visualstudio.com/docs/remote/containers-tutorial) are essentially a Docker container image that is specifically configured to provide a complete development environment. When working with this dev container, it will run locally on your development machine (via VS Code) and the Atym toolchain runs within that container. 

**Note:** The included dev container can also be run interactively, without the need for VSCode.

## Prerequisites

Please install all of the following prerequisites if you intend to use the dev contianer within VS Code. Alternatively, if you're only planning on running the dev container locally you only need to install **Docker Desktop**.

* [Docker Desktop](https://www.docker.com/products/docker-desktop/)
* [Visual Studio Code](https://code.visualstudio.com/)
* [Dev Containers](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers) Extension in VSCode

---

### Development with Visual Studio Code

To develop with the Atym dev container using VS Code follow these steps:

1. Install all of the mentioned pre-requesites
2. Clone the [`atym-io/getting-started`](https://github.com/atym-io/getting-started) repository locally
3. Start VS Code
4. Run the `Dev Containers: Open Folder in Container...` from the Command Palette (`Cmd + Shift + P` for Mac, or `Ctrl + Shift + P` for Windows).
5. Select the folder containing the cloned repository.

The VS Code window will then reload, but since the container does not exist yet, VS Code will create one and clone the sample repository into an isolated container volume. This will take some time and progress will be displayed in the lower right corner of the VS Code window. This step only needs to be done once.

Opening a terminal will give you console access to the container environment. From there, the samples can be built. 

---

### Interactive Development
The toolchain container can also be launched and used *interactively* as follows, without the need for VS Code:
```
cd samples
docker run --rm -it -v $(pwd):/home/atym ghcr.io/atym-io/atym-c-toolchain:latest
```

## Building the Samples

Each of the sample applications has a CMake file and can be built as follows:
```
cd samples/sample_name
mkdir build && cd build
cmake -DWASI_SDK_DIR=/opt/wasi-sdk -DCMAKE_TOOLCHAIN_FILE=/opt/wamr-sdk/app-sdk/wamr_toolchain.cmake ..
make
```

**Note:** This step results in the creation of a file named `sample_name.wasm`, which will be used by the Atym CLI to create an Atym container in the next step.

Next, jump up out of the *build* directory and back to the project root.

```
cd ..
```

Then build the container using the Atym CLI (provided in the Dev Container)

```
atym build
```

You should now see a new `.atym` directory in your home directory (`~/.atym`) that contains the containers name (as specified in `build.yml`), as well as all of the container contents.

## What's Next?
Now that you've built your first Atym container, it's time to deploy it to your device! 
Visit our [Quickstart Guide](https://docs.atym.io/quickstart) to learn how to set up the Atym Runtime on your board, register your device with the Atym Hub, and deploy your first containerized application. 

## Important links
Explore these sections in our docs to get more framiliar with developing Atym containers.

- [Container Specification](https://docs.atym.io/architecture/containers/overview) - Understand the Atym container format
- [Container APIs](https://docs.atym.io/reference/api) - Discover available container APIs
- [CLI Reference](https://docs.atym.io/reference/cli) - Master the Atym CLI capabilities

Or, visit our [documentation](https://docs.atym.io) home for comprehensive guides and reference materials.