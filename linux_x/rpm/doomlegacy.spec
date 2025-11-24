Summary: DOOM Legacy for Linux X-Windows and Mesa
Name: doomlegacy
Version: 1.29
Release: 22
Source: doomlegacy-20000330.tar.gz
Source8: doom3.wad
Patch: doomlegacy-makefile.patch
Copyright: GPL, perhaps except for doom3.wad
Group: Amusements/Games
Buildroot: /var/tmp/doom-buildroot
#Requires: doom-data
#Provides: doom
Packager: Erling Jacobsen <erlingj@dk-online.dk>

%description
This is Doom Legacy for Linux X11 and Mesa.

%changelog
* Thu Mar 30 2000 Erling Jacobsen <linuxcub@email.dk>
- Updated to new version from CVS, fixed makefile.patch :-(
* Tue Mar 28 2000 Erling Jacobsen <linuxcub@email.dk>
- Added master volume for sound effects. Fixed scripts.
* Wed Mar 25 2000 Erling Jacobsen <linuxcub@email.dk>
- Updated to new version from CVS
* Wed Mar 13 2000 Erling Jacobsen <linuxcub@email.dk>
- Updated to new version from CVS
* Wed Mar 12 2000 Erling Jacobsen <linuxcub@email.dk>
- Updated to new version from CVS
* Wed Mar 8 2000 Erling Jacobsen <linuxcub@email.dk>
- Updated to new version from CVS
* Fri Mar 3 2000 Erling Jacobsen <linuxcub@email.dk>
- Added patch which tries to fix the nobackpoly problem.
* Fri Mar 3 2000 Erling Jacobsen <linuxcub@email.dk>
- Removed commercial wad-files from the package.
* Tue Feb 29 2000 Erling Jacobsen <linuxcub@email.dk>
- Replaced doom-1.10 with doomlegacy-20000228 and patches.

%prep
%setup -n doomlegacy
%patch -p1

%build
chmod a+x linux_x/rpm/doom
chmod a+x linux_x/rpm/gldoom
mkdir objs
mkdir bin
make O=objs BIN=bin
make O=objs BIN=bin dll
cd linux_x/musserv
mkdir linux
make -f Makefile.linux
cd ../sndserv
mkdir linux
make
cd ../..

%install
mkdir -p ${RPM_BUILD_ROOT}/etc/X11/wmconfig
cp linux_x/rpm/doom.wmconfig ${RPM_BUILD_ROOT}/etc/X11/wmconfig/doom
cp linux_x/rpm/gldoom.wmconfig ${RPM_BUILD_ROOT}/etc/X11/wmconfig/gldoom

mkdir -p ${RPM_BUILD_ROOT}/usr/lib/games/doom
mkdir -p ${RPM_BUILD_ROOT}/usr/games

cp bin/llxdoom bin/r_opengl.so ${RPM_BUILD_ROOT}/usr/lib/games/doom
cp linux_x/sndserv/linux/llsndserv ${RPM_BUILD_ROOT}/usr/lib/games/doom
cp linux_x/musserv/linux/musserver ${RPM_BUILD_ROOT}/usr/lib/games/doom
( cd linux_x/rpm ; cp doom gldoom ${RPM_BUILD_ROOT}/usr/games/ )

cp ${RPM_SOURCE_DIR}/doom3.wad ${RPM_BUILD_ROOT}/usr/lib/games/doom/doom3.wad

%clean
rm -rf ${RPM_BUILD_ROOT}

%files
%defattr(644,root,root,755)
%doc _doc/*.txt _doc/copying
%dir /usr/lib/games/doom
%attr(755,root,root) /usr/games/doom
%attr(755,root,root) /usr/games/gldoom
%attr(755,root,root) /usr/lib/games/doom/musserver
%attr(755,root,root) /usr/lib/games/doom/llsndserv
%attr(755,root,root) /usr/lib/games/doom/r_opengl.so
%attr(755,root,root) /usr/lib/games/doom/llxdoom
%attr(644,root,root) %config /etc/X11/wmconfig/doom
%attr(644,root,root) %config /etc/X11/wmconfig/gldoom
%attr(644,root,root) /usr/lib/games/doom/doom3.wad
