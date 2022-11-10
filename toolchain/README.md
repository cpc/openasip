# Toolchain (Docker Image)

This directory contains a `Dockerfile` and some shell scripts that automate the process of building the OpenASIP project. It follows the steps in the [INSTALL](https://github.com/cpc/openasip/blob/master/tce/INSTALL) document, in particular automating the building of llvm, and tce.

The image also exposes a VNC server to interact with the graphical tools. Under the hood this is done using a virtual x11 server ( xvfb ), and x11vnc.

## Usage

Currently there is no standard hosted image, so you'll have to build it yourself.

Building the image (do this from the openasip root directory): `docker build -t openasip-testing -f toolchain/Dockerfile --progress tty toolchain`

To run the image: `docker run -p5900:5900 openasip-testing`.

To drop into the image you can use [docker exec](https://docs.docker.com/engine/reference/commandline/exec/). Example: `docker exec -ti $(docker ps | grep openasip-testing | awk '{print $1}') /usr/bin/bash`.

 * Once you're inside the container remember to `source /root/tce-env.sh`.
 * Set the `DISPLAY` environment variable ( `export DISPLAY=:0` ).
 * Run `prode &` or similar.
 * Connect using VNC to view the gui.

## Further toolchain work

 * Cleanup some of the scripts a bit.
 * Version pin `ubuntu`.
 * Host a standard image.
 * Setup kubernetes resources and add in `distcc` to facilitate parallel builds ( separate project? )
