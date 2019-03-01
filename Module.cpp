#include	<stdio.h>
#include	<iostream>
#include	"Module.hpp"

#include	"Section.hpp"
#include	"ExportTable.hpp"
#include	"ImportTable.hpp"
#include	"ResourceTable.hpp"
#include	"RelocTable.hpp"
#include	"Text.hpp"

#ifndef		FILE_MAP_EXECUTE
# define	FILE_MAP_EXECUTE	0x20
#endif		/* !FILE_MAP_EXECUTE */
#ifndef		IMAGE_DIRECTORY_ENTRY_COPYRIGHT
# define	IMAGE_DIRECTORY_ENTRY_COPYRIGHT	7
#endif		/* !IMAGE_DIRECTORY_ENTRY_COPYRIGHT */

static const std::map<int, const char*>	flagsMap =
  {
    { Module::RELOCS_STRIPPED,		"RELOCS_STRIPPED" },
    { Module::EXECUTABLE,		"EXECUTABLE" },
    { Module::LINE_NUMS_STRIPPED,	"LINE_NUMS_STRIPPED" },
    { Module::LOCAL_SYMS_STRIPPED,	"LOCAL_SYMS_STRIPPED" },
    { Module::AGGRESIVE_WS_TRIM,	"AGGRESIVE_WS_TRIM" },
    { Module::LARGE_ADDRESS_AWARE,	"LARGE_ADDRESS_AWARE" },
    { Module::_16BIT_MACHINE,		"_16BIT_MACHINE" },
    { Module::BYTES_REVERSED_LO,	"BYTES_REVERSED_LO" },
    { Module::_32BIT_MACHINE,		"_32BIT_MACHINE" },
    { Module::DEBUG_STRIPPED,		"DEBUG_STRIPPED" },
    { Module::REMOVABLE_RUN_FROM_SWAP,	"REMOVABLE_RUN_FROM_SWAP" },
    { Module::NET_RUN_FROM_SWAP,	"NET_RUN_FROM_SWAP" },
    { Module::SYSTEM,			"SYSTEM" },
    { Module::DLL,			"DLL" },
    { Module::UP_SYSTEM_ONLY,		"UP_SYSTEM_ONLY" },
    { Module::BYTES_REVERSED_HI,	"BYTES_REVERSED_HI" },
  };
static const std::map<int, const char*>	subsystemsMap =
  {
    { Module::NATIVE,			"NATIVE" },
    { Module::WINDOWS_GUI,		"WINDOWS_GUI" },
    { Module::WINDOWS_CUI,		"WINDOWS_CUI" },
    { Module::OS2_CUI,			"OS2_CUI" },
    { Module::POSIX_CUI,		"POSIX_CUI" },
    { Module::NATIVE_WINDOWS,		"NATIVE_WINDOWS" },
    { Module::WINDOWS_CE_GUI,		"WINDOWS_CE_GUI" },
    { Module::EFI_APPLICATION,		"EFI_APPLICATION" },
    { Module::EFI_BOOT_SERVICE_DRIVER,	"EFI_BOOT_SERVICE_DRIVER" },
    { Module::EFI_RUNTIME_DRIVER,	"EFI_RUNTIME_DRIVER" },
    { Module::EFI_ROM,			"EFI_ROM" },
    { Module::XBOX,			"XBOX" },
    { Module::WINDOWS_BOOT_APPLICATION,	"WINDOWS_BOOT_APPLICATION" }
  };
static const std::map<int, const char*>	dllFlagsMap =
  {
    { Module::DLL_LOAD,			"DLL_LOAD" },
    { Module::THREAD_END,		"THREAD_END" },
    { Module::THREAD_STARTUP,		"THREAD_STARTUP" },
    { Module::DLL_UNLOAD,		"DLL_UNLOAD" },
    { Module::DYNAMIC_BASE,		"DYNAMIC_BASE" },
    { Module::FORCE_INTEGRITY,		"FORCE_INTEGRITY" },
    { Module::NX_COMPAT,		"NX_COMPAT" },
    { Module::NO_ISOLATION,		"NO_ISOLATION" },
    { Module::NO_SEH,			"NO_SEH" },
    { Module::NO_BIND,			"NO_BIND" },
    { Module::WDM_DRIVER,		"WDM_DRIVER" },
    { Module::TERMINAL_SERVER_AWARE,	"TERMINAL_SERVER_AWARE" }
  };
