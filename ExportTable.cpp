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

  directory = (const IMAGE_EXPORT_DIRECTORY*)this->data();
  for (unsigned int i = 0; i < directory->NumberOfFunctions; i++)
    {
      DWORD		offset;
      const char*	name;
      DWORD		ordinal;

      offset = addr<Addr::FilePointer, DWORD*>(directory->AddressOfFunctions)[i];
      if (i < directory->NumberOfNames)
	name = addr<Addr::FilePointer, const char*>(addr<Addr::FilePointer, DWORD*>(directory->AddressOfNames)[i]);
      else
	name = NULL;
      ordinal = directory->Base + addr<Addr::FilePointer, WORD*>(directory->AddressOfNameOrdinals)[i];
      this->table.push_back(ExportTable::Entry(addr<Addr::FilePointer>(offset), offset, name, ordinal));
    }
  return this->table;
}
