#ifndef VERSION_HPP_
# define VERSION_HPP_

template<typename T>
class	Version
{
public:
  T&	_major;
  T&	_minor;

  Version(T& major, T& minor)
    : _major(major), _minor(minor)
  {}

  float	getFloat() const
  {
    float	major = this->_major;
    float	minor = this->_minor;

    while (minor > 1)
      minor /= 10;
    return major + minor;
  }
};

#endif /* !VERSION_HPP_ */