static const std::map<int, const char*>	loaderFlagsMap =
  {
    { Module::THROW_BREAKPOINT,	"THROW_BREAKPOINT" },
    { Module::ATTACH_DEBUGGER,	"ATTACH_DEBUGGER" }
  };


std::string	Module::_printFlags(unsigned int flags, const std::map<int, const char*>& flagsMap)
{
  std::string	str;

  for (auto& it : flagsMap)
    if (flags & it.first)
      {
	if (str.empty() != false)
	  str += " | ";
	str += it.second;
      }
  return str;
}




Module::Module()
  : AddrAwareObject(this), hFile(nullptr), hFileMapping(nullptr), data(nullptr)
{}

Module::~Module()
{
  if (hFile)
    {
      for (auto& it : this->sections)
	delete it.second;
      UnmapViewOfFile(this->data);
      CloseHandle(this->hFileMapping);
      CloseHandle(this->hFile);
    }
}

Module*		Module::alloc(LPCTSTR filename)
{
  Module*	module = new Module;
  if (module->load(filename) == false)
    {
      delete module;
      return nullptr;
    }
  return module;
}

bool	Module::load(LPCTSTR filename)
{
  this->hFile = CreateFile(filename, FILE_GENERIC_READ/* | FILE_GENERIC_WRITE | FILE_GENERIC_EXECUTE*/, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
			   FILE_ATTRIBUTE_NORMAL, nullptr);
  if (this->hFile == INVALID_HANDLE_VALUE)
    {
      printf("CreateFile failed.\n");
      return false;
    }
  this->hFileMapping = CreateFileMapping(this->hFile, nullptr, PAGE_WRITECOPY/*PAGE_EXECUTE_WRITECOPY*/, 0, 0, nullptr);
  this->data = (BYTE*)MapViewOfFile(this->hFileMapping, FILE_MAP_COPY/*FILE_MAP_EXECUTE*/, 0, 0, 0);

  IMAGE_DOS_HEADER*	dosHeader;
  dosHeader = (IMAGE_DOS_HEADER*)this->data;
  header = addr<Addr::FilePointer, IMAGE_NT_HEADERS32*>(dosHeader->e_lfanew);

  if (memcmp(&header->Signature, "PE\0\0", 4) != 0)
    {
      printf("Wrong signature.\n");
      return false;
    }

  opHeader = &header->OptionalHeader;
  if (opHeader->Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) // can also be IMAGE_NT_OPTIONAL_HDR64_MAGIC or IMAGE_ROM_OPTIONAL_HDR_MAGIC
    {
      printf("Wrong magic in optional header.\n");
      return false;
    }

  return true;
}

// Note : a lot of other processors exists, but I don't think I need to list them.
Module::ProcessorMode	Module::getProcessorMode() const
{
  if (this->header->FileHeader.Machine == IMAGE_FILE_MACHINE_I386)
    return Module::x86;
  else if (this->header->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64)
    return Module::amd64;
  else
    return Module::unknown;
}

void	Module::setProcessorMode(ProcessorMode mode)
{
  if (mode == x86)
    this->header->FileHeader.Machine = IMAGE_FILE_MACHINE_I386;
  else if (mode == amd64)
    this->header->FileHeader.Machine = IMAGE_FILE_MACHINE_AMD64;
  else
    throw std::invalid_argument("Unknown processor mode");
}

DWORD	Module::getCreationTimestamp() const
{
  return this->header->FileHeader.TimeDateStamp;
}

void	Module::setCreationTimestamp(DWORD timestamp)
{
  this->header->FileHeader.TimeDateStamp = timestamp;
}

Flags<WORD>	Module::flags()
{
  return Flags<WORD>(flagsMap, this->header->FileHeader.Characteristics);
}



Version<BYTE>	Module::linkerVersion()
{
  return Version<BYTE>(this->opHeader->MajorLinkerVersion, this->opHeader->MinorLinkerVersion);
}

void	Module::setEntryPointRva(DWORD offset)
{
  this->opHeader->AddressOfEntryPoint = offset;
}

DWORD	Module::getLoadVa() const
{
  return this->opHeader->ImageBase;
}

void	Module::setLoadVa(DWORD address)
{
  this->opHeader->ImageBase = address;
}

DWORD	Module::getSectionAlignment() const
{
  return this->opHeader->SectionAlignment;
}

