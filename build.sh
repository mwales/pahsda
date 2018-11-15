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

# Build the test plugin
mkdir testPlugin
cd testPlugin

qmake ../../../src/plugins/testPlugin/testPlugin.pro

if [ $? -ne 0 ];
then
	echo "Error building the test plugin"
	exit 2
fi

make -j8
if [ $? -ne 0 ];
then
	echo "Error building the testPluging"
	exit 2
fi

echo "Test Plugin successfully built"

# Copy the plugin binary to the directory the executable we just build wil be looking for it in
cp libtest_plugin.so ../

# Get ready to build the DUML Lite plugin
cd ..
mkdir dumlLitePlugin
cd dumlLitePlugin


qmake ../../../src/plugins/dumlLite/dumlLitePlugin.pro

if [ $? -ne 0 ];
then
	echo "Error building the DUML Lite plugin"
	exit 2
fi

make -j8
if [ $? -ne 0 ];
then
	echo "Error building the DUML Lite plugin"
	exit 2
fi

echo "DUML Lite Plugin successfully built"

cp libdumlLite_plugin.so ../

echo "Build completed"

