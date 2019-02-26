#ifndef SECTION_HPP_
# define SECTION_HPP_

# include	<string>
# include	<windows.h>
# include	"Module.hpp"
# include	"Flags.hpp"

class	Section : public AddrAwareObject
{
protected:
  Module&		module;
  IMAGE_SECTION_HEADER&	header;
  const Section*	parent;
  std::string		name;

public:
  Section(Module& module, IMAGE_SECTION_HEADER& header);
  Section(const Section& src);
  virtual ~Section();

  // Some tables are a part of a section, and are allocated as a complete Section class (the import address table, for example). In that case, use this function to tell which is the section containing this one.
  void			setParentSection(const Section* parent);

  DWORD	getPointerToRawData() const { return this->header.PointerToRawData; }

  const std::string&	getName() const;
  void			setName(const std::string& name);
  DWORD			getSize() const;
  DWORD			getLoadRva() const;
  void			setLoadRva(DWORD rva);
  DWORD			getRoundedSize() const; // TODO: explain the difference with getSize.
  BYTE*			data();
  // PointerToRelocations, PointerToLinenumbers, NumberOfRelocations and NumberOfLinenumbers are useless for exe files.
  enum	SectionFlags
    {
      _IMAGE_SCN_TYPE_NO_PAD = IMAGE_SCN_TYPE_NO_PAD,		// Unknown
      TYPE_CODE		= IMAGE_SCN_CNT_CODE,			// Code section (.text for example).
      TYPE_DATA		= IMAGE_SCN_CNT_INITIALIZED_DATA,	// Data section.
      TYPE_BSS		= IMAGE_SCN_CNT_UNINITIALIZED_DATA,	// Uninitialized data section (.bss for example).
      _IMAGE_SCN_LNK_OTHER = IMAGE_SCN_LNK_OTHER,		// Unknown
      LINK_INFO		= IMAGE_SCN_LNK_INFO,			// Comments or some other type of information. Can be for example .drectve
      LINK_REMOVE	= IMAGE_SCN_LNK_REMOVE,			// Shouldn't exists in the executable file.
      _IMAGE_SCN_LNK_COMDAT = IMAGE_SCN_LNK_COMDAT,					// Unknown
      _IMAGE_SCN_MEM_FARDATA = IMAGE_SCN_MEM_FARDATA,					// Unknown
      _IMAGE_SCN_MEM_PURGEABLE = IMAGE_SCN_MEM_PURGEABLE,					// Unknown
      _IMAGE_SCN_MEM_16BIT = IMAGE_SCN_MEM_16BIT,					// Unknown
      _IMAGE_SCN_MEM_LOCKED = IMAGE_SCN_MEM_LOCKED,					// Unknown
      _IMAGE_SCN_MEM_PRELOAD = IMAGE_SCN_MEM_PRELOAD,					// Unknown
      _IMAGE_SCN_ALIGN_1BYTES = IMAGE_SCN_ALIGN_1BYTES,					// Unknown
      _IMAGE_SCN_ALIGN_2BYTES = IMAGE_SCN_ALIGN_2BYTES,					// Unknown
      _IMAGE_SCN_ALIGN_4BYTES = IMAGE_SCN_ALIGN_4BYTES,					// Unknown
      _IMAGE_SCN_ALIGN_8BYTES = IMAGE_SCN_ALIGN_8BYTES,					// Unknown
      _IMAGE_SCN_ALIGN_16BYTES = IMAGE_SCN_ALIGN_16BYTES,					// Unknown
      _IMAGE_SCN_ALIGN_32BYTES = IMAGE_SCN_ALIGN_32BYTES,					// Unknown
      _IMAGE_SCN_ALIGN_64BYTES = IMAGE_SCN_ALIGN_64BYTES,					// Unknown
      _IMAGE_SCN_LNK_NRELOC_OVFL = IMAGE_SCN_LNK_NRELOC_OVFL,				// Unknown
      MEM_DISCARDABLE	= IMAGE_SCN_MEM_DISCARDABLE,		// Useless after linking (.reloc for example)
      _IMAGE_SCN_MEM_NOT_CACHED = IMAGE_SCN_MEM_NOT_CACHED,					// Unknown
      _IMAGE_SCN_MEM_NOT_PAGED = IMAGE_SCN_MEM_NOT_PAGED,					// Unknown
      MEM_SHARED	= IMAGE_SCN_MEM_SHARED,			// Can be shared among processes (useful for DLLs).
      MEM_EXEC		= IMAGE_SCN_MEM_EXECUTE,		// Executable.
      MEM_READ		= IMAGE_SCN_MEM_READ,			// Readable.
      MEM_WRITE		= IMAGE_SCN_MEM_WRITE			// Writable.
    };
  Flags<DWORD>	flags();

  void		load();
};

#endif /* !SECTION_HPP_ */
