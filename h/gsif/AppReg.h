#ifndef _APPREG_H_
#define _APPREG_H_

// Registry Trees
#define GigaManufacturer           _T("TASCAM")
#define RegGigaRoot                _T("Software\\TASCAM")
#define RegGigaSettings            _T("Software\\TASCAM\\GigaStudio\\Settings")
#define NMSRegGSSettings           _T("Software\\TASCAM\\GigaStudio\\Settings")
#define RegGigaStartup             _T("Software\\TASCAM\\GigaStudio\\Startup")
#define RegGigaAdvanced            _T("Software\\TASCAM\\GigaStudio\\Advanced")
#define RegGigaPlugins             _T("Software\\TASCAM\\GigaStudio\\Plugins")
#define RegGigaAutoRegistration    _T("Software\\TASCAM\\GigaStudio\\AutoRegistration")


//----------------------------
// Generic Registry Values
//----------------------------
#define NMS_USE_SYSTEM_COLORS      _T("UseSystemColors")
#define NMS_SCSI_TIMEOUT           _T("CdRom Timeout")

//----------------------------
// GigaStudio Registry Values
//----------------------------

// AutoRegistration values
#define NMSRegId        				    _T("id")
#define NMSRegControl				        _T("control")
#define NMSRegImage				            _T("image")

// Registry Keys
#define NMSRegGSSettings				   _T("Software\\TASCAM\\GigaStudio\\Settings")
#define NMSUserGSSettings				   _T("Software\\TASCAM\\GigaStudio\\User")
#define NMSRegEWSettings				   _T("Software\\Conexant\\EndlessWave\\Settings")

// Mode to startup giga in
#define NMSRegMinusHardware					_T("NoHardware")
#define NMSRegMinusComp						_T("NoComp")
#define NMSRegMinusDB						_T("NoDB")

// Mode to Advanced property settings
#define NMSRegDebugMode					_T("DebugMode")
#define NMSRegOverrideMixerLimit		_T("MaxInputs")

// Plugins configuration (under HKEY_LOCAL_MACHINE)
#define VSTPATH_CONTROL_DISABLE       0x1
#define VSTPATH_CONTROL_DEFAULT       0x2

#define NMSRegVSTSearchPath		  	    _T("VSTSearchPath")
#define NMSRegVSTSearchControl			_T("VSTSearchControl")
#define NMSRegOverrideMixerLimit		_T("MaxInputs")


// State of GStudio
#define NMSRegGracefulExit             _T("GracefulExit")
#define NMSRegGSFirstTimeFlag			   _T("FirstTimeFlag")
#define NMSRegCurrentPane				   _T("CurrentPane")
#define NMSRegCurrentTab				   _T("CurrentTab")
#define NMSSRegUpdateTimer				   _T("UpdateTimer")

// Main App Position/Visiblity
#define NMSRegAppLeft                	_T("WndAppLeft")
#define NMSRegAppRight                	_T("WndAppRight")
#define NMSRegAppTop                	_T("WndAppTop")
#define NMSRegAppBottom                _T("WndAppBot")
#define NMSRegAppShow                	_T("WndAppShow")

// Other GUI States
#define NMSRegGUI_QSNDLISTCOLUMNS      _T("GuiListColQSnd")
#define NMSRegGUI_QSNDLISTSTYLE        _T("GuiListLayoutQSnd")
#define NMSRegGUI_QSNDLISTARRANGE      _T("GuiListArrangeQSnd")

// Use System Colors
#define NMSRegUseSystemColors			   _T("UseSystemColors")

// Reset Levels
#define NMSREGResetLevels				   _T("ResetLevels")

// Paths to applications
#define NMSRegGSInstallDirectory		   _T("InstallDirectory")
#define NMSRegGSPatchEditor				_T("PatchEditor")
#define NMSRegGSSequencer				   _T("Sequencer")
#define NMSRegGSWaveEditor				   _T("WaveEditor")

// EndlessWave settings
#define NMSSRegEWDither					   _T("Dither")
#define NMSSRegEWDitherType				_T("DitherType")
#define NMSSRegEWPolyphony				   _T("Polyphony")
#define NMSSRegEWTransitionVoices		_T("TransitionVoices")
#define NMSSRegEWMasterAttenuation		_T("MasterAttenuation")
#define NMSSRegEWInstallState			   _T("InstallState")

// Audio Capture Settings
#define NMSSRegAudioCaptureView			_T("AudioCaptureView")
#define NMSSRegAudioCaptureTop			_T("AudioCaptureViewTop")
#define NMSSRegAudioCaptureLeft			_T("AudioCaptureViewLeft")
#define NMSSRegAudioCaptureBitDepth		_T("AudioCaptureBitDepth")
#define NMSSRegAudioCaptureSyncMidi		_T("AudioCaptureSyncToMidiStart")
#define NMSSRegAudioCaptureBufferSize	_T("AudioCaptureBufferSize")
#define NMSSRegAudioCaptureAutoNumber	_T("AudioCaptureAutoNumber")
#define NMSSRegAudioCaptureDir			_T("AudioCaptureAutoDir")

