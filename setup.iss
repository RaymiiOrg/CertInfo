; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

; build deploy on windows
; \qt5\5.15.2\mingw81_32\bin\qtenv2.bat
; cd \Users\user\git\build-CertInfo-Desktop_Qt_5_15_2_MinGW_64_bit-Release\release
; \qt5\5.15.2\mingw81_32\bin\windeployqt.exe CertInfo.exe 

#define MyAppName "CertInfo"
#define MyAppVersion "2024.01"
#define MyAppPublisher "Sparkling Network"
#define MyAppURL "https://raymii.org/"
#define MyAppExeName "CertInfo.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{67E9D8A9-58D4-4989-B54D-09E39CA26C65}}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=C:\Users\user\git\CertInfo\LICENSE
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
OutputDir=C:\Users\user\git\setup
OutputBaseFilename=CertInfoSetup
SetupIconFile=C:\Users\user\git\CertInfo\certinfo.ico
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\user\git\build-CertInfo-Desktop_Qt_5_15_2_MinGW_64_bit-Release\release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\user\git\build-CertInfo-Desktop_Qt_5_15_2_MinGW_64_bit-Release\release\*"; DestDir: "{app}"
Source: "C:\Qt5\Tools\OpenSSL\Win_x86\bin\*.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\windows\system32\msvcr100.dll"; DestDir: "{app}";
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

