let
  pkgs = import <nixpkgs> {};
in
pkgs.stdenv.mkDerivation rec {
  pname = "chip8";
  version = "0.0.1";

  src = ./src;

  buildInputs = [
    pkgs.ncurses
  ];

  configurePhase = ''
  '';

  buildPhase = ''
    make
  '';

  installPhase = ''
    mkdir -p $out/bin
    mv chip8 $out/bin/chip8
  '';
}
