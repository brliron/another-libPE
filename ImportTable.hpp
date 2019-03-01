#ifndef IMPORTTABLE_HPP_
# define IMPORTTABLE_HPP_

# include	<vector>
# include	"Section.hpp"

class	ImportTable : public Section
{
public:
  // TODO: handle forwarding.
  struct	Entry
  {
    const char*	dll;
    const char*	name;
    WORD	ordinal;
  };

private:
  std::vector<Entry>	table;

public:
  ImportTable(Module& module, IMAGE_SECTION_HEADER& header);
  ImportTable(const ImportTable& src);
  ~ImportTable();

  const std::vector<Entry>&	get();
#ifdef WITH_EXECUTE
  void				patch();
#endif /* WITH_EXECUTE */
};

#endif /* !IMPORTTABLE_HPP_ */
