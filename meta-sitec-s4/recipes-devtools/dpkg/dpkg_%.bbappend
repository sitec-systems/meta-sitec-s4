pkg_postinst_${PN}-perl () {
#!/bin/sh -e
ln -s /usr/lib/perl/Dpkg /usr/lib/perl/5.20.0/Dpkg
ln -s /usr/lib/perl/Dpkg.pm /usr/lib/perl/5.20.0/Dpkg.pm
}

pkg_prerm_${PN}-perl () {
#!/bin/sh -e
rm -rf  /usr/lib/perl/5.20.0/Dpkg
rm -rf /usr/lib/perl/5.20.0/Dpkg.pm
}
