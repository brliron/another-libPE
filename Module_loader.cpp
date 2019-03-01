#include	<iostream>
#include	"Module.hpp"
#include	"Section.hpp"
#include	"ImportTable.hpp"

#ifdef WITH_EXECUTE
void	Module::execute()
{
  // Some debug info
  MEMORY_BASIC_INFORMATION	mbi;
  VirtualQuery(this->getLoadVa(), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
  std::cout << "Base: " << mbi.BaseAddress << "; AllocationBase: " << mbi.AllocationBase << "; RegionSize: " << mbi.RegionSize << "; State: " << mbi.State << std::endl;

  // Get highest load address
  DWORD	higher_rva = 0;
  for (auto it : this->getSections())
    if ((DWORD)it.second->getLoadRva() + it.second->getSize() > higher_rva)
      higher_rva = (DWORD)it.second->getLoadRva() + it.second->getSize();

  // for (int i = 0; i < 20; i++)
    // std::cout << VirtualAlloc(NULL, higher_rva, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE) << std::endl;

  // Reserve range
  if (VirtualAlloc((void*)this->getLoadVa(), higher_rva, MEM_RESERVE, PAGE_READWRITE) == NULL)
    {
      std::cerr << "Reserve of range (" << this->getLoadVa() << ", size " << higher_rva << ") failed; error " << GetLastError() << std::endl;
      return ;
    }

  // Allocate base
  BYTE*	base;
  base = (BYTE*)VirtualAlloc((void*)this->getLoadVa(), this->opHeader->SizeOfHeaders, MEM_COMMIT, PAGE_READWRITE);
  if (base == NULL)
    {
      std::cerr << "Alloc of base (" << this->getLoadVa() << ") failed; error " << GetLastError() << std::endl;
      return ;
    }
  CopyMemory(base, this->data, this->opHeader->SizeOfHeaders);
  // TODO: set to read-only.

  // Allocate sections
  for (auto it : this->getSections())
    it.second->load();

  // Patch IAT
  this->getImportTable()->patch();

  // Run entry point
  void	(*entry)();
  entry = (void(*)())(base + this->getEntryPoint<Addr::RVA, DWORD>());
  entry();
}
#endif /* WITH_EXECUTE */
