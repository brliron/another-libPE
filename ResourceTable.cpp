#ifdef __STRICT_ANSI__
# undef __STRICT_ANSI__
#endif /* __STRICT_ANSI__ */
#include	"Module.hpp"
#include	"ResourceTable.hpp"

ResourceTable::ResourceTable(Module& module, IMAGE_SECTION_HEADER& header)
  : Section(module, header)
{}

ResourceTable::ResourceTable(const ResourceTable& src)
  : Section(src)
{}

ResourceTable::~ResourceTable()
{}


void	ResourceTable::addEntry(const IMAGE_RESOURCE_DIRECTORY_ENTRY& entry, LPCWSTR dirName)
{
  WCHAR	name[MAX_PATH]; // TODO: dynamic max path handling.

  if (entry.Name & 0x80000000)
    {
      const IMAGE_RESOURCE_DIR_STRING_U* nameStruct = (const IMAGE_RESOURCE_DIR_STRING_U*)(this->fp() + (entry.Name & 0x7FFFFFFF));
      wcscpy(name, dirName);
      wcscat(name, L"/");
      wcsncat(name, nameStruct->NameString, nameStruct->Length);
    }
  else
    swprintf(name, L"%s/#%d", dirName, entry.Name);
  if (entry.OffsetToData & 0x80000000)
    this->addDirectory((const IMAGE_RESOURCE_DIRECTORY*)(this->fp() + (entry.OffsetToData & 0x7FFFFFFF)), name);
  else
    {
      const IMAGE_RESOURCE_DATA_ENTRY*	resEntry = (const IMAGE_RESOURCE_DATA_ENTRY*)(this->fp() + entry.OffsetToData);
      this->table.push_back(Entry(name, entry.OffsetToData, rvaToFp<>(resEntry->OffsetToData), resEntry->Size, resEntry->CodePage, resEntry->Reserved));
    }
}

void					ResourceTable::addDirectory(const IMAGE_RESOURCE_DIRECTORY* dir, LPCWSTR name)
{
  const IMAGE_RESOURCE_DIRECTORY_ENTRY*	entries;

  entries = (const IMAGE_RESOURCE_DIRECTORY_ENTRY*)(dir + 1);
  for (int i = 0; i < dir->NumberOfNamedEntries + dir->NumberOfIdEntries; i++)
    this->addEntry(entries[i], name);
}

const std::vector<ResourceTable::Entry>&	ResourceTable::get()
{
  if (this->table.size() != 0)
    return this->table;

  this->addDirectory((const IMAGE_RESOURCE_DIRECTORY*)this->fp(), L"");
  return this->table;
}
