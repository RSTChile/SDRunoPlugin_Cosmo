#define MyAppName "Cosmo (SDRuno Plugin)"
#define MyAppPublisher "RSTChile"
#ifndef MyAppVersion
  #define MyAppVersion "0.1.0"
#endif
#ifndef MySourceDir
  #define MySourceDir "dist"
#endif
#ifndef MyOutputDir
  #define MyOutputDir "output"
#endif

[Setup]
AppId={{72F8B4C8-1A2B-4C3D-8E9F-1234567890AB}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
DefaultDirName={autopf86}\SDRplay\SDRuno\Plugins
DefaultGroupName={#MyAppName}
OutputDir={#MyOutputDir}
OutputBaseFilename=Cosmo_SDRuno_Plugin_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
PrivilegesRequired=admin

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#MySourceDir}\Cosmo.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

[Run]
; Don't try to run the DLL directly - just show completion message