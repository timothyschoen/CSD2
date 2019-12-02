#include <unistd.h>

namespace Inst
{
class Instrument {
  public: Instrument(std::string greeting) {
    std::cout << greeting << std::endl;
  }
  int play(std::string sound) {
    std::cout << sound << std::endl;

    return 0;
  }
};
}
