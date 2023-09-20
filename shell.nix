let
    pkgs = import (fetchTarball "https://github.com/NixOS/nixpkgs/archive/nixos-22.11.tar.gz") {};
in pkgs.mkShell { 
  buildInputs = [
    pkgs.libarchive
    pkgs.cmake
    pkgs.clang
  ];
}
