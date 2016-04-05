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

  const std::vector<std::string>&	disassemble();
};

#endif /* TEXT_HPP_ */
