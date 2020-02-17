#include "./jack/jack_module.h"
#include "./halite.cpp"
#include "./digitalComponents.cpp"
#include "./analogComponents.cpp"
#include "./genComponents.cpp"

#include <sstream>
#include <fstream>
#include <streambuf>
#include <tclap/CmdLine.h>

std::vector<std::string> savefile;
std::vector<std::string> object;


int main(int argc, char* argv[])
{

  bool realtime;
  int outputsamplerate;
  int enginesamplerate;
  int bitdepth;
  std::string inputpath;
  std::string outputpath;
  std::string outputformat;

  try {
    TCLAP::CmdLine cmd("Halite", ' ', "0.9");
    TCLAP::ValueArg<std::string> inputFile("i","input", "choose input file", false, "./precompile.ncl", "path");
    TCLAP::ValueArg<std::string> outputFile("o","output", "choose output file", false, "./output.wav", "path");
    TCLAP::ValueArg<int> osr("s","samplerate", "set sample rate for output", false, 44100, "integer");
    TCLAP::ValueArg<int> esr("t","interval", "set sample rate for engine", false, 44100, "integer");
    TCLAP::ValueArg<int> bd("b","bitdepht", "set bit depth for output", false, 24, "integer");
    TCLAP::ValueArg<std::string> format("f","format", "output file format", false, ".WAV", "string");
    TCLAP::SwitchArg rtswitch("r", "realtime", "enable realtime playback");

    cmd.add(inputFile);
    cmd.add(outputFile);
    cmd.add(esr);
    cmd.add(osr);
    cmd.add(bd);
    cmd.add(rtswitch);
    cmd.add(format);
    cmd.parse(argc, argv);

    outputformat = format.getValue();
    bitdepth = bd.getValue();
    outputsamplerate = osr.getValue();
    enginesamplerate = esr.getValue();
    inputpath = inputFile.getValue();
	  outputpath = outputFile.getValue();
    realtime = rtswitch.getValue();

  }
  catch (TCLAP::ArgException &e)  // catch any exceptions
	{ std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl; }


  double outamp = 0.8;

std::ifstream t(inputpath);
std::string str; // = "output, 0.3, 2 \n input, ./samples/sample-44k.wav, 0.2, 1, 0 \n resistor, 200, 1, 2 \n ";


t.seekg(0, std::ios::end);
str.reserve(t.tellg());
t.seekg(0, std::ios::beg);


str.assign((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());

  std::stringstream ss(str);
  std::string obj;
  NetList * net;


    while(std::getline(ss,obj,'\n')){
      std::vector<std::string> seglist;
      std::string segment;
      std::stringstream inputcode(obj);


      while(std::getline(inputcode, segment, ','))
      {
         segment.erase(0,1);
         seglist.push_back(segment);
      }


      if(!seglist[0].compare("ground")) {
      net = new NetList(stoi(seglist[1]));
    }

// Digital components

    else if(!seglist[0].compare("cycle-"))
    net->addComponent(new digitalCycle(std::stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

    else if(!seglist[0].compare("sig-"))
    net->addComponent(new digitalSignal(std::stof(seglist[1]), std::stoi(seglist[2])));

    else if(!seglist[0].compare("dac"))
    net->addComponent(new digitalAnalogConverter(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

    else if(!seglist[0].compare("adc"))
    net->addComponent(new analogDigitalConverter(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

    else if(!seglist[0].compare("output-"))
    net->addComponent(new digitalOutput(std::stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

    else if(!seglist[0].compare("input-"))
    net->addComponent(new digitalInput(seglist[1], std::stof(seglist[2]), std::stoi(seglist[2])));

    else if(!seglist[0].compare("stinput-"))
    net->addComponent(new stereoDigitalInput(seglist[1], std::stof(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4])));

 // Analog components

      else if(!seglist[0].compare("resistor"))
      net->addComponent(new Resistor(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

      else if(!seglist[0].compare("varres"))
      net->addComponent(new VariableResistor(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4])));
      else if(!seglist[0].compare("capacitor"))
      net->addComponent(new Capacitor(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

      else if(!seglist[0].compare("voltage"))
      net->addComponent(new Voltage(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

      else if(!seglist[0].compare("input"))
      net->addComponent(new InputSample(seglist[1], std::stof(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4])));

      else if(!seglist[0].compare("diode"))
      net->addComponent(new Diode(std::stoi(seglist[1]), std::stoi(seglist[2])));

      else if(!seglist[0].compare("bjt"))
      net->addComponent(new BJT(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4])));

      else if(!seglist[0].compare("cycle"))
      net->addComponent(new InputSignal(std::stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

      else if(!seglist[0].compare("opa"))
      net->addComponent(new OPA(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4]), std::stoi(seglist[5])));

      else if(!seglist[0].compare("potentiometer")) {
        net->addComponent(new Potentiometer(stof(seglist[1]), stoi(seglist[2]), stoi(seglist[3]), stoi(seglist[4]), stoi(seglist[5])));
      }

      else if(!seglist[0].compare("print"))
      net->addComponent(new Printer(std::stoi(seglist[1]), std::stoi(seglist[2])));

      else if(!seglist[0].compare("delay"))
      net->addComponent(new AnalogDelay(std::stoi(seglist[1]), std::stof(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4]), std::stoi(seglist[5])));

      else if(!seglist[0].compare("digidelay"))
      net->addComponent(new digitalDelay(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));


      else if(!seglist[0].compare("phasor"))
      net->addComponent(new genLoader("phasor", {std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])}));


      else if(!seglist[0].compare("+-")  ||
              !seglist[0].compare("--")  ||
              !seglist[0].compare("!--") ||
              !seglist[0].compare("*-")  ||
              !seglist[0].compare("/-")  ||
              !seglist[0].compare("!/-") ||
              !seglist[0].compare(">-")  ||
              !seglist[0].compare("<-")  ||
              !seglist[0].compare(">=-") ||
              !seglist[0].compare("<=-") ||
              !seglist[0].compare("==-") ||
              !seglist[0].compare("!=-") ||
              !seglist[0].compare("%-")  ||
              !seglist[0].compare("!%-"))
        net->addComponent(new digitalArithmetic(seglist[0], 0, std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]))); // Add optional arguments in JS!!!!


      else if(!seglist[0].compare("probe")) {
        net->addComponent(new Probe(stof(seglist[1]), std::stoi(seglist[2])));
      outamp = std::stof(seglist[3]);
      }

    }



net->buildSystem();

// sets amount of time that is simulated between ticks (1/samplerate)
std::cout << enginesamplerate << std::endl;
std::cout << (double)1/enginesamplerate << std::endl;
net->setTimeStep((double)1/enginesamplerate);
int startOffset = -3 * enginesamplerate;

net->simulateTick();
net->simulateTick();
net->simulateTick();


double* output;

if (realtime == false) {

  int length = 8 * 44100;
  AudioFile<double> audioFile;
  audioFile.setAudioBufferSize (2, length);

  for (int i = startOffset; i<length; i++) {
    if(i%20000 == 0) {
      std::cout << (i-startOffset) *100/(length-startOffset) << "%" << std::endl;
    }
    net->simulateTick();
    // *
    output = net->getOutput();
    //std::cout << *output << '\n';

    if (i > 0)
      audioFile.samples[0][i] = *(output)*outamp;
      audioFile.samples[1][i] = *(output+1)*outamp;
      }

    //audioFile.samples[0][i] = output;

  audioFile.setBitDepth (bitdepth);
  audioFile.setSampleRate (outputsamplerate);

  std::cout << "OK";
  if (!outputformat.compare(".AIF") || !outputformat.compare(".AIFF") || !outputformat.compare(".aif") || !outputformat.compare(".aiff"))
    audioFile.save(outputpath, AudioFileFormat::Aiff);
  else
    audioFile.save(outputpath);
}

else {
  for (size_t i = startOffset; i > 0 ; i++) {
    net->simulateTick(); // filters out DC pops at the start
  }

  JackModule jack;
  jack.init("halite");

  jack.onProcess = [&net, &output, &outamp](jack_default_audio_sample_t *inBuf,
     jack_default_audio_sample_t *outBufR, jack_default_audio_sample_t *outBufL, jack_nframes_t nframes)
  {
    //loop through frames, retrieve sample of sine per frame
    for(int i = 0; i < nframes; i++) {
      net->simulateTick();
      output = net->getOutput();

      /*
      // clipping
      if (output > 1) output = 1;
      if (output < -1) output = -1;

      */
      outBufL[i] = *(output)*outamp;
      outBufR[i] = *(output+1)*outamp;
    }
    return 0;
  };
  jack.autoConnect();


  //printf("\t%+.4e", output);

  while(true){
    usleep(20000);
  }
}

};
