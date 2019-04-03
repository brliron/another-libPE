#include	"Module.hpp"
#include	"RelocTable.hpp"

RelocTable::RelocTable(Module& module, IMAGE_SECTION_HEADER& header)
  : Section(module, header)
{}

RelocTable::RelocTable(const RelocTable& src)
  : Section(src)
{}

RelocTable::~RelocTable()
{}


const std::vector<DWORD>&	RelocTable::get()
{
  if (this->table.size() != 0)
    return this->table;

  const BYTE*	ptr = this->getData().inFile<const BYTE*>();
  const BYTE*	end = ptr + this->getSize();
  while (ptr < end)
    {
      const IMAGE_BASE_RELOCATION*	reloc = (const IMAGE_BASE_RELOCATION*)ptr;
      if (reloc->SizeOfBlock == 0)
        break ;
      const WORD*			addresses = (const WORD*)(ptr + sizeof(IMAGE_BASE_RELOCATION));
      ptr += reloc->SizeOfBlock;
      for (DWORD i = 0; i < (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2; i++)
	if (addresses[i] >> 12 == IMAGE_REL_BASED_HIGHLOW)
	  this->table.push_back(reloc->VirtualAddress + (addresses[i] & 0x0FFF));
    }
  return this->table;
}

void	RelocTable::apply()
{
  // I couldn't make ld generate a reloc table so I can't test with a tiny binary,
  // and a bigger binary will probably have way more reason to fail.
  // TODO: implement someday.
}
