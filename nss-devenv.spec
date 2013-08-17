Name:           nss-devenv
Version:        0.0.b503ddcd
Release:        1%{?dist}
Summary:        Libnss plugin for *.dev host name resolution

Group:          System Environment/Libraries
License:        LGPLv2+
URL:            https://github.com/aliem/libnss_devenv

# git archive --format tar b503ddcd | gzip -9c > libnss_devenv.b503ddcd.tar.bz2
Source0:        libnss_devenv.b503ddcd.tar.bz2

%description
A simple libnss provider to resolve any *.dev domains to localhost.
This may not seem particularly useful, but I'm too lazy to add
every hostname I use for my box to nginx config and the /etc/hosts file.

%prep
%setup -q -c


%build
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT/%{_libdir}
install -m 755 libnss_devenv.so.2 $RPM_BUILD_ROOT/%{_libdir}/

mkdir -p $RPM_BUILD_ROOT/%{_docdir}/%{name}-%{version}
install -m 644 README.md $RPM_BUILD_ROOT/%{_docdir}/%{name}-%{version}/
install -m 644 LICENSE $RPM_BUILD_ROOT/%{_docdir}/%{name}-%{version}/

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%{_libdir}/libnss_devenv.so.2
%doc README.md
%doc LICENSE

%changelog
* Sat Aug 17 2013 Jiří Zapletal <yaplik@gmail.com> - 0.0.b503ddcd-1
- Initial packaging

