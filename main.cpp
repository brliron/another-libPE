#include	<iostream>
#include	"Module.hpp"
#include	"ExportTable.hpp"
#include	"ImportTable.hpp"
#include	"ResourceTable.hpp"
#include	"RelocTable.hpp"
#include	"Text.hpp"

int		main(int ac, char** av)
{
  Module*	module;

  if (ac != 2)
    {
      std::cout << "Usage : " << av[0] << " file" << std::endl;
      return 0;
    }
  module = Module::alloc(av[1]); // TODO: use unicode if the lib is compiled in unicode.
  if (module == nullptr)
    return 1;

  std::cout << "CPU mode "		<< module->getProcessorMode()			<< std::endl;
  std::cout << "timestamp "		<< module->getCreationTimestamp()		<< std::endl;
  std::cout << "flags "			<< module->flags().print()			<< std::endl;
  std::cout << "linkerVersion "		<< module->linkerVersion().getFloat()		<< std::endl;
  std::cout << "entry point offset "	<< module->getEntryPoint().inFile<void*>()	<< std::endl;
  std::cout << "load address "		<< module->getLoadVa()				<< std::endl;
  std::cout << "section alignment "	<< module->getSectionAlignment()		<< std::endl;
  std::cout << "file alignment "	<< module->getFileAlignment()			<< std::endl;
  std::cout << "OS version "		<< module->OSVersion().getFloat()		<< std::endl;
  std::cout << "image version "		<< module->imageVersion().getFloat()		<< std::endl;
  std::cout << "subsystem version "	<< module->subsystemVersion().getFloat()	<< std::endl;
  std::cout << "subsystem "		<< module->printSubsystem()			<< std::endl;
  std::cout << "DLL flags "		<< module->dllFlags().print()			<< std::endl;
  std::cout << "getSizeOfStackReserve "	<< module->getSizeOfStackReserve()		<< std::endl;
  std::cout << "getSizeOfStackCommit "	<< module->getSizeOfStackCommit()		<< std::endl;
  std::cout << "getSizeOfHeapReserve "	<< module->getSizeOfHeapReserve()		<< std::endl;
  std::cout << "getSizeOfHeapCommit "	<< module->getSizeOfHeapCommit()		<< std::endl;
  std::cout << "loader flags "		<< module->loaderFlags().print()		<< std::endl;
  std::cout << std::endl;

  std::cout << "Sections: " << std::hex;
  for (auto& it : module->getSections())
    std::cout << it.first << "@" << it.second->getLoadRva() << " ";
  std::cout << std::dec << std::endl << std::endl;

  std::cout << "Nb of code sections " << module->getCodeSections().size() << std::endl;
  for (Section* section : module->getCodeSections())
    std::cout << "name " << section->getName() << " flags " << section->flags().print() << std::endl;
  /*
  for (const std::string& line : module->getCodeSections[0].disassemble())
    std::cout << line << std::endl;
  */
  std::cout << std::endl;

  ExportTable*	exports = module->getExportTable();
  if (exports)
    {
      std::cout << "Exported module name : " << exports->getExportedName() << std::endl;
      std::cout << "Export table size : " << exports->get().size() << std::endl;
      for (auto& it : exports->get())
	std::cout << "[" << it.ordinal << "] " << it.name << " " << it.offset << std::endl;
      std::cout << std::endl;
    }
  else
    std::cout << "No export table found" << std::endl << std::endl;

  ImportTable*	imports = module->getImportTable();
  if (imports)
    {
      std::cout << "Import table size : " << imports->get().size() << std::endl;
      for (auto& it : imports->get())
	std::cout << "[" << it.ordinal << "] " << it.name << "@" << it.dll << std::endl;
      std::cout << std::endl;
    }
  else
    std::cout << "No import table found" << std::endl << std::endl;

  ResourceTable*	resources = module->getResourceTable();
  if (resources)
    {
      std::cout << "Resource table size : " << resources->get().size() << std::endl;
      for (auto& it : resources->get())
	std::cout << it.getAsciiName() << " " << (void*)it.getData() << std::endl;
      std::cout << std::endl;
    }
  else
    std::cout << "No resource table found" << std::endl << std::endl;

  RelocTable*	relocs = module->getBaseRelocationTable();
  if (relocs)
    {
      std::cout << "Relocation table size : " << relocs->get().size() << std::endl;
      // for (auto& it : relocs->get())
	// std::cout << it << std::endl;
      std::cout << std::endl;
    }
  else
    std::cout << "No relocation table found" << std::endl << std::endl;
  // module->load();

  // std::cout << "The program is alive and will exit normally." << std::endl;

  delete module;
  return 0;
}
