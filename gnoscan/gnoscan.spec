# gnoscan.spec: used for building rpms.
# -*- rpm-spec -*-

%define name gnoscan 
%define version 0.1
%define release 1

Name: %{name}
Version: %{version}
Release: %{release}
Summary: Use GnoScan to scan and secure your network.
Copyright: GPL
Group: Applications/Utilities
Source: %{name}-%{version}.tar.gz
Buildroot: %{_tmppath}/%{name}-%{version}-%{release}-root
URL: http://gnoscan.berlios.de/
Packager: Andreas Bauer <baueran@users.berlios.de>
Requires: gnome-libs >= 1.2.0
Requires: libsigc++ >= 1.0.0
Requires: gnomemm >= 1.1.12
Requires: gtkmm >= 1.2.4

%description
GnoScan is a simple network scan and security utility with an intuitive
graphical user interface. It runs under the Gnome environment and is an
independent application that does not depend on third-party toolkits.

%prep
%setup

%build
./configure
make

%install
make install prefix=$RPM_BUILD_ROOT%{_prefix} sysconfdir=$RPM_BUILD_ROOT%{_sysconfdir}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO
%{_bindir}/gnoscan

%changelog
* Thu Feb 01 2001 Andreas Bauer <baueran@users.berlios.de>

- Initial creation of rpm package
