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


std::string ExportTable::getExportedName()
{
  const IMAGE_EXPORT_DIRECTORY*	directory = this->getData();
  const char *name = Pointer::fromRva(this, directory->Name);
  return name;
}

const std::vector<ExportTable::Entry>&	ExportTable::get()
{
  if (this->table.size() != 0)
    return this->table;

  const IMAGE_EXPORT_DIRECTORY*	directory = this->getData();
  DWORD *AddressOfFunctions        = Pointer::fromRva(this, directory->AddressOfFunctions);
  DWORD *AddressOfNames            = Pointer::fromRva(this, directory->AddressOfNames);
  WORD  *AddressOfNameOrdinals     = Pointer::fromRva(this, directory->AddressOfNameOrdinals);

  for (unsigned int i = 0; i < directory->NumberOfFunctions; i++)
    {
      DWORD		offset;
      const char*	name;
      DWORD		ordinal;

      offset = AddressOfFunctions[i];
      if (i < directory->NumberOfNames)
	name = Pointer::fromRva(this, AddressOfNames[i]);
      else
	name = NULL;
      ordinal = directory->Base + AddressOfNameOrdinals[i];
      this->table.push_back(
	ExportTable::Entry(
	  Pointer::fromRva(this, offset),
	  offset, name, ordinal
	)
      );
    }
  return this->table;
}
