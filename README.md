# meta-sitec-s4

This project contains the sources to build the board support package (BSP) for
the (S4)[www.sitec-systems.de/der-telematik-spezialist/produkte/s4-commpro-x.html].

## Build

To build the BSP you need a Linux based development environment with the following
packages installed:

* git
* docker
* bash

For setup the work directory you need execute the following command:

```shell
./scripts/init-project
```

The script downloads all depenencies needed for the build. This step is only
neccessary for the initial project setup.

To build the image run the following command:

```shell
./script/bitbake-exec -t sitec-image
```
