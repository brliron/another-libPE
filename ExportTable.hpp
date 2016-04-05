#ifndef EXPORTTABLE_HPP_
# define EXPORTTABLE_HPP_

# include	<vector>
# include	"Section.hpp"

class	ExportTable : public Section
{
public:
  struct	Entry
  {
  private:
    BYTE*	address;
  public:
    Entry(BYTE* address, DWORD offset, const char* name, DWORD ordinal)
      : address(address), offset(offset), name(name), ordinal(ordinal)
    {}
    DWORD	offset;
    const char*	name;
    DWORD	ordinal;
  };

private:
  std::vector<Entry>	table;

public:
  ExportTable(Module& module, IMAGE_SECTION_HEADER& header);
  ExportTable(const ExportTable& src);
  ~ExportTable();

  const std::vector<Entry>&	get();
};

#endif /* !EXPORTTABLE_HPP_ */
