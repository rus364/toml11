{
  description = "toml11 3.5.0";

  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs/nixos-unstable;
  };

  outputs = { self, nixpkgs, ... }: {

    defaultPackage.x86_64-linux =
      with import nixpkgs { system = "x86_64-linux"; };

      stdenv.mkDerivation {
        pname = "toml11";
        version = "3.5.0";
        src = self;

        nativeBuildInputs = [ cmake ];
        buildInputs = [ boost172 ];

        cmakeFlags = [
          "-DCMAKE_INSTALL_PREFIX:PATH=$out"
          "-Dtoml11_BUILD_TEST:BOOL=OFF"
        ];

        buildPhase = "cmake .";

        installPhase = ''
          make install;
        '';

      };

  };

}
