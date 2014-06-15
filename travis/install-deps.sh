#!/usr/bin/env bash

# Install cppcheck on Ubuntu from source since we need a recent version.
# Will not be needed once travis is updated to Ubuntu 14.04.
install_cppcheck()
{
    git clone https://github.com/danmar/cppcheck.git
    cd cppcheck
    git checkout 1.65
    make SRCDIR=build CFGDIR=/usr/share/cppcheck HAVE_RULES=no -j2
    sudo make install
    # On travis cppcheck ignores CFGDIR. Instead, it looks in $PWD. Compare
    # strace output.
    sudo install -m644 ./cfg/* ../
}

if [ $# -ne 1 ]; then
    echo "Error: expected OS type argument." >&2
    exit 1
fi

os=$1

case "$os" in
    osx)
        brew update
        brew install boost openssl leveldb cppcheck
        brew link --force openssl
		brew tap homebrew/versions
		brew install gcc48
		# Can be replaced back later if you prefer, but
		# keeping the new g++ is a good idea.
		ln -sf /usr/local/bin/g++-4.8 /usr/bin/g++
        ;;
    linux|"" )
        sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
        sudo apt-get -qq update
        sudo apt-get -qq install g++-4.8
        sudo apt-get install libboost1.48-all-dev pkg-config libcurl4-openssl-dev libleveldb-dev libsnappy-dev
        sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
        sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
        install_cppcheck
        ;;
    *)
        echo "Error: unknown OS '$os'" >&2
        exit 1
        ;;
esac


git clone https://github.com/bitcoin/secp256k1.git
cd secp256k1
aclocal
autoreconf --install
./configure
make -j2
sudo make install
sudo ldconfig

#leveldb in Ubuntu 12.04 is outdated
git clone https://code.google.com/p/leveldb/
cd leveldb
make -j2
sudo cp --preserve=links libleveldb.* /usr/local/lib
sudo cp -r include/leveldb /usr/local/include/
sudo ldconfig
