To run the unit tests:

  $ bash make.sh

Known issues: secp256k1 needs to use the GMP (install libgmp-dev) backend
not the OpenSSL one. I think it has a bug with repeated
initialization/shutdown when using the SSL backend.

  $ sudo apt-get install libgmp-dev
  $ cd secp256k1
  $ ./configure
  $ make install

