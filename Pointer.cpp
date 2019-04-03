#include "Module.hpp"
#include "Section.hpp"
#include "Pointer.hpp"

Pointer::Pointer(const Module *module, const Section *section, DWORD_PTR offset)
  : module(module), section(section), offset(offset)
{}



Pointer	Pointer::fromRva(const Module *module, DWORD rva)
{
  return Pointer(module, nullptr, rva);
}

#include <iostream>
Pointer	Pointer::fromRva(const Section *section, DWORD rva)
{
  const Module	*module = &section->getModule();

  // Rva is relative to the beginning of the module. Shift it to the beginning of the section
  rva = rva - section->getLoadRva();

  return Pointer(module, section, rva);
}

Pointer	Pointer::fromModule(const Module *module, DWORD offset)
{
  return Pointer(module, nullptr, offset);
}

Pointer	Pointer::fromSection(const Section *section, DWORD offset)
{
  return Pointer(&section->getModule(), section, offset);
}



void	*Pointer::_inFile() const
{
  if (this->section)
    return this->module->getData() + this->section->getPointerToRawData() + this->offset;
  else
    return this->module->getData() + this->offset;
}

#ifdef WITH_EXECUTABLE
void	*Pointer::_inMemory() const
{
    if (section)
      return (BYTE*)this->module->getLoadVa() + this->section->getLoadRva() + this->offset;
    else
      return (BYTE*)this->module->getLoadVa() + this->offset;
}
#endif /* WITH_EXECUTABLE */
