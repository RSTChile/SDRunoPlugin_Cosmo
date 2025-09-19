[Setup]
AppName=Cosmo SDRuno Plugin
AppVersion={#MyAppVersion}
AppPublisher=RSTChile
DefaultDirName={autopf32}\SDRplay\SDRuno\Plugins
DefaultGroupName=Cosmo SDRuno Plugin
DisableProgramGroupPage=yes
OutputDir={#MyOutputDir}
OutputBaseFilename=Cosmo_Win32_Setup_{#MyAppVersion}
Compression=lzma
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Files]
Source: "{#MySourceDir}\Cosmo.dll"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\{cm:UninstallProgram,Cosmo SDRuno Plugin}"; Filename: "{uninstallexe}"