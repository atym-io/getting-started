Blinky Sample
=============

## Building the Blinky sample
```
mkdir build && cd build
cmake -DWASI_SDK_DIR=/opt/wasi-sdk -DCMAKE_TOOLCHAIN_FILE=/opt/wamr-sdk/app-sdk/wamr_toolchain.cmake ..
make
```

This will create a container image called `blinky.nbx`.  



## Deploying the Blinky sample
This can be pushed to the Nubix Orchestation Hub using the following command:
```
nubix container service push blinky:latest ./build/blinky.nbx
```

The application is defined in `manifest.yaml`.  This can be pushed to the Hub as follows:
```
nubix container application push blinky-app:latest manifest.yaml
```

Finally, this can be deployed to your device as follows:
```
nubix container application assign blinky-app:latest [DEVICE-ID]
```