// Warning/Information messages
#define NMSSSupressDitherWarning		   _T("SupressDitherWarning")

// Registration Wizard
#define NMSSRegUserName					   _T("UserName")		// baggage from Elijah
#define NMSSRegUserFName				   _T("UserFirstName")
#define NMSSRegUserLName				   _T("UserLastName")
#define NMSSRegCompanyName				   _T("CompanyName")
#define NMSSRegEmail					      _T("Email")
#define NMSSRegCDKey					      _T("UserSerial") // CDKey
#define NMSSRegUserDLC					   _T("CLD")        // Download Code/Key
#define NMSSRegUserNum					   _T("UserNum")    // Registration Code
#define NMSSRegUserKey					   _T("UserKey")    // Registration Key
#define NMSSRegCurrPage					   _T("CurrPage")   // Last RegistrationWizard page visited by user
#define NMSSRegCity						   _T("City")
#define NMSSRegState					   _T("State")
#define NMSSRegCountry					   _T("Country")
#define NMSSRegStreetAddress			   _T("Street")
#define NMSSRegZipCode					   _T("ZipCode")
#define NMSSRegMemory					   _T("Memory")
#define NMSSRegOS						   _T("OS")
#define NMSSRegSequencer				   _T("Sequencer")
#define NMSSRegSoundCard				   _T("SoundCard")
#define NMSSRegSystemProcessor             _T("Processor")
#define NMSSRegWaveEdit					   _T("WaveEdit")
#define NMSSRegWherePurchased			   _T("Purchased")

// Demo Timeout
#define NMSTIMEOUTGUID                 _T("CLSID\\{A4433373-0CE1-4540-A2FF-A402D32FA8BB}")      // full path=HKEY_CLASSES_ROOT\CLSID\{a4433373-0ce1-4540-a2ff-a402d32fa8bb}
#define NMSTimeOutLastSysHours         _T("Win32")          // Deceptive Name
#define NMSTimeOutTimeOutHours         _T("Windows")        // Deceptive Name

// Distributed Wave
#define NMSRegWaveStartPoint			   _T("WaveStartPoint")
#define DBRegAudtionMode				   _T("DBAuditionMode")
#define DBRegStackMode				       _T("DBStackMode")

// QuickSound Search
#define NMSRegDBSection                 _T("Software\\TASCAM\\DataBase")
#define NMSRegLastSearch				_T("LastSearch")
#define NMSRegStopLastSearch			_T("StopLastSearch")
#define NMSReg_QSND_AllowNetScan        _T("AllowNetScan")         // if true, enabled net drives are scanned and dirs/files are put in DBase
#define DBRegMajorVerKey                _T("DBMajorVer")
#define DBRegMinorVerKey                _T("DBMinorVer")
#define DBRegBuildVerKey                _T("DBBuildVer")
#define DBEnableLogging                 _T("DBLogging")            // when true, output goes to GStudio/db/error.log
#define DBRegShowHits			        _T("DBShowHits")
#define DBRegHitsVerbose		        _T("DBHitsVerbose")
#define DBRegHitDetail	   	            _T("DBRegHitDetail")       // Level of Hit Detail (see HITRESULT_XXXXX in DefineDB2.h)
#define DBRegCompleteSearches	        _T("DBCompleteSearches")   // AutoComplete of user's search token
#define DBRegRecursiveSearch	        _T("DBRecursiveSearch")    // vs. only get results for the local directory
#define DBRegShowInstr                  _T("DBRegShowInstr")       // vs. only show the file (no instruments are displayed, files cannot be expanded)
#define DBRegShowOnlyMatchInstr	        _T("DBShowOnlyMatchInstr") // vs. show all of a file's instruments
#define DBRegShowExpanded	   	        _T("DBShowExpanded")       // automatically expands to show a file's instruments


// QuickSound/FileSpy Settings
//Spy Defines, same as in spy.c
#define NMSRegDBSection					   _T("Software\\TASCAM\\DataBase")
#define NMSRegSPYScopeOn				   _T("ScopeOn")
#define NMSRegSPYScopeName				   _T("ScopeName")
#define NMSRegSPYScopeTypes				_T("ScopeTypes")
#define NMSRegSPYScopeHighWater			_T("ScopeHighWater")
#define NMSRegDBFilterTypes				_T("DBFilterTypes")
#define NMSRegDBFilterDirMode			   _T("DBFilterDirMode")
#define    NMSRegDBFilterDirModeAll		_T("All")
#define    NMSRegDBFilterDirModeIncl	_T("Include")
#define    NMSRegDBFilterDirModeExcl	_T("Exclude")
#define    DIRMODE_ALL					(1)
#define    DIRMODE_INCL					(2)
#define    DIRMODE_EXCL					(4)
#define NMSRegDBFilterDirIncl			   _T("DBFilterDirIncl")
#define NMSRegDBFilterDirExcl			   _T("DBFilterDirExcl")
#define NMSRegDBCreateStatsOn			   _T("DBCreateStats")
#define NMSRegDBCreateSpyStatsOn		   _T("DBCreateSpyStats")
#define NMSRegDBCanRefresh				   _T("DBCanRefresh")
#define NMSRegDBPromptOff				   _T("DBPromptOff")
#define NMSRegDBDrvTop					   _T("DrvTopo")
#define NMSRegDBPersistNodePath        _T("DBPersistNodePath")
#define NMSRegDBPersistNodeID          _T("DBPersistNodeID")
#define NMSRegSPYScopeDrives			_T("ScopeFilterDrives")



#define NMSBackgroundColor				RGB(102, 137, 143)
//#define NMSBackgroundColor				RGB(58, 110, 165)
//#define NMSBackgroundColor				RGB(90, 141, 197)
#define NMSTextColor					RGB(255, 255, 255)


//
// Library Management
//
#define GSRegLibraries					_T("Software\\TASCAM\\Libraries")

#define GSRegGMLibrary					_T("GMLibrary")
#define GMIniFileName					_T("GMLibrary.ini")
#define GMIniGeneralSection				_T("General")
#define GMIniUpdateMapperSection		_T("UpdateMapper")
#define GMIniGMFileEntry				_T("GMGig")
#define GMIniDrumsFileEntry				_T("DrumsGig")


//---------------------------------------------------------------
// NMSHWReg and HWReg definitions (some from HKEY_LOCAL_MACHINE)
//---------------------------------------------------------------
#include "HWReg.h"

//----------------------------
// AConverter
//----------------------------
#define NMSRegAConvSettings        _T("Software\\TASCAM\\GigaStudio\\Settings")
#define ACONV_SCSI_TIMEOUT         NMS_SCSI_TIMEOUT
#define ACONV_USE_SYSTEM_COLORS    NMS_USE_SYSTEM_COLORS
#define ACONV_CONV_DIR             _T("ConversionDirectory")
#define ACONV_CONV_PREFIX          _T("ConversionFilePrefix")

//----------------------------
// SConverter
//----------------------------
#define NMSRegSConvSettings          _T("Software\\TASCAM\\GigaStudio\\Settings")
#define INSTALLDIR_GSTUDIO           NMSRegGSInstallDirectory
#define NMSSRegSConvMergeLRID        _T("SConvCfg_MergeLR")
#define NMSSRegSConvDblClkPrgOpID    _T("SConvCfg_DblClkOptPrg")
#define NMSSRegSConvDblClkVolOpID    _T("SConvCfg_DblClkOptVol")
#define NMSSRegSConvDblClkBchOpID    _T("SConvCfg_DblClkOptBch")
#define NMSSRegSConvDblClkApplyTo    _T("SConvCfg_DblClkApply")
#define NMSSRegSConvAutoClnTmpDirID  _T("SConvCfg_AutoClnTmpDir")
#define NMSSRegSConvTmpDirPath       _T("SConvCfg_TmpDirPath")
#define NMSSRegSConvFilterOn         _T("SConvCfg_FilterOn")


// Debug Registry Keys
#define NMSRegCompanyDebug			_T("Software\\TASCAM\\Debug")
#define NMSRegDebugLevel			_T("DebugLevel")
#define NMSRegVCTrace				_T("VCTrace")


#define DIAG_KEYCUR              "Software\\TASCAM\\GigaStudio\\Diag_BootDiagsCurr"
#define DIAG_KEYPRV              "Software\\TASCAM\\GigaStudio\\Diag_BootDiagsPrev"
#define DIAG_KEYDBG              "Software\\TASCAM\\GigaStudio\\Diag_BootDiagsDBG"
#define DIAG_INDEX_VAL          "Diag_Index"
#define DIAG_INDEX_DFT          (0)
#define DIAG_STEP_VALPREFIX     "Diag_Step"
#define DIAG_RESET_VAL          "Diag_Reset"
#define DIAG_RESET_DFT          (1)
#define DIAG_RESET_OFF          (0)

// Giga Component definitions.
#ifndef NMSGigaStudioSoftwareKey
#define NMSGigaStudioSoftwareKey	_T("Software\\TASCAM\\GigaStudio\\Components")
#define NMSEndlessWaveSoftwareKey	_T("Software\\TASCAM\\EndlessWave\\Components")
#define NMSEndlessWaveEffectsKey	_T("Software\\TASCAM\\EndlessWave\\Components\\Effects")
#define NMSComponentWEditorKey		_T("\\Wave Editors")
#define NMSComponentIEditorKey		_T("\\Instrument Editors")
#define NMSComponentConvertersKey	_T("\\Sample Converters")
#define NMSComponentSequencerKey	_T("\\Sequencers")
#define NMSComponentEffectsKey		_T("\\Effects")
#define NMSComponentSoftSynthKey	_T("\\SoftSynth")
#endif // NMSGigaStudioSoftwareKey

// GStudio Configuration Page Options
#define NMSConfigSkipDongle            _T("__NoDongle__")  // __IndicatesRegistryReadInDebugModeOnly__

#endif //_APPREG_H_
