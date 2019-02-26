#ifdef WITH_UDIS86
# include	"udis86.h"
#endif /* WITH_UDIS86 */
#include	"Module.hpp"
#include	"Text.hpp"

Text::Text(const Section& src)
  : Section(src)
{}

Text::~Text()
{}


#ifdef WITH_UDIS86
const std::vector<std::string>&	Text::disassemble()
{
  if (this->code.size() > 0)
    return this->code;

  ud_t	ud;

  ud_init(&ud);
  if (this->module.getProcessorMode() == Module::x86)
    ud_set_mode(&ud, 32);
  else if (this->module.getProcessorMode() == Module::amd64)
    ud_set_mode(&ud, 64);
  ud_set_input_buffer(&ud, this->data(), this->getSize());
  ud_set_syntax(&ud, UD_SYN_INTEL);

  while (ud_disassemble(&ud))
    this->code.push_back(ud_insn_asm(&ud));

  return this->code;
}
#endif /* WITH_UDIS86 */
