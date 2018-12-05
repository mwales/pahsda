#!/bin/bash

if [ -d build ];
then
	echo "Cleaning the build directory"
	rm -rf build/*
else
	echo "Creating a build directory"
	mkdir build
fi

if [ $? -ne 0 ];
then
	echo "Error creating or cleaning the build directory"
	exit 1
fi

# Go into the build directory and build the main application
cd build
qmake ../src/pahsda.pro

if [ $? -ne 0 ];
then
	echo "qmake failed for the main project"
	exit 1
fi

make -j8
if [ $? -ne 0 ];
then
	echo "make failed on the main project"
	exit 1
fi

echo "Main project built successfully!"

mkdir plugins
cd plugins

for pluginDirName in ../../src/plugins/*; do
	pluginName=$(basename $pluginDirName)

	if [[ $pluginName =~ build-.* ]]; then
		echo -e "\nIgnoring build directory $pluginName in plugins directory"
		continue
	fi

	echo -e "\nBuilding plugin $pluginName"
	
	mkdir $pluginName
	cd $pluginName
	
	echo "Now in $(pwd)"
	qmake ../../../src/plugins/${pluginName}/*.pro

	if [ $? -ne 0 ];
	then
		echo "qmake failed for the plugin $pluginName"
		exit 1
	fi

	make -j8
	
	if [ $? -ne 0 ];
	then
		echo "Compilation failed for the plugin $pluginName"
		exit 1
	fi

	mv *.so ../
	
	cd ..

	echo "*** Plugin $pluginName build success!"
done

echo "Build completed"

