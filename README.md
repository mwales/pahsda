# pahsda
Protocol Analyzer Highlighting Structured Data for Analysis

Pronouned like pasta, because I like food.

# Dependencies

Requires Qt5 with QtSerialPort package.  If you install Qt manually using the
downloadable installer from Digia, it comes with the serial port library
already installed.

You can then uncomment the line in build.sh at the beginning and specify the
path to the qmake binary that was installed with the manually installed
version.

```
QMAKE_PATH=/opt/Qt5.12.2/5.12.2/gcc_64/bin/
```

If you are on an Ubuntu system, add the following package to install Qt with
serial port support. After installing this package, you should be able to
build pahsda with the Ubuntu packaged version of qt5-dev.

* libqt5serialport5-dev

# Building

```
cd pahsda
git submodule update --init --recursive
./build.sh
```

# Modularity

DUML Lite is a plugin for analyzing DUML traffic from DJI drones.  You can use
the files in src/plugins/dumlLite as a template for making your own plugin to
analyze different protocols.

I plan to make the IO initialization a plugin as well.

# Screenshots

![Screenshot](images/Screenshot.png)


