#include	"Module.hpp"
#include	"ExportTable.hpp"

ExportTable::ExportTable(Module& module, IMAGE_SECTION_HEADER& header)
  : Section(module, header)
{}

ExportTable::ExportTable(const ExportTable& src)
  : Section(src)
{}

ExportTable::~ExportTable()
{}


const std::vector<ExportTable::Entry>&	ExportTable::get()
{
  if (this->table.size() != 0)
    return this->table;

  const IMAGE_EXPORT_DIRECTORY*	directory;

  directory = (const IMAGE_EXPORT_DIRECTORY*)this->fp();
  for (unsigned int i = 0; i < directory->NumberOfFunctions; i++)
    {
      DWORD		offset;
      const char*	name;
      DWORD		ordinal;

      offset = rvaToFp<DWORD*>(directory->AddressOfFunctions)[i];
      if (i < directory->NumberOfNames)
	name = rvaToFp<const char*>(rvaToFp<DWORD*>(directory->AddressOfNames)[i]);
      else
	name = NULL;
      ordinal = directory->Base + rvaToFp<WORD*>(directory->AddressOfNameOrdinals)[i];
      this->table.push_back(ExportTable::Entry(rvaToFp(offset), offset, name, ordinal));
    }
  return this->table;
}
