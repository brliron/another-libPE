#ifndef VERSION_HPP_
# define VERSION_HPP_

template<typename T>
class	Version
{
public:
  T&	major;
  T&	minor;

  Version(T& major, T& minor)
    : major(major), minor(minor)
  {}

  float	getFloat() const
  {
    float	major = this->major;
    float	minor = this->minor;

    while (minor > 1)
      minor /= 10;
    return major + minor;
  }
};

#endif /* !VERSION_HPP_ */
