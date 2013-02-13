A simple `libnss` provider to resolve any `*.dev` domains to localhost. This may
not seem particularly useful, but I'm too lazy to add every hostname I use for
my box to naginx config and the `/etc/hosts` file.

[nss-myhostname](http://0pointer.de/lennart/projects/nss-myhostname/) has been
a BIG help, Much of the `_fill_hostent` function has been taken from it.

The code is released under LGPL license.