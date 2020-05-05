#pragma once
#include "SynthClass.h"

class MoogFilter : public Synth
{
public:
MoogFilter();
~MoogFilter();
    
void init();
void calc();
float process(float x);
void setRes(float r);

private:
float cutoff;
float res;
float fs;
float y1,y2,y3,y4;
float oldx;
float oldy1,oldy2,oldy3;
float x;
float r;
float p;
float k;
};