DWORD	Module::getFileAlignment() const
{
  return this->opHeader->FileAlignment;
}

Version<WORD>	Module::OSVersion()
{
  return Version<WORD>(this->opHeader->MajorOperatingSystemVersion, this->opHeader->MinorOperatingSystemVersion);
}

Version<WORD>	Module::imageVersion()
{
  return Version<WORD>(this->opHeader->MajorImageVersion, this->opHeader->MinorImageVersion);
}

Version<WORD>	Module::subsystemVersion()
{
  return Version<WORD>(this->opHeader->MajorSubsystemVersion, this->opHeader->MinorSubsystemVersion);
}

unsigned int	Module::getSubsystem() const
{
  return this->opHeader->Subsystem;
}

std::string	Module::printSubsystem() const
{
  return subsystemsMap.at(this->getSubsystem());
}

void	Module::setSubsystem(unsigned int subsystem)
{
  this->opHeader->Subsystem = subsystem;
}

Flags<WORD>	Module::dllFlags()
{
  return Flags<WORD>(dllFlagsMap, this->opHeader->DllCharacteristics);
}

DWORD	Module::getSizeOfStackReserve() const
{
  return this->opHeader->SizeOfStackReserve;
}

void	Module::setSizeOfStackReserve(DWORD size)
{
  this->opHeader->SizeOfStackReserve = size;
}

DWORD	Module::getSizeOfStackCommit() const
{
  return this->opHeader->SizeOfStackCommit;
}

void	Module::setSizeOfStackCommit(DWORD size)
{
  this->opHeader->SizeOfStackCommit = size;
}

DWORD	Module::getSizeOfHeapReserve() const
{
  return this->opHeader->SizeOfHeapReserve;
}

void	Module::setSizeOfHeapReserve(DWORD size)
{
  this->opHeader->SizeOfHeapReserve = size;
}

DWORD	Module::getSizeOfHeapCommit() const
{
  return this->opHeader->SizeOfHeapCommit;
}

void	Module::setSizeOfHeapCommit(DWORD size)
{
  this->opHeader->SizeOfHeapCommit = size;
}

Flags<DWORD>	Module::loaderFlags()
{
  return Flags<DWORD>(loaderFlagsMap, this->opHeader->LoaderFlags);
}




Section*	Module::allocSectionFromDirectory(int dirIndex, IMAGE_SECTION_HEADER& header)
{
  switch (dirIndex)
    {
    case IMAGE_DIRECTORY_ENTRY_EXPORT:
      return new ExportTable(*this, header);
    case IMAGE_DIRECTORY_ENTRY_IMPORT:
      return new ImportTable(*this, header);
    case IMAGE_DIRECTORY_ENTRY_RESOURCE:
      return new ResourceTable(*this, header);
    case IMAGE_DIRECTORY_ENTRY_EXCEPTION:
    case IMAGE_DIRECTORY_ENTRY_SECURITY:
      return new Section(*this, header);
    case IMAGE_DIRECTORY_ENTRY_BASERELOC:
      return new RelocTable(*this, header);
    case IMAGE_DIRECTORY_ENTRY_COPYRIGHT:
    case IMAGE_DIRECTORY_ENTRY_GLOBALPTR:
    case IMAGE_DIRECTORY_ENTRY_TLS:
    case IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG:
    case IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT:
    case IMAGE_DIRECTORY_ENTRY_IAT:
      return new Section(*this, header);
    default:
      return new Section(*this, header);
    }
}

