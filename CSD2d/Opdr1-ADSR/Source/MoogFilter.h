#pragma once
#include "Generator.h"

class MoogFilter : public Generator
{
public:
MoogFilter();
~MoogFilter();
    
void init();
void calc();
double getSample(float input) override;
void setRes(float r);
float res;
    
    
private:
float cutoff;

float fs;
float y1,y2,y3,y4;
float oldx;
float oldy1,oldy2,oldy3;
float x;
float r;
float p;
float k;
};
