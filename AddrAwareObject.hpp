#ifndef ADDRAWAREOBJECT_HPP_
# define ADDRAWAREOBJECT_HPP_

# include	<windows.h>

class	Module;
class	Section;

enum class	Addr
{
  FilePointer,
  RVA,
#ifdef WITH_EXECUTE
  Load
#endif /* WITH_EXECUTE */
};

class	AddrAwareObject
{
private:
  Module*	module;
  Section*	section;

  BYTE*		_addr_FilePointer(DWORD rva);
#ifdef WITH_EXECUTE
  void*		_addr_Load(DWORD rva);
#endif /* WITH_EXECUTE */

  template<Addr A>	void*	_addr(DWORD rva);

public:
  AddrAwareObject(Module* module, Section* section = nullptr);
  AddrAwareObject(AddrAwareObject* ref, Section* section = nullptr);

  template<Addr A, typename T = BYTE*>	T	addr(DWORD rva) {return (T)this->_addr<A>(rva); }
};

template<>	void*	AddrAwareObject::_addr<Addr::FilePointer>(DWORD rva);
template<>	void*	AddrAwareObject::_addr<Addr::RVA>(DWORD rva);
#ifdef WITH_EXECUTE
template<>	void*	AddrAwareObject::_addr<Addr::Load>(DWORD rva);
#endif /* WITH_EXECUTE */

#endif /* !ADDRAWAREOBJECT_HPP_ */
