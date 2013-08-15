#if !defined __DOS2UNIX_H

#include <iostream>
#include <fstream>

using namespace std;

class dos2unixbuf : public streambuf {
public:
#ifdef WIN32
	typedef ios::open_mode openmode;
#else
	typedef ios::openmode openmode;
#endif

  dos2unixbuf(const char *filename, openmode mode);
  virtual ~dos2unixbuf();

  void close();

protected:
  virtual int overflow(int c = EOF);
  virtual int underflow();
  virtual int uflow();
  virtual int pbackfail(int c = EOF);
  virtual int sync();
  
private:
  filebuf inner;
  char *buffer;
};

class idos2unixstream
  : public istream {
public:
  idos2unixstream(const char *filename)
    : istream(new dos2unixbuf(filename, ios::in))
    {
    }

  virtual ~idos2unixstream();

  void close();
};

#endif // __DOS2UNIX_H
