# SPEC file

%global c_vendor    %{_vendor}
%global gh_owner    %{_owner}
%global gh_cvspath  %{_cvspath}
%global gh_project  %{_project}

Name:      %{_package}
Version:   %{_version}
Release:   %{_release}%{?dist}
Summary:   Output the most frequently used words in the input file.

Group:     Applications/Services
License:   %{_docpath}/LICENSE
URL:       https://%{gh_cvspath}/%{gh_project}

BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-%(%{__id_u} -n)

Provides:  %{gh_project} = %{version}

%description
Parse an input file and return the most frequently used words with their frequency.
In this context a word is a continuous sequence of characters from 'a' to 'z'.
The words are case-insensitive, so uppercase letters are always mapped in lowercase.

%build
#(cd %{_current_directory} && make build)

%install
rm -rf $RPM_BUILD_ROOT
(cd %{_current_directory} && make install DESTDIR=$RPM_BUILD_ROOT)

#%clean
#rm -rf $RPM_BUILD_ROOT
#(cd %{_current_directory} && make clean)

%files
%attr(-,root,root) %{_binpath}/%{_project}
%attr(-,root,root) %{_docpath}
%attr(-,root,root) %{_manpath}/%{_project}.1.gz
%docdir %{_docpath}
%docdir %{_manpath}

%changelog
* Wed Nov 18 2015 Nicola Asuni <info@tecnick.com> 1.0.0-1
- Initial Commit

