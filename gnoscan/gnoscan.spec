# gnoscan.spec: used for building rpms.
# -*- rpm-spec -*-

%define name GnoScan 
%define version 0.1
%define release 1

Name:           %{name}
Version:        %{version}
Release:        %{release}
Summary:        Use GnoScan to scan and secure your network.
Copyright:      GPL
Group:          Applications/Utilities
Source:         %{name}-%{version}.tar.gz
Buildroot:      %{_tmppath}/%{name}-%{version}-%{release}-root
URL:            http://gnoscan.berlios.de/
Packager:       Andreas Bauer <baueran@users.berlios.de>

Requires:       gnomemm

%description
GnoScan is a multi-threaded network scan and security utility with an
intuitive graphical user interface. It runs under the Gnome environment and
is an independent application that does not depend on third-party toolkits.

%prep
%setup

%build
./configure --prefix=%{_prefix}
make

%install
make install prefix=$RPM_BUILD_ROOT%{_prefix} \
             sysconfdir=$RPM_BUILD_ROOT%{_sysconfdir}

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-, root, root)
%doc AUTHORS COPYING ChangeLog INSTALL NEWS README TODO
%{_prefix}/doc/gnoscan/index.html
%{_datadir}/pixmaps/gnoscan/tb_search.xpm
%{_bindir}/gnoscan

%changelog
* Tue Feb 20 2001 Andreas Bauer <baueran@users.berlios.de>

- Fixed problems with pixmaps/ and doc/ that accidently got copied to the wrong
  place

* Mon Feb 19 2001 Andreas Bauer <baueran@users.berlios.de>

- Added pixmaps/ and doc/ to the specs

* Sun Feb 18 2001 Andreas Bauer <baueran@users.berlios.de>

- Changed name from gnoscan to GnoScan, otherwise rpm would complain

* Thu Feb 15 2001 Andreas Bauer <baueran@users.berlios.de>

- Added Docdir: and make install-strip

- Fixed up description

* Thu Feb 01 2001 Andreas Bauer <baueran@users.berlios.de>

- Initial creation of rpm package
