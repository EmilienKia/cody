;--------------------------------
;--------------------------------
;--      Cody (win32)
;--------------------------------
;-- Cody nis 
;-- installer description file.
;--------------------------------
;--------------------------------

;--------------------------------
; Modern UI
;--------------------------------
!include "MUI2.nsh"


;--------------------------------
; Global definitions
;--------------------------------

; The name of the installer
Name "Cody"
OutFile "cody-0.1-win32.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Cody

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Cody" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Interface Settings
;--------------------------------

!define MUI_ABORTWARNING

;Show all languages, despite user's codepage
!define MUI_LANGDLL_ALLLANGUAGES
; Dont show component descriptions
!define MUI_COMPONENTSPAGE_NODESC

;--------------------------------
;Language Selection Dialog Settings

;Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU" 
!define MUI_LANGDLL_REGISTRY_KEY "Software\Cody" 
!define MUI_LANGDLL_REGISTRY_VALUENAME "Installer Language"


;--------------------------------
; Pages
;--------------------------------

!insertmacro MUI_PAGE_LICENSE "../../COPYING"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES



;--------------------------------
; Languages
;--------------------------------
 
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "French"

;--------------------------------
;Reserve Files
;--------------------------------
  
;If you are using solid compression, files that are required before
;the actual installation should be stored first in the data block,
;because this will make your installer start faster.

!insertmacro MUI_RESERVEFILE_LANGDLL


;--------------------------------
;Installer Sections
;--------------------------------

Section $(SecCodyApplication) SecCodyApplication

  SectionIn RO
    
  ; Cody executable (compiled in VC9 Static Unicode Release)
  SetOutPath $INSTDIR
  File "vc9_mswu\cody.exe"
  
  ; Cody default properties and graphic description resources  
  File "..\..\share\cody.conf"
  File "..\..\share\cody.xrc"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Cody "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Cody" "DisplayName" "Cody"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Cody" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Cody" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Cody" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd


Section

  ; Cody graphic resources (icons)
  SetOutPath $INSTDIR\icons\hicolor
  File "..\..\share\icons\hicolor\index.theme"
  
  SetOutPath $INSTDIR\icons\hicolor\32x32\actions
  File "..\..\share\icons\hicolor\32x32\actions\*.png"
  
SectionEnd


; Optional section (can be disabled by the user)
Section $(Sec_StartMenuShortcuts) ;Sec_StartMenuShortcuts

  CreateDirectory "$SMPROGRAMS\Cody"
  CreateShortCut "$SMPROGRAMS\Cody\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Cody\Cody.lnk" "$INSTDIR\cody.exe" "" "$INSTDIR\cody.exe" 0
  
SectionEnd

;--------------------------------
; Installer Functions
;--------------------------------

Function .onInit

  !insertmacro MUI_LANGDLL_DISPLAY

FunctionEnd

;--------------------------------
; Section descriptions
;--------------------------------

;Language strings

LangString SecCodyApplication ${LANG_ENGLISH} "Cody application (required)"
LangString SecCodyApplication ${LANG_FRENCH} "Application Cody (requis)"
LangString Sec_StartMenuShortcuts ${LANG_ENGLISH} "Start menu shortcuts"
LangString Sec_StartMenuShortcuts ${LANG_FRENCH} "Raccourcis du menu Démarrer"



/* *** Removed as description are not really usefull yet.
LangString DESC_SecCodyApplication ${LANG_ENGLISH} "TestNSIS application (required)"
LangString DESC_SecCodyApplication ${LANG_FRENCH}  "Application TestNSIS (requis)"
LangString DESC_Sec_StartMenuShortcuts ${LANG_ENGLISH} "Start menu shortcuts"
LangString DESC_Sec_StartMenuShortcuts ${LANG_FRENCH}  "Raccourcis du menu Démarrer"

;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCodyApplication} $(DESC_SecCodyApplication)
  !insertmacro MUI_DESCRIPTION_TEXT ${Sec_StartMenuShortcuts} $(DESC_Sec_StartMenuShortcuts)
!insertmacro MUI_FUNCTION_DESCRIPTION_END
*/



;--------------------------------
; Uninstaller
;--------------------------------

Section "Uninstall" Sec_Uninstall
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Cody"
  DeleteRegKey HKLM SOFTWARE\Cody

  ; Remove files and uninstaller
  RMDir /r $INSTDIR\icons
  Delete $INSTDIR\cody.xrc
  Delete $INSTDIR\cody.conf
  Delete $INSTDIR\cody.exe
  Delete $INSTDIR\uninstall.exe

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Cody\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Cody"
  RMDir "$INSTDIR"

SectionEnd


;--------------------------------
;Uninstaller Functions
;--------------------------------

Function un.onInit

  !insertmacro MUI_UNGETLANGUAGE
  
FunctionEnd


