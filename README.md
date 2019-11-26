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
 	The default {Build directory} of this project is ~/build-mainmoil-Desktop-Debug, please copy 
	a image file to the {Build directory}
        cp ~/qt_mainmoil_6view/images/image.jpg {Build directory}    
        Press F5 to run in Qt creator


        
## 3. Video from Raspberry camera

	copy files in rpi_camera directory to a Raspberry Pi board with a camera, run
	python3 rpi_camera.py
	The Raspberry Pi will start video streaming, then we can specify the "URL" in the mainmoil application, 
	for example,
	http://{Raspberry Pi's IP}:8000/stream.mpeg  
	Click on "Camera" button to start receiving the video stream.
	
