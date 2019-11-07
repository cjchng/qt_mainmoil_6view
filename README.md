# qt_mainmoil_6view
Sample Qt Moil Application

![screenshot](https://github.com/cjchng/qt_mainmoil_6view/blob/master/images/screen.gif?raw=true)

## 1. Requirement 

If you already have Opencv and Qt creator installed, the followings can be skipped. Opencv can be any version, 3.2.0 is recommented. 

	sudo apt update
	sudo apt upgrade
	sudo apt install build-essential cmake pkg-config
	sudo apt install libjpeg-dev libpng-dev libtiff-dev

	sudo apt install software-properties-common
	sudo add-apt-repository "deb http://security.ubuntu.com/ubuntu xenial-security main"
	sudo apt update
	sudo apt install libjasper1 libjasper-dev
	sudo apt install libgtk-3-dev
	sudo apt install libatlas-base-dev gfortran
	sudo apt install libopencv-dev python-opencv

	sudo apt install qtcreator qt5-default 

## 2. Build and Run

    cd ~
	git clone https://github.com/cjchng/qt_mainmoil_6view.git
	cd ~/qt_mainmoil_6view/moildev_install
	sudo chmod +x install.sh
	sudo ./install.sh 	
	
	qtcreator 

![screenshot](https://github.com/cjchng/qt_mainmoil_6view/blob/master/images/screen.png?raw=true)

	Select "File/Open File or Project" in Qt menu bar, browse to ~/qt_mainmoil_6view/mainmoil.pro
    Select Project on the left side window, click "Configure Project"
	Select Build/ Build project "mainmoil" or press Ctrl+B
 	Press F5 to run 
 

