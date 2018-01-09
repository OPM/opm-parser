# opm-parser [![Build Status](https://travis-ci.org/OPM/opm-parser.svg?branch=master "TravisCI Build Status")](https://travis-ci.com/OPM/opm-parser)

opm-parser is a small library for parsing ECLIPSE datafiles.


## Dependencies

- [boost](http://www.boost.org) version 1.45 or newer.
- [libecl](https://github.com/Statoil/libecl) 2.2 or newer.


## Build instructions

We assume `boost` is installed in `BOOST_INCLUDE=/opt/software` and that you
wish to install opm-parser in `INSTALL_PATH=/opt/software`.


Obtaining the data:

```
git clone https://github.com/Statoil/libecl
git clone https://github.com/OPM/opm-parser
```


Configure the paths:

```
export INSTALL_PATH=/opt/software
export BOOST_INCLUDE=/opt/software/include
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$INSTALL_PATH/lib:$INSTALL_PATH/lib64
```


Build and install `libecl`:

```
cd libecl
mkdir build
cd build/
cmake -DENABLE_PYTHON=OFF -DBUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH ..
make install
cd ../..
```


Build and install `opm-parser` (you may omit the flag to `Boost_INCLUDE_DIR` if
it is installed at a standardized location):

```
cd opm-parser/
mkdir build
cd build/
cmake -DBoost_INCLUDE_DIR=$BOOST_INCLUDE -DCMAKE_INSTALL_PREFIX=$INSTALL_PATH ..
make install
```
