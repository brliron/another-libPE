#ifndef RELOCTABLE_HPP_
# define RELOCTABLE_HPP_

# include	<vector>
# include	"Section.hpp"

class	RelocTable : public Section
{
private:
  std::vector<DWORD>	table;

public:
  RelocTable(Module& module, IMAGE_SECTION_HEADER& header);
  RelocTable(const RelocTable& src);
  ~RelocTable();

  const std::vector<DWORD>&	get();
  void				apply();
};

#endif /* !RELOCTABLE_HPP_ */
