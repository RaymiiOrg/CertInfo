{
  stdenv,
  qtbase,
  qtdeclarative,
  qmake,
  wrapQtAppsHook,
  lib,
}:
stdenv.mkDerivation {
  pname = "CertInfo";
  version = "1.0";

  src = ../.;

  buildInputs = [qtbase qtdeclarative];
  nativeBuildInputs = [wrapQtAppsHook qmake];

  # Wrapping the inside of the app bundles, avoiding double-wrapping
  dontWrapQtApps = stdenv.hostPlatform.isDarwin;

  postInstall = lib.optionalString stdenv.hostPlatform.isDarwin ''
    mkdir -p $out/Applications
    mv $out/{bin,Applications}/CertInfo.app
    ln -s $out/{Applications/CertInfo.app/Contents/MacOS,bin}/CertInfo
    wrapQtApp $out/Applications/CertInfo.app/Contents/MacOS/CertInfo
  '';

  postPatch = ''
    substituteInPlace src/versioncheck/versioncheck.cpp \
      --replace 'VersionCheck.h' 'versioncheck.h'
    substituteInPlace CertInfo.pro \
      --replace '/opt/$${TARGET}/bin' '$$PREFIX/bin'
  '';
}
