#!/bin/bash
set -e

# This script should build all the opm-parser dependencies which are installed from source.

function install_python_deps {
    export TRAVIS_PYTHON_VERSION="2.7"
    wget https://repo.continuum.io/miniconda/Miniconda-latest-Linux-x86_64.sh -O miniconda.sh;

    bash miniconda.sh -b -p $HOME/miniconda
    export CONDA_HOME="$HOME/miniconda"
    export PATH="$CONDA_HOME/bin:$PATH"
    hash -r
    conda config --set always_yes yes --set changeps1 no
    conda update -q conda

    conda install numpy
}


function build_libecl {
    install_python_deps
    git clone https://github.com/Statoil/libecl.git
    mkdir -p libecl/build
    pushd libecl/build > /dev/null
    cmake .. && make
    popd > /dev/null
}


#################################################################
build_libecl
