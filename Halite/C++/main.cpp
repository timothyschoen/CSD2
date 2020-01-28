#include "./jack/jack_module.h"
#include "./halite.cpp"
#include <sstream>
#include <fstream>
#include <streambuf>


std::vector<std::string> savefile;
std::vector<std::string> object;



int main(int argc, char* argv[])
{

  int outputnode;
  double outamp = 0.8;

std::ifstream t("./precompile.hcl");
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
      for (int i = 0; i < seglist.size(); i++)
        std::cout << "'" << seglist[i] << "'";


      if(!seglist[0].compare("ground")) {
      net = new NetList(stoi(seglist[1]));
    }

      else if(!seglist[0].compare("resistor"))
      net->addComponent(new Resistor(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

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

      else if(!seglist[0].compare("output")) {
      outputnode = std::stoi(seglist[2]);
      outamp = std::stoi(seglist[1]);
      }

    }



net->buildSystem();
//net->printHeaders();

// set to about 44100
net->setTimeStep((float)1/4410);

//net->simulateTick();


double output;
bool realtime = false;

for (int i = 1; i < argc; i++)
{

        if (strcmp(argv[i], "-realtime") == 0) // This is your parameter name
        {
            realtime = true;    // The next value in the array is your value
            i++;    // Move to the next flag
        }
}



if (realtime == false) {

  int length = 8 * 44100;
  AudioFile<double> audioFile;

  audioFile.setAudioBufferSize (1, length);

  for (int i = 0; i<length; i++) {
    if(i%10000 == 0) {
      std::cout << i*100/length << "%" << std::endl;
    }
    net->simulateTick();
    // *
    output = net->getMNA().b[outputnode].lu * net->getMNA().nodes[outputnode].scale;
    audioFile.samples[0][i] = output*0.3;
      }

    //audioFile.samples[0][i] = output;

  audioFile.setBitDepth (24);
  audioFile.setSampleRate (44100);

  std::cout << "OK";
  std::string randomnumber = std::to_string(rand()%100);
  audioFile.save ("./output"+randomnumber+".wav", AudioFileFormat::Wave);
}

else {

  JackModule jack;
  jack.init("server 1");
  jack.onProcess = [&net, &output, &outamp, &outputnode](jack_default_audio_sample_t *inBuf,
     jack_default_audio_sample_t *outBuf, jack_nframes_t nframes)
  {
    //loop through frames, retrieve sample of sine per frame
    for(int i = 0; i < nframes; i++) {
      net->simulateTick();
      output = net->getMNA().b[outputnode].lu * net->getMNA().nodes[outputnode].scale;
      output = output*outamp;
      if (output > 1) output = 1;
      if (output < -1) output = -1;
      outBuf[i] = output;
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
