#include <iostream>
#include <unistd.h>

class HelloWorld {
  public: HelloWorld(std::string greeting) {
    this->greet = greeting;
  }

  std::string greet;
  int printAgain(std::string name) {
    std::cout << this->greet << name << std::endl;
    return 0;
  }
};

int main() {
  int i = 0;
  HelloWorld hw1("Hey, ");
  std::string name;
  while(true) {
    std::cout << "\nWhat's Your Name? \n";
    std::cin >> name;
    hw1.printAgain(name);
    sleep(1);
  }
  return 0;
}
