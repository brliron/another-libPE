#include	"AddrAwareObject.hpp"
#include	"Module.hpp"
#include	"Section.hpp"

AddrAwareObject::AddrAwareObject(Module* module, Section* section)
  : module(module), section(section)
{}

AddrAwareObject::AddrAwareObject(AddrAwareObject* ref, Section* section)
  : module(ref->module), section(section)
{}


template<>	void*	AddrAwareObject::_addr<Addr::FilePointer>(DWORD rva)	{ return (void*)this->_addr_FilePointer(rva); }
template<>	void*	AddrAwareObject::_addr<Addr::RVA>(DWORD rva)		{ return (void*)rva; }
template<>	void*	AddrAwareObject::_addr<Addr::Load>(DWORD rva)		{ return this->_addr_Load(rva); }


BYTE*	AddrAwareObject::_addr_FilePointer(DWORD rva)
{
  if (this->section)
    {
      // TODO: ensure the RVA lies inside this section. If it doesn't, look for the correct section.
      rva = rva - this->section->getLoadRva() + this->section->getPointerToRawData();
      // The RVA is relative to the module base.
      // * Substract the section load address. The rva becomes relative to the beginning of the section.
      // * Add the section offset in the file. The rva becomes relative to the beginning of the file.
    }
  return this->module->getData() + rva;
}

void*	AddrAwareObject::_addr_Load(DWORD rva)
{
  BYTE*	addr;

  addr = (BYTE*)this->module->getLoadVa();
  if (this->section)
    {
      // TODO: ensure the RVA lies inside this section. If it doesn't, look for the correct section.
      addr += section->getLoadRva();
    }
  return addr + rva;
}
