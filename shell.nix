with import <nixpkgs> {};
stdenv.mkDerivation {
  name = "env";
  nativeBuildInputs = [ cmake clang-tools ];
  buildInputs = [ libarchive ];

  shellHook = ''
    echo "We are now in a C development environment, with cmake and gcc, libarchive and gcc available."
    echo "Use exit to enter and return to your normal environment."
  '';
}
