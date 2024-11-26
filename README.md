Getting Started with Atym
==========================
This repository contains materials and sample code for building applications with the Atym Platform, including the Atym toolchain for C/C++ via a dev container.

[Dev containers](https://code.visualstudio.com/docs/remote/containers-tutorial) are essentially a Docker container image that is specifically configured to provide a complete development environment. When working with this dev container, it will run locally on your development machine (via VS Code) and the Atym toolchain runs within that container. Note, the included dev container can also be run interactively, without the need for VSCode.

Also, this guide assumes that you have already created an account and registered at least one device with the Atym Hub. If you've not yet done those steps, please follow the "[Getting Started](https://docs.atym.io/quickstart)" guide in our docs.

---

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
The toolchain container can be launched and used *interactively* as follows, without the need for VS Code:
```
cd samples
docker run --rm -it -v $(pwd):/home/atym ghcr.io/atym-io/atym-c-toolchain:latest
```

## Building the Samples

Each of the sample applications has a CMake file and can be built as follows:
```
cd samples/<sample name>
mkdir build && cd build
cmake -DWASI_SDK_DIR=/opt/wasi-sdk -DCMAKE_TOOLCHAIN_FILE=/opt/wamr-sdk/app-sdk/wamr_toolchain.cmake ..
make
```

This step results in a WASM module called, `<sample_name.wasm>`, which will be used by Atym in the next step to create an Atym Container.

Next, jump up out of the *build* directory, and build the container using the Atym CLI (provided in the Dev Container)

```
cd ..
atym build
```

**Note:** The Atym *build* command creates an Atym container image using the specified image definition file. If none is provided, Atym will look for the default image file name `build.yml`.

## Deploying the Samples

Once built, Atym containers can be easily deployed to your device with just *two* commands.

1. First, deploy your newly created Atym container to the Atym Hub.

```
atym push IMAGE_NAME
```

**Note:** The `IMAGE_NAME` argument is simply the container image filename, minus the extension. For example, if you were building the "*hello world*" sample, you'd simply run `atym push hello-world`.

2. Next, to load your newly created Atym container onto your device, simply run the following command.

```
atym run IMAGE_NAME DEVICE_ID
```
**Note:** Where `DEVICE_ID` is simply your devices ID as found in the Atym Hub.

If all goes well, you should see your container run in the terminal output below!

---

## Next Steps
Now that you've successfully built and deployed your first Atym container, explore these sections in our docs to get more framiliar with developing Atym containers.

- [Container Specification](https://docs.atym.io/architecture/containers/overview) - Understand the Atym container format
- [Container APIs](https://docs.atym.io/reference/api) - Discover available container APIs
- [CLI Reference](https://docs.atym.io/reference/cli) - Master the Atym CLI capabilities

Or, visit our [documentation](https://docs.atym.io) home for comprehensive guides and reference materials.