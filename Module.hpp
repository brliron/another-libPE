#ifndef MODULE_HPP_
# define MODULE_HPP_

# include	<string>
# include	<map>
# include	<array>
# include	<vector>
# include	<windows.h>
# include	"Pointer.hpp"
# include	"Version.hpp"
# include	"Flags.hpp"

/*
** Rva, Fp and Va
**
** This library can use 3 kind of addresses: Rva, Fp and Va.
** Rva (Relative Virtual Address) is what is usually found in the PE header.
** It is the address where the data should be after beeing loaded into memory,
** relative to the image base.
**
** Fp (for File Pointer) is a pointer to the exe file's data.
** This program reads the entire exe file into a contiguous byte array in memory,
** and the pointer returned by the Fp functions points to some place in this array.
** These pointer can be accessed safely.
**
** Va (Virtual Address) is a pointer to the data once it have been loaded.
** The PE header uses it only for the preferred base address.
** The pointers returned by these functions are safe to use only if the module have
** been loaded for execution.
*/

class		Section;
class		ExportTable;
class		ImportTable;
class		ResourceTable;
class		RelocTable;
class		Text;

class		Module
{
private:
  HANDLE				hFile;
  HANDLE				hFileMapping;
  BYTE*					data;
  IMAGE_NT_HEADERS32*			header;
  IMAGE_OPTIONAL_HEADER32*		opHeader;
  std::array<Section*, 16>		singleSections;
  std::map<std::string, Section*>	sections;

  static std::string	_printFlags(unsigned int flags, const std::map<int, const char*>& flagsMap);
  void			buildSectionsTable();
  Section*		allocSectionFromDirectory(int dirIndex, IMAGE_SECTION_HEADER& header);

public:
  Module();
  ~Module();

  static Module*	alloc(LPCTSTR filename);
  bool			load(LPCTSTR filename);
  BYTE*			getData() const { return this->data; }

  enum	ProcessorMode
    {
      x86 = 32,
      amd64 = 64,
      unknown = -1
    };
  ProcessorMode	getProcessorMode() const;
  void		setProcessorMode(ProcessorMode mode);
  DWORD		getCreationTimestamp() const;
  void		setCreationTimestamp(DWORD timestamp);
  // IMAGE_FILE_HEADER->PointerToSymbolTable and NumberOfSymbols are around here.
  // TODO: make functions to handle these variables.
  enum	ModuleFlags
    {
      RELOCS_STRIPPED		= IMAGE_FILE_RELOCS_STRIPPED, // The file doesn't contain any relocation.
      EXECUTABLE		= IMAGE_FILE_EXECUTABLE_IMAGE, // TODO: see if this flags means "exe" or "exe or dll".
      LINE_NUMS_STRIPPED	= IMAGE_FILE_LINE_NUMS_STRIPPED,
      LOCAL_SYMS_STRIPPED	= IMAGE_FILE_LOCAL_SYMS_STRIPPED,
      AGGRESIVE_WS_TRIM		= IMAGE_FILE_AGGRESIVE_WS_TRIM,
      LARGE_ADDRESS_AWARE	= IMAGE_FILE_LARGE_ADDRESS_AWARE,
      _16BIT_MACHINE		= -1, // This define seems absent from my computer : IMAGE_FILE_16BIT_MACHINE, // Has an underscore because it can't begin with a number
      BYTES_REVERSED_LO		= IMAGE_FILE_BYTES_REVERSED_LO,
      _32BIT_MACHINE		= IMAGE_FILE_32BIT_MACHINE, // Has an underscore because it can't begin with a number
      DEBUG_STRIPPED		= IMAGE_FILE_DEBUG_STRIPPED,
      REMOVABLE_RUN_FROM_SWAP	= IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP,
      NET_RUN_FROM_SWAP		= IMAGE_FILE_NET_RUN_FROM_SWAP,
      SYSTEM			= IMAGE_FILE_SYSTEM,
      DLL			= IMAGE_FILE_DLL,  // .dll file.
      UP_SYSTEM_ONLY		= IMAGE_FILE_UP_SYSTEM_ONLY,
      BYTES_REVERSED_HI		= IMAGE_FILE_BYTES_REVERSED_HI
    };
  Flags<WORD>	flags();

  // Return the version of the linker that produced the file.
  // getFloatLinkerVersion returns a more human-readable number. This number should not be used in comparaisons
  // because, for example, 1.7 > 1.12.
  Version<BYTE>	linkerVersion();
  // Note: the SizeOfSomething fields are calculated from the sections fields. (TODO)
  // Return the entry point (the address where the execution begins).
  Pointer	getEntryPoint() { return Pointer::fromRva(this, this->opHeader->AddressOfEntryPoint); }
  void		setEntryPointRva(DWORD offset);
  // Note: ignoring BaseOfCode and BaseOfData (TODO)
  // Return the address where the Windows loader will try to map the executable.
  DWORD		getLoadVa() const;
  void		setLoadVa(DWORD address);
  // TODO: make a setter for these functions.
  DWORD		getSectionAlignment() const;
  DWORD		getFileAlignment() const;
  Version<WORD>	OSVersion(); // Seems to always be 1.0
  Version<WORD>	imageVersion(); // User-defined version number
  Version<WORD>	subsystemVersion(); // Windows version (6.1 means Windows 7, for example)
  // Note: ignoring Reserved1
  // Note: SizeOfImage and SizeOfHeaders will be calculated automatically. (TODO)
  // Note: CheckSum will be calculated aotomatically if needed. (TODO)
  enum	Subsystems
    {
      NATIVE			= IMAGE_SUBSYSTEM_NATIVE,
      WINDOWS_GUI		= IMAGE_SUBSYSTEM_WINDOWS_GUI, // Stantard program
      WINDOWS_CUI		= IMAGE_SUBSYSTEM_WINDOWS_CUI, // Standard program with a console
      OS2_CUI			= IMAGE_SUBSYSTEM_OS2_CUI,
      POSIX_CUI			= IMAGE_SUBSYSTEM_POSIX_CUI,
      NATIVE_WINDOWS		= IMAGE_SUBSYSTEM_NATIVE_WINDOWS,
      WINDOWS_CE_GUI		= IMAGE_SUBSYSTEM_WINDOWS_CE_GUI,
      EFI_APPLICATION		= IMAGE_SUBSYSTEM_EFI_APPLICATION,
      EFI_BOOT_SERVICE_DRIVER	= IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER,
      EFI_RUNTIME_DRIVER	= IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER,
      EFI_ROM			= IMAGE_SUBSYSTEM_EFI_ROM,
      XBOX			= IMAGE_SUBSYSTEM_XBOX,
      WINDOWS_BOOT_APPLICATION	= 16 // This define seems absent from my computer : IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION
    };
  unsigned int	getSubsystem() const;
  std::string	printSubsystem() const;
  void		setSubsystem(unsigned int subsystem);
  enum DllFlags
    {
      DLL_LOAD			= 1,
      THREAD_END		= 2,
      THREAD_STARTUP		= 4,
      DLL_UNLOAD		= 8,
      DYNAMIC_BASE		= IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE,
      FORCE_INTEGRITY		= IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY,
      NX_COMPAT			= IMAGE_DLLCHARACTERISTICS_NX_COMPAT,
      NO_ISOLATION		= IMAGE_DLLCHARACTERISTICS_NO_ISOLATION,
      NO_SEH			= IMAGE_DLLCHARACTERISTICS_NO_SEH,
      NO_BIND			= IMAGE_DLLCHARACTERISTICS_NO_BIND,
      WDM_DRIVER		= IMAGE_DLLCHARACTERISTICS_WDM_DRIVER,
      TERMINAL_SERVER_AWARE	= IMAGE_DLLCHARACTERISTICS_TERMINAL_SERVER_AWARE 
    };
  Flags<WORD>	dllFlags();
  DWORD		getSizeOfStackReserve() const;
  void		setSizeOfStackReserve(DWORD size);
  DWORD		getSizeOfStackCommit() const;
  void		setSizeOfStackCommit(DWORD size);
  DWORD		getSizeOfHeapReserve() const;
  void		setSizeOfHeapReserve(DWORD size);
  DWORD		getSizeOfHeapCommit() const;
  void		setSizeOfHeapCommit(DWORD size);
  enum LoaderFlags
    {
      THROW_BREAKPOINT	= 1, // Throw a breakpoint at the 1st instructon of the process.
      ATTACH_DEBUGGER	= 2  // Attach the default debugger to the process.
    }; // Does it work ? MSDN says this field is obsolete.
  Flags<DWORD>	loaderFlags();
  //  NumberOfRvaAndSizes and DataDirectory will be handled with specific functions. See below.

  const std::map<std::string, Section*>&	getSections();
  ExportTable*		getExportTable();
  ImportTable*		getImportTable();
  ResourceTable*	getResourceTable();
  Section*		getExceptionTable();
  Section*		getCertificateTable(); // IMAGE_DIRECTORY_ENTRY_SECURITY
  RelocTable*		getBaseRelocationTable();
  Section*		getArchitectureSpecificData(); // IMAGE_DIRECTORY_ENTRY_COPYRIGHT
  Section*		getGlobalPointerRVA(); // IMAGE_DIRECTORY_ENTRY_GLOBALPTR
  Section*		getTlsTable();
  Section*		getLoadConfigTable();
  Section*		getBoundImportTable();
  Section*		getImportAddressTable();
  // TODO: add DelayImportDescriptor and ClrHeader. 15th field is reserved.
  const std::vector<Text*>	getCodeSections();
  const std::vector<Section*>	getDataSections(); // Useless for now (returns an empty array)

  // This function loads the PE file in memory as an executable, loads its dependencies,
  // and execute it. It isn't complete - for now, it doesn't load the dependencies,
  // and the GetModuleHandle (and every call using this function) may fail (and
  // GetModuleHandle(NULL) WILL fail).
  // Also, this function is only available on 32-bits builds.
#ifdef WITH_EXECUTE
  void			execute();
#endif /* WITH_EXECUTE */
};

#endif /* !MODULE_HPP_ */
