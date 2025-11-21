This project is meant to offer a python module allowing bubling messages from python to c++

BUILD
	REQUIREMENTS
		requires python >= 3.10 and qt6.5
		make sure to have libshiboken6 and libpyside6
		download dependencies using deps_qt6.json
		
	CONFIGURATION
		everything should be already in place but if you need to
		edit the config.json in order to have all required informations
		to better understand the previous step use:
			$ python pyside_config.py --help 
	
	BUILD
		create the build directory
		$ mkdir build
		copy patch file into build folder
		$ cp patch.txt build/
		move into it build folder
		$ cd build
		
		export Qt6_DIR=/home/netresults.wintranet/aru/Qt/6.5.0/gcc_64/lib/cmake/
		export LD_LIBRARY_PATH=/Qt/6.5.3/gcc_64/lib
		
		launch cmake
		$ cmake -H.. -B. -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release
		launch make
		$ make
