# Moildev library installation 

Get into the directory corresponding to your platform, run install.sh to install the shared library to your system.

## 1. x86_64_Qt5.6.3

To build Qt_Mainmoil on Desktop/ Ubuntu 22.04, you need to install this version on your system.

```
cd x86_64_Qt5.6.3
sudo ./install.sh
```	        
## 2. RZ

For cross compilation for Renesas RZ/G2L and RZ/V2H, suppose your SDK is installed at
```
/opt/poky/3.1.31/
```

Run the below command to install the libmoildev.so library.

```
cd RZ
sudo ./install_poky.sh
```

On the other hand, we also need to parepare a copy of shared library for runtime execution. If you have SD card with Yocto build Linux image on it, you can run the command,  

( on PC )
```
cd RZ
sudo ./install_sd.sh
```

Or, copy the files to SD card and run the installation on Renesas RZ/G2L or RZ/V2H, as below,

( on Renesas )
```
cd RZ
sudo ./install.sh
```

## 3. NVIDIA 

For NVIDIA Jetson TX2, 

```
cd NVIDIA_Jetson
sudo ./install.sh
```





   



	
