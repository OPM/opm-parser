#
# spec file for package opm-parser
#

%define tag rc1

Name:           opm-parser
Version:        2017.10
Release:        0
Summary:        Open Porous Media - parser library
License:        GPL-3.0
Group:          Development/Libraries/C and C++
Url:            http://www.opm-project.org/
Source0:        https://github.com/OPM/%{name}/archive/release/%{version}/%{tag}.tar.gz#/%{name}-%{version}.tar.gz
BuildRequires:  blas-devel lapack-devel dune-common-devel
BuildRequires:  git suitesparse-devel doxygen bc opm-common-devel devtoolset-6-toolchain 
%{?el6:BuildRequires:  cmake28 boost148-devel}
%{?!el6:BuildRequires:  cmake boost-devel}
BuildRequires:  tinyxml-devel ecl-devel
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
Requires:       libopm-parser1 = %{version}

%description
The Open Porous Media (OPM) initiative provides a set of open-source tools centered around the simulation of flow and transport of fluids in porous media. The goal of the initiative is to establish a sustainable environment for the development of an efficient and well-maintained software suite.

%package -n libopm-parser1
Summary:        Open Porous Media - parser library
Group:          System/Libraries

%description -n libopm-parser1
The Open Porous Media (OPM) initiative provides a set of open-source tools centered around the simulation of flow and transport of fluids in porous media. The goal of the initiative is to establish a sustainable environment for the development of an efficient and well-maintained software suite.

%package devel
Summary:        Development and header files for opm-parser
Group:          Development/Libraries/C and C++
Requires:       %{name} = %{version}
Requires:       blas-devel
Requires:       lapack-devel
Requires:       suitesparse-devel
Requires:       libopm-parser1 = %{version}

%description devel
This package contains the development and header files for opm-parser

%package doc
Summary:        Documentation files for opm-parser
Group:          Documentation
BuildArch:	noarch

%description doc
This package contains the documentation files for opm-parser

%package bin
Summary:        Applications in opm-parser
Group:          Scientific
Requires:       %{name} = %{version}
Requires:       libopm-parser1 = %{version}

%description bin
This package contains the applications for opm-parser

%prep
%setup -q -n %{name}-release-%{version}-%{tag}

# consider using -DUSE_VERSIONED_DIR=ON if backporting
%build
scl enable devtoolset-6 bash
%{?el6:cmake3} %{?!el6:cmake} -DBUILD_SHARED_LIBS=1 -DCMAKE_BUILD_TYPE=RelWithDebInfo  -DSTRIP_DEBUGGING_SYMBOLS=ON -DCMAKE_INSTALL_PREFIX=%{_prefix} -DCMAKE_INSTALL_DOCDIR=share/doc/%{name}-%{version} -DUSE_RUNPATH=OFF -DCMAKE_CXX_COMPILER=/opt/rh/devtoolset-6/root/usr/bin/g++ -DCMAKE_C_COMPILER=/opt/rh/devtoolset-6/root/usr/bin/gcc -DCMAKE_Fortran_COMPILER=/opt/rh/devtoolset-6/root/usr/bin/gfortran %{?el6:-DBOOST_LIBRARYDIR=%{_libdir}/boost148 -DBOOST_INCLUDEDIR=%{_includedir}/boost148} -DBUILD_TESTING=0
make

%install
make install DESTDIR=${RPM_BUILD_ROOT}

%clean
rm -rf %{buildroot}

%post -n libopm-parser1 -p /sbin/ldconfig

%postun -n libopm-parser1 -p /sbin/ldconfig

%files
%doc README.md

%files doc
%{_docdir}/*

%files -n libopm-parser1
%defattr(-,root,root,-)
%{_libdir}/libopm*.so.*

%files devel
%defattr(-,root,root,-)
%{_libdir}/libopm*.so
%{_prefix}/lib/dunecontrol/*
%{_prefix}/lib64/cmake/*
%{_prefix}/lib/pkgconfig/*
%{_datadir}/opm-parser/*
%{_includedir}/*

%files bin
%{_bindir}/*
