#ifndef POINTER_HPP_
# define POINTER_HPP_

# include	<windows.h>

class Module;
class Section;

class Pointer
{
private:
  const Module	*module;
  const Section	*section; // May be null
  // If section is not null, this offset is from the beginning of the section.
  // Else, it is from the beginning of the module.
  // The from* functions will do the conversions from a RVA or a file pointer.
  DWORD_PTR	offset;

  // In the .hpp file, we don't want to have code using the module or the section classes
  // (otherwise, Module.hpp includes Pointer.hpp and Pointer.hpp includes Module.hpp).
  // So we put the implementation of the template functions in the .cpp file.
  void		*_inFile() const;
#ifdef WITH_EXECUTABLE
  void		*_inMemory() const;
#endif /* WITH_EXECUTABLE */

  Pointer(const Module *module, const Section *section, DWORD_PTR offset);

public:
  // These 4 functions create a Pointer from some kind of adderss.
  // pos is relative to the beginning of the module
  static Pointer fromModule(const Module *module, DWORD offset);
  // pos is relative to the beginning of the section
  static Pointer fromSection(const Section *section, DWORD offset);
  // rva is a RVA. If this RVA points to inside a section, call the version
  // that takes a Section as parameter. Otherwise, call the one that takes
  // a Module as parameter.
  // TODO: remove the Section one, and find the section automatically.
  static Pointer fromRva(const Module *module, DWORD rva);
  static Pointer fromRva(const Section *section, DWORD rva);

  // Return a pointer to the pointed object in the file
  template<typename T>	T inFile() const
  {
    return static_cast<T>(this->_inFile());
  }
#ifdef WITH_EXECUTABLE
  // Return a pointer to the pointed object in the loaded image.
  // Works only if the module have been loaded in memory for execution.
  template<typename T>	T inMemory() const
  {
    return static_cast<T>(this->_inMemory());
  }
#endif /* WITH_EXECUTABLE */
};

#endif /* !POINTER_HPP_ */
