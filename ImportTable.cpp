#include	"Module.hpp"
#include	"ImportTable.hpp"

ImportTable::ImportTable(Module& module, IMAGE_SECTION_HEADER& header)
  : Section(module, header)
{}

ImportTable::ImportTable(const ImportTable& src)
  : Section(src)
{}

ImportTable::~ImportTable()
{}


const std::vector<ImportTable::Entry>&	ImportTable::get()
{
  if (this->table.size() != 0)
    return this->table;

  const IMAGE_IMPORT_DESCRIPTOR*	headers;

  headers = this->getData().inFile<const IMAGE_IMPORT_DESCRIPTOR*>();
  for (int i = 0; headers[i].FirstThunk != 0; i++)
    {
      const char*	name = Pointer::fromRva(this, headers[i].Name).inFile<const char*>();
      const DWORD*	funcsTable;
      if (headers[i].Characteristics != 0)
	funcsTable = Pointer::fromRva(this, headers[i].Characteristics).inFile<const DWORD*>();
      else
	funcsTable = Pointer::fromRva(this, headers[i].FirstThunk).inFile<const DWORD*>();
      for (int j = 0; funcsTable[j]; j++)
	{
	  const IMAGE_IMPORT_BY_NAME*	func;
	  WORD				ordinal;
	  const char*			funcName;

	  if (funcsTable[j] & 0x80000000) // Import by ordinal
	    {
	      ordinal = funcsTable[j] & 0xFFFF;
	      funcName = "";
	    }
	  else // Import by name
	    {
	      func = Pointer::fromRva(this, funcsTable[j]).inFile<const IMAGE_IMPORT_BY_NAME*>();
	      ordinal = func->Hint;
	      funcName = (const char*)&func->Name;
	    }

	  this->table.push_back(ImportTable::Entry({name, funcName, ordinal}));
	}
    }
  return this->table;
}

#ifdef WITH_EXECUTE
void	ImportTable::patch()
{
  const IMAGE_IMPORT_DESCRIPTOR*	headers;
  BYTE*					base = (BYTE*)this->module.getLoadVa();

  headers = (const IMAGE_IMPORT_DESCRIPTOR*)(base + this->getLoadRva());
  for (int i = 0; headers[i].FirstThunk != 0; i++) // For now, this block shares a lot of code with get(). We need to add some caching to get to avoid code duplication.
    {
      const char*	name = (const char*)(base + headers[i].Name);
      const DWORD*	funcsTable;
      DWORD*		importAddressTable;

      if (headers[i].Characteristics != 0)
	funcsTable = (DWORD*)(base + headers[i].Characteristics);
      else
	funcsTable = (DWORD*)(base + headers[i].FirstThunk);
      importAddressTable = (DWORD*)(base + headers[i].FirstThunk);

      if (GetModuleHandleA(name) == NULL)
	LoadLibraryA(name);
      for (int j = 0; funcsTable[j]; j++)
	{
	  const IMAGE_IMPORT_BY_NAME*	func;
	  WORD				ordinal;
	  const char*			funcName;

	  if (funcsTable[j] & 0x80000000) // Import by ordinal
	    {
	      ordinal = funcsTable[j] & 0xFFFF;
	      funcName = "";
	    }
	  else // Import by name
	    {
	      func = (const IMAGE_IMPORT_BY_NAME*)(base + funcsTable[j]);
	      ordinal = func->Hint;
	      funcName = (const char*)&func->Name;
	    }

	  DWORD	funcAddrInMemory;
	  if (funcName[0])
	    funcAddrInMemory = (DWORD)GetProcAddress(GetModuleHandleA(name), funcName);
	  else
	    funcAddrInMemory = (DWORD)GetProcAddress(GetModuleHandleA(name), (char*)(DWORD)ordinal);
	  importAddressTable[j] = funcAddrInMemory;
	}
    }
}
#endif /* WITH_EXECUTE */