void			Module::buildSectionsTable()
{
  IMAGE_SECTION_HEADER*	sectionsTable = (IMAGE_SECTION_HEADER*)(this->header + 1);

  // Clean singleSections
  for (int j = 0; j < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; j++)
    this->singleSections[j] = nullptr;

  // Parse the sections table, store its content.
  for (int i = 0; i < this->header->FileHeader.NumberOfSections; i++)
    {
      Section*	section = nullptr;
      for (int j = 0; j < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; j++)
	if (this->opHeader->DataDirectory[j].VirtualAddress == sectionsTable[i].VirtualAddress)
	  {
	    section = this->allocSectionFromDirectory(j, sectionsTable[i]);
	    this->singleSections[j] = section;
	    break ;
	  }

      if (!section)
	section = new Section(*this, sectionsTable[i]); // TODO: free all these sections at the end.
      if (section->flags().get() & Section::TYPE_CODE)
	{
	  Section*	section_ = new Text(*section);
	  delete section;
	  section = section_;
	}
      this->sections[section->getName()] = section;
    }

  // Sometimes, IMAGE_DIRECTORY_ENTRY elements doesn't point to a section, but to a part of a section. We'll register them here.
  // TODO: find a way to differenciate partial structures from structures.
  // TODO: eventually, store a pointer to the singleSection entry in Section.
  for (int j = 0; j < IMAGE_NUMBEROF_DIRECTORY_ENTRIES; j++)
    {
      IMAGE_SECTION_HEADER*	header;
      int			i;

      if (this->singleSections[j] != nullptr || this->opHeader->DataDirectory[j].VirtualAddress == 0)
	continue ;
      for (i = 0; i < this->header->FileHeader.NumberOfSections; i++)
	if (this->opHeader->DataDirectory[j].VirtualAddress >= sectionsTable[i].VirtualAddress &&
	    this->opHeader->DataDirectory[j].VirtualAddress + this->opHeader->DataDirectory[j].Size < sectionsTable[i].VirtualAddress + sectionsTable[i].Misc.VirtualSize)
	  break ;
      if (i == this->header->FileHeader.NumberOfSections)
	{
	  std::cerr << "[WARNING] Could not find in which section is " << j /* TODO: replace with a string */ << std::endl;
	  continue ;
	}

      header = new IMAGE_SECTION_HEADER;
      memcpy(header, sectionsTable + i, sizeof(IMAGE_SECTION_HEADER));
      header->Misc.VirtualSize = this->opHeader->DataDirectory[j].Size;
      header->VirtualAddress = this->opHeader->DataDirectory[j].VirtualAddress;
      header->SizeOfRawData = this->opHeader->DataDirectory[j].Size;
      header->PointerToRawData = sectionsTable[i].PointerToRawData + (this->opHeader->DataDirectory[j].VirtualAddress - sectionsTable[i].VirtualAddress);
      this->singleSections[j] = this->allocSectionFromDirectory(j, *header); // TODO: free someday.
      for (auto& it : this->sections)
	if (it.second->getLoadRva() == sectionsTable[i].VirtualAddress)
	  {
	    this->singleSections[j]->setParentSection(it.second);
	    break ;
	  }
    }
}

const std::map<std::string, Section*>&	Module::getSections()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->sections;
}

ExportTable*	Module::getExportTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return dynamic_cast<ExportTable*>(this->singleSections[IMAGE_DIRECTORY_ENTRY_EXPORT]);
}

ImportTable*	Module::getImportTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return dynamic_cast<ImportTable*>(this->singleSections[IMAGE_DIRECTORY_ENTRY_IMPORT]);
}

ResourceTable*	Module::getResourceTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return dynamic_cast<ResourceTable*>(this->singleSections[IMAGE_DIRECTORY_ENTRY_RESOURCE]);
}

Section*	Module::getExceptionTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
}

Section*	Module::getCertificateTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_SECURITY];
}

RelocTable*	Module::getBaseRelocationTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return dynamic_cast<RelocTable*>(this->singleSections[IMAGE_DIRECTORY_ENTRY_BASERELOC]);
}

Section*	Module::getArchitectureSpecificData()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_COPYRIGHT];
}

Section*	Module::getGlobalPointerRVA()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_GLOBALPTR];
}

Section*	Module::getTlsTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_TLS];
}

Section*	Module::getLoadConfigTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG];
}

Section*	Module::getBoundImportTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT];
}

Section*	Module::getImportAddressTable()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();
  return this->singleSections[IMAGE_DIRECTORY_ENTRY_IAT];
}

const std::vector<Text*>	Module::getCodeSections()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();

  std::vector<Text*>		tab;

  for (auto& it : this->getSections())
    if (dynamic_cast<Text*>(it.second))
      tab.push_back(dynamic_cast<Text*>(it.second));
  return tab;
}

const std::vector<Section*>	Module::getDataSections()
{
  if (this->sections.size() == 0)
    this->buildSectionsTable();

  std::vector<Section*>		tab;

  for (auto& it : this->getSections())
    if (false) // TODO: make a test here.
      tab.push_back(dynamic_cast<Section*>(it.second)); // TODO: make a cast to the corresponding class
  return tab;
}
