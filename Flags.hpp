#ifndef FLAGS_HPP_
# define FLAGS_HPP_

# include	<map>
# include	<string>

template<typename T>
class	Flags
{
private:
  const std::map<int, const char*>&	map;
  T&	flags;

public:
  Flags(const std::map<int, const char*>& map, T& flags): map(map),	flags(flags)	{}
  Flags(const Flags<T>& src)				: map(src.map),	flags(src.flags){}
  T	get() const			{ return this->flags; }
  T	operator&(T other) const	{ return this->flags & other; }
  void	set(T flags)			{ this->flags = flags; }
  // TODO: add a bunch of operator overloadings.

  const std::map<int, const char*>&	getMap() const	{ return this->map; }

  const std::string	print() const
  {
    std::string	str;

    for (auto& it : this->map)
      if (this->flags & it.first)
	{
	  if (str.empty() == false)
	    str += " | ";
	  str += it.second;
	}
    return str;
  }
};

#endif /* !FLAGS_HPP_ */
