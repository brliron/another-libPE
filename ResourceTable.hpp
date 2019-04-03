#ifndef RESOURCETABLE_HPP_
# define RESOURCETABLE_HPP_

# include	<vector>
# include	<string>
# include	"Section.hpp"

class	ResourceTable : public Section
{
public:
  class	Entry
  {
  private:
    const BYTE*		address;
    DWORD		size;
    DWORD		codePage; // 1252 for example.
    DWORD		reserved; // I wonder what is inside this.
    std::wstring	name;
    std::string		asciiName;
  public:
    DWORD		offset;

    Entry(LPCWSTR name, DWORD offset, const BYTE* address, DWORD size, DWORD codePage, DWORD reserved)
      : address(address), size(size), codePage(codePage), reserved(reserved), name(name), offset(offset)
    {
      for (WCHAR c : this->name)
	this->asciiName += (char)c;
    }
    const std::wstring&	getName() const      { return this->name; }
    const std::string&	getAsciiName() const { return this->asciiName; }
    const BYTE*		getData() const      { return this->address; }
    DWORD		getSize() const      { return this->size; }
  };

private:
  std::vector<Entry>	table;

  void			addEntry(const IMAGE_RESOURCE_DIRECTORY_ENTRY& entry, LPCWSTR dirName);
  void			addDirectory(const IMAGE_RESOURCE_DIRECTORY* dir, LPCWSTR name);

public:
  ResourceTable(Module& module, IMAGE_SECTION_HEADER& header);
  ResourceTable(const ResourceTable& src);
  ~ResourceTable();

  const std::vector<Entry>&	get();
};

#endif /* !RESOURCETABLE_HPP_ */
