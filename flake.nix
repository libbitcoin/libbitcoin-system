{
  description = "Reference Implementation for PV286 project on BIP380 ";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
    # old-boost-pkgs.url = "github:NixOS/nixpkgs/7a5cc174569052f935d642b227ecf53584595147";
    # custom-nixpkgs.url = "github:quapka/nixpkgs/customPkgs";
  };

  outputs =
    {
      self,
      nixpkgs,
      # old-boost-pkgs,
      flake-utils,
      ...
    }:
    flake-utils.lib.eachDefaultSystem (
      system:
      let
        overlays = [ ];
        pkgs = import nixpkgs { inherit system overlays; };
        # oldBoostPkgs = import old-boost-pkgs { inherit system overlays; };
        # customPkgs = import custom-nixpkgs { inherit system overlays; };
      in
      with pkgs;
      {
        packages = rec {
          # default = stdenv.mkDerivation {
          #   name = "libbitcoin-system";

          #   src = fetchFromGitHub {
          #     owner = "libbitcoin";
          #     repo = "libbitcoin-system";
          #     rev = "v3.8.0";
          #     hash = "sha256-7fxj2hnuGRUS4QSQ1w0s3looe9pMvE2U50/yhNyBMf0=";
          #   };
          #   # Configure step differs from the oficial README.md as Boost Chrono is not found.
          #   configurePhase = ''
          #     ./autogen.sh
          #     ./configure --with-boost-libdir=${lib.makeLibraryPath [ oldBoostPkgs.boost175 ]} --prefix=$out
          #   '';

          #   buildPhase = ''
          #     make -j$(nproc)
          #   '';

          #   installPhase = ''
          #     make install
          #   '';

          #   nativeBuildInputs = [
          #      glib
          #      cmake
          #      secp256k1
          #      # libbitcoin-system requires Boost version < 1.76
          #      # oldBoostPkgs.boost175
          #      boost
          #      pkg-config
          #      libtool
          #      bison
          #      flex
          #      fontforge
          #      makeWrapper
          #      pkg-config
          #      gcc
          #      libiconv
          #      autoconf
          #      automake
          #   ];
          # };
        };
        devShells.default = mkShell {
          buildInputs = [
              glib
              cmake
              secp256k1
              # oldBoostPkgs.boost175
              boost177
              boost177.dev
              libtool
              bison
              flex
              fontforge
              makeWrapper
              pkg-config
              gcc
              libiconv
              autoconf
              automake
              # pthread
          ];

          BOOST_ROOT = with pkgs; lib.makeLibraryPath [ boost177 ];
          # BOOST_ROOT = with pkgs; lib.makeLibraryPath [ boost ];
        };
      }
    );
}
