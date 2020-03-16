#include "MNACell.h"

  void MNACell::clear()
  {
      g = 0;
      gtimed = 0;
  }

  void MNACell::initLU(double stepScale)
  {
      prelu = g + gtimed * stepScale;


  }

  // restore matrix state and update dynamic values
  void MNACell::updatePre(double stepScale)
  {
      lu = prelu;
      for(int i = 0; i < gdyn.size(); ++i)
      {
          lu += *(gdyn[i]);
      }
      for(int i = 0; i < gdyntimed.size(); ++i)
      {
          lu += *(gdyntimed[i]) * stepScale;
      }
  }
