#define MyAppName "modbus programmer"
#define MyAppVersion "1.0"
#define MyAppExeName "mbloader_qt54_gui.exe"

[Setup]
AppId={{8C70F1E2-AB34-4C5D-9D71-6D9A7F2B1041}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
WizardStyle=dark
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}
Compression=lzma2
SolidCompression=yes
OutputDir=output
OutputBaseFilename=modbus_programmer_setup
ArchitecturesInstallIn64BitMode=x64
PrivilegesRequired=admin
SetupIconFile=installer.ico

[Files]
Source: "release\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Tasks]
Name: "desktopicon"; Description: "Create a Desktop shortcut"; GroupDescription: "Additional icons:"