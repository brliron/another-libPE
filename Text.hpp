#ifndef TEXT_HPP_
# define TEXT_HPP_

# include	<vector>
# include	"Section.hpp"

class	Text : public Section
{
private:
  std::vector<std::string>	code;

public:
  Text(const Section& src);
  ~Text();

#ifdef WITH_UDIS86
  const std::vector<std::string>&	disassemble();
#endif /* WITH_UDIS86 */
};

#endif /* TEXT_HPP_ */
