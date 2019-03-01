#include	<iostream>
#include	"Section.hpp"
#include	"Module.hpp"

static const std::map<int, const char*>	flagsMap =
  {
    { Section::_IMAGE_SCN_TYPE_NO_PAD,		"IMAGE_SCN_TYPE_NO_PAD" },
    { Section::TYPE_CODE,			"TYPE_CODE" },
    { Section::TYPE_DATA,			"TYPE_DATA" },
    { Section::TYPE_BSS,			"TYPE_BSS" },
    { Section::_IMAGE_SCN_LNK_OTHER,		"IMAGE_SCN_LNK_OTHER" },
    { Section::LINK_INFO,			"LINK_INFO" },
    { Section::LINK_REMOVE,			"LINK_REMOVE" },
    { Section::_IMAGE_SCN_LNK_COMDAT,		"IMAGE_SCN_LNK_COMDAT" },
    { Section::_IMAGE_SCN_MEM_FARDATA,		"IMAGE_SCN_MEM_FARDATA" },
    { Section::_IMAGE_SCN_MEM_PURGEABLE,	"IMAGE_SCN_MEM_PURGEABLE" },
    { Section::_IMAGE_SCN_MEM_16BIT,		"IMAGE_SCN_MEM_16BIT" },
    { Section::_IMAGE_SCN_MEM_LOCKED,		"IMAGE_SCN_MEM_LOCKED" },
    { Section::_IMAGE_SCN_MEM_PRELOAD,		"IMAGE_SCN_MEM_PRELOAD" },
    { Section::_IMAGE_SCN_ALIGN_1BYTES,		"IMAGE_SCN_ALIGN_1BYTES" },
    { Section::_IMAGE_SCN_ALIGN_2BYTES,		"IMAGE_SCN_ALIGN_2BYTES" },
    { Section::_IMAGE_SCN_ALIGN_4BYTES,		"IMAGE_SCN_ALIGN_4BYTES" },
    { Section::_IMAGE_SCN_ALIGN_8BYTES,		"IMAGE_SCN_ALIGN_8BYTES" },
    { Section::_IMAGE_SCN_ALIGN_16BYTES,	"IMAGE_SCN_ALIGN_16BYTES" },
    { Section::_IMAGE_SCN_ALIGN_32BYTES,	"IMAGE_SCN_ALIGN_32BYTES" },
    { Section::_IMAGE_SCN_ALIGN_64BYTES,	"IMAGE_SCN_ALIGN_64BYTES" },
    { Section::_IMAGE_SCN_LNK_NRELOC_OVFL,	"IMAGE_SCN_LNK_NRELOC_OVFL" },
    { Section::MEM_DISCARDABLE,			"MEM_DISCARDABLE" },
    { Section::_IMAGE_SCN_MEM_NOT_CACHED,	"IMAGE_SCN_MEM_NOT_CACHED" },
    { Section::_IMAGE_SCN_MEM_NOT_PAGED,	"IMAGE_SCN_MEM_NOT_PAGED" },
    { Section::MEM_SHARED,			"MEM_SHARED" },
    { Section::MEM_EXEC,			"MEM_EXEC" },
    { Section::MEM_READ,			"MEM_READ" },
    { Section::MEM_WRITE,			"MEM_WRITE" }
  };


Section::Section(Module& module, IMAGE_SECTION_HEADER& header)
  : AddrAwareObject(&module, this), module(module), header(header)
{
  char	name[9];

  name[8] = '\0';
  memcpy(name, header.Name, 8);
  this->name = name;
}

Section::Section(const Section& src)
  : AddrAwareObject(&module, this), module(src.module), header(src.header), name(src.name)
{}

Section::~Section()
{}

void	Section::setParentSection(const Section* parent)
{
  this->parent = parent;
}


const std::string&	Section::getName() const
{
  return this->name;
}

void	Section::setName(const std::string& name)
{
  this->name = name;
  strncpy((char*)this->header.Name, this->name.c_str(), 8);
}

DWORD	Section::getSize() const
{
  return this->header.Misc.VirtualSize;
}

DWORD	Section::getLoadRva() const
{
  return this->header.VirtualAddress;
}

void	Section::setLoadRva(DWORD rva)
{
  this->header.VirtualAddress = rva;
}

DWORD	Section::getRoundedSize() const
{
  return this->header.SizeOfRawData;
}

BYTE*	Section::data()
{
  return this->addr<Addr::FilePointer>(this->getLoadRva());
}

Flags<DWORD>	Section::flags()
{
  return Flags<DWORD>(flagsMap, this->header. Characteristics);
}


#ifdef WITH_EXECUTE
void	Section::load()
{
  BYTE*	base;
  DWORD	protection;
  DWORD	dummy;

  // base = (BYTE*)VirtualAlloc((BYTE*)this->module.getLoadVa() + (DWORD)this->getLoadRva(), this->getSize(), MEM_COMMIT, PAGE_READWRITE);
  base = (BYTE*)VirtualAlloc(this->addr<Addr::Load>(0), this->getSize(), MEM_COMMIT, PAGE_READWRITE);
  if (base == NULL)
    {
      std::cerr << "Alloc of section " << this->getName() << " (" << std::hex << this->getLoadRva() << ") failed; error " << GetLastError() << std::endl;
      return ;
    }
  CopyMemory(base, this->data(), this->getSize());

  protection = 0;
  if (this->flags() & MEM_WRITE)
    protection = PAGE_READWRITE;
  else if (this->flags() & MEM_READ)
    protection = PAGE_READONLY;
  else
    protection = PAGE_NOACCESS;
  if (this->flags() & MEM_EXEC)
    protection <<= 4;
  std::cout << std::hex << "[LOG] section " << this->getName() << "; Protection: " << protection << std::endl;
  VirtualProtect(base, this->getSize(), PAGE_READWRITE, &dummy);
}
#endif /* WITH_EXECUTE */
