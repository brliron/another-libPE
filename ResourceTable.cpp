#ifdef __STRICT_ANSI__
# undef __STRICT_ANSI__
#endif /* __STRICT_ANSI__ */
#include	"Module.hpp"
#include	"ResourceTable.hpp"
#include	"swprintf.hpp"

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
  WCHAR		name[MAX_PATH]; // TODO: dynamic max path handling.
  const BYTE	*data = this->getData().inFile<const BYTE*>();

  if (entry.Name & 0x80000000)
    {
      const IMAGE_RESOURCE_DIR_STRING_U* nameStruct = (const IMAGE_RESOURCE_DIR_STRING_U*)(data + (entry.Name & 0x7FFFFFFF));
      wcscpy(name, dirName);
      wcscat(name, L"/");
      wcsncat(name, nameStruct->NameString, nameStruct->Length);
    }
  else
    SWPRINTF(name, MAX_PATH, L"%s/#%d", dirName, entry.Name);

  if (entry.OffsetToData & 0x80000000)
    {
      const IMAGE_RESOURCE_DIRECTORY *subdirectory = (const IMAGE_RESOURCE_DIRECTORY*)(data + (entry.OffsetToData & 0x7FFFFFFF));
      this->addDirectory(subdirectory, name);
    }
  else
    {
      const IMAGE_RESOURCE_DATA_ENTRY *resEntry = (const IMAGE_RESOURCE_DATA_ENTRY*)(data + entry.OffsetToData);
      const BYTE *resData = Pointer::fromSection(this, resEntry->OffsetToData).inFile<const BYTE*>();
      this->table.push_back(Entry(name, entry.OffsetToData, resData, resEntry->Size, resEntry->CodePage, resEntry->Reserved));
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

  this->addDirectory(this->getData().inFile<const IMAGE_RESOURCE_DIRECTORY*>(), L"");
  return this->table;
}
