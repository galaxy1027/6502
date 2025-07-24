#include "cpu6502.hpp"
#include <string>

class nes
{
  private:
    cpu6502 *cpu;

  public:
    nes();
    ~nes();
    int LoadRom(std::string path);
};
