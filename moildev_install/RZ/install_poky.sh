#!/bin/bash
sudo cp libmoildev.so.1.0.0 /opt/poky/3.1.31/sysroots/aarch64-poky-linux/usr/lib64/
cd /opt/poky/3.1.31/sysroots/aarch64-poky-linux/usr/lib64/
sudo ln -f -s libmoildev.so.1.0.0 libmoildev.so
sudo ln -f -s libmoildev.so.1.0.0 libmoildev.so.1
sudo ln -f -s libmoildev.so.1.0.0 libmoildev.so.1.0




