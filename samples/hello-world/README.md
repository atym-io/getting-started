Hello World Sample
==================

## Building the Hello World sample
```
mkdir build && cd build
cmake -DWASI_SDK_DIR=/opt/wasi-sdk -DCMAKE_TOOLCHAIN_FILE=/opt/wamr-sdk/app-sdk/wamr_toolchain.cmake ..
make
```

This will create a container image called `hello-world.nbx`.  



## Deploying the Hello World sample
This can be pushed to the Nubix Orchestation Hub using the following command:
```
nubix container service push hello-world:latest ./build/hello-world.nbx
```

The application is defined in `manifest.yaml`.  This can be pushed to the Hub as follows:
```
nubix container application push hello-world-app:latest manifest.yaml
```

Finally, this can be deployed to your device as follows:
```
nubix container application assign hello-world-app:latest [DEVICE-ID]
```


