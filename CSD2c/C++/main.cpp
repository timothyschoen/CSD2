#include <lo/lo.h>
#include <lo/lo_cpp.h>

#include <sstream>
#include <fstream>
#include <streambuf>
#include <unistd.h>
#include <tclap/CmdLine.h>

// Audio I/O libraries
#include "rtlibs/RtAudio.h"
#include "rtlibs/RtMidi.h"
#include "AudioFile.h"

//#include "./gen/genlib.h"
//#include "./genComponents.cpp"


double* inbuffer; // ugly, fix this

#include "MNASystem.h"
#include "NetList.h"

#include "MNASolver.h"

#include "IComponent.h"
#include "Component.h"
#include "digitalComponents.cpp"

#include "analogComponents.h"



std::vector<std::string> savefile;
std::vector<std::string> object;
NetList * net;
double outamp = 0.2;
unsigned int bufferFrames, fs = 44100, offset = 0;

RtMidiIn *midiin = new RtMidiIn();
std::vector<unsigned char> message;


std::vector<double> oscBuffer(20, 0.5);





 /*
  * Add a method handler for "/example,i" using a C++11 lambda to
  * keep it succinct.  We capture a reference to the `received'
  * count and modify it atomatically.
  *
  * You can also pass in a normal function, or a callable function
  * object.
  *
  * Note: If the lambda doesn't specify a return value, the default
  *       is `return 0', meaning "this message has been handled,
  *       don't continue calling the method chain."  If this is not
  *       the desired behaviour, add `return 1' to your method
  *       handlers.
  */



void errorCallback( RtAudioError::Type type, const std::string &errorText )
{
    std::cout << "in errorCallback" << std::endl;
    if ( type == RtAudioError::WARNING )
        std::cerr << '\n' << errorText << "\n\n";
    else if ( type != RtAudioError::WARNING )
        throw( RtAudioError( errorText, type ) );
}

unsigned int channs;
RtAudio::StreamOptions options;
unsigned int frameCounter = 0;
bool checkCount = false;
unsigned int nFrames = 0;
unsigned int bufferBytes;



const unsigned int callbackReturnValue = 1;

int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double /*streamTime*/, RtAudioStreamStatus status, void *data )
{
    unsigned int i;
    extern unsigned int channs;
    double *buffer = (double *) outputBuffer;

    //inbuffer = (double *) inputBuffer;

    double *output;
    midiin->getMessage( &message );

    for ( i=0; i<nBufferFrames; i++ )
    {
        inbuffer[i] = ((double *)inputBuffer)[i];
        net->simulateTick();
        output = net->getAudioOutput();
        *buffer++ = output[0]*outamp;
        *buffer++ = output[1]*outamp;

    }


    frameCounter += nBufferFrames;
    if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
    return 0;
}




int main(int argc, char* argv[])
{

    bool realtime;
    int outputsamplerate;
    int enginesamplerate;
    int bitdepth;
    int buffersize;
    std::string inputpath;
    std::string outputpath;
    std::string outputformat;


    double initarr[512] = {0};
    inbuffer = initarr;


    lo::ServerThread st(9000);


    if (!st.is_valid()) {
        std::cout << "Nope." << std::endl;
    }



    st.start();


    // register all our parameters
    try
    {
        TCLAP::CmdLine cmd("Halite", ' ', "0.9");
        TCLAP::ValueArg<std::string> inputFile("i","input", "choose input file", false, "./precompile.ncl", "path");
        TCLAP::ValueArg<std::string> outputFile("o","output", "choose output file", false, "./output.wav", "path");
        TCLAP::ValueArg<int> osr("s","samplerate", "set sample rate for output", false, 44100, "integer");
        TCLAP::ValueArg<int> esr("t","interval", "set sample rate for engine", false, 44100, "integer");
        TCLAP::ValueArg<int> bd("d","bitdepht", "set bit depth for output", false, 24, "integer");
        TCLAP::ValueArg<int> bs("b","buffersize", "set realtime buffersize", false, 1024, "integer");
        TCLAP::ValueArg<std::string> format("f","format", "output file format", false, ".WAV", "string");
        TCLAP::SwitchArg rtswitch("r", "realtime", "enable realtime playback");

        cmd.add(inputFile);
        cmd.add(outputFile);
        cmd.add(esr);
        cmd.add(osr);
        cmd.add(bd);
        cmd.add(bs);
        cmd.add(rtswitch);
        cmd.add(format);
        cmd.parse(argc, argv);

        // assign them to variables
        outputformat = format.getValue();
        bitdepth = bd.getValue();
        buffersize = bs.getValue();
        outputsamplerate = osr.getValue();
        enginesamplerate = esr.getValue();
        inputpath = inputFile.getValue();
        outputpath = outputFile.getValue();
        realtime = rtswitch.getValue();

    }
    catch (TCLAP::ArgException &e)  // catch any exceptions
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }


    // Here we start parsing out input file that we generate in JS

    std::ifstream t(inputpath);
    std::string str;


    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);


    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

    std::stringstream ss(str);
    std::string obj;

    // first we split by newline
    while(std::getline(ss,obj,'\n'))
    {
        std::vector<std::string> seglist;
        std::vector<std::string> optargs;
        std::string segment;
        std::stringstream inputcode(obj);

        // then we split by commas
        while(std::getline(inputcode, segment, ','))
        {
            segment.erase(0,1);
            seglist.push_back(segment);
        }


        std::stringstream argstream(seglist[seglist.size()-1]);
        std::string parsedargs;

        // and then our final list of optional arguments is splitted with :
        while(std::getline(argstream, parsedargs, ':'))
        {
            // remove array blocks
            parsedargs.erase(0, 1);
            parsedargs.erase(parsedargs.size()-1, 1);
            //remove any quotes
            parsedargs.erase(
            remove( parsedargs.begin(), parsedargs.end(), '\"' ),
            parsedargs.end()
            );
            optargs.push_back(parsedargs);
        }


        if(!seglist[0].compare("setup"))
        {
            net = new NetList(stoi(seglist[1]), stoi(seglist[2]));
        }
        else if(!seglist[0].compare("ground") || !seglist[0].compare("comment"))
        {
            continue;
        }
// Digital components
        else if(!seglist[0].compare("slider-")) {
            std::string address(optargs[0]);
            int idx(std::stoi(optargs[1]));

            net->addComponent(new slider(seglist[1], idx));
            st.add_method(address, "f",
                          [&, seglist, idx](lo_arg **argv, float){
                            oscBuffer[idx] = argv[0]->f;
                        });
        }


        else if(!seglist[0].compare("cycle-"))
            net->addComponent(new sineGenerator(optargs, seglist[1], seglist[2]));

        else if(!seglist[0].compare("rect-"))
            net->addComponent(new squareGenerator(optargs, seglist[1], seglist[2]));

        else if(!seglist[0].compare("triangle-"))
            net->addComponent(new triangleGenerator(optargs, seglist[1], seglist[2]));

        else if(!seglist[0].compare("saw-"))
            net->addComponent(new sawGenerator(optargs, seglist[1], seglist[2]));

        else if(!seglist[0].compare("sig-"))
            net->addComponent(new digitalSignal(std::stof(seglist[1]), seglist[2]));

        else if(!seglist[0].compare("dac"))
            net->addComponent(new digitalAnalogConverter(seglist[1], std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("adc"))
            net->addComponent(new analogDigitalConverter(std::stoi(seglist[1]), std::stoi(seglist[2]), seglist[3]));

        else if(!seglist[0].compare("output-")) {
            net->addComponent(new digitalOutput(std::stof(seglist[1]), seglist[2], seglist[3]));
            outamp = std::stof(seglist[1]);
          }

        else if(!seglist[0].compare("input-"))
            net->addComponent(new digitalInput(seglist[1], std::stof(seglist[2]), seglist[3]));

        else if(!seglist[0].compare("stinput-"))
            net->addComponent(new stereoDigitalInput(seglist[1], std::stof(seglist[2]), seglist[3], seglist[4]));

        else if(!seglist[0].compare("rtinput-"))
            net->addComponent(new rtDigitalInput(std::stof(seglist[1]), seglist[2], buffersize));

        else if(!seglist[0].compare("ctlin-"))
            net->addComponent(new midiCtlIn(seglist[1], seglist[2]));

        else if(!seglist[0].compare("notein-"))
            net->addComponent(new midiNoteIn(seglist[1], seglist[2]));

        else if(!seglist[0].compare("mtof-"))
            net->addComponent(new mToF(seglist[1], seglist[2]));

        else if(!seglist[0].compare("digidelay"))
            net->addComponent(new digitalDelay(optargs, seglist[1], seglist[2], seglist[3]));

        else if(!seglist[0].compare("dcblock-"))
            net->addComponent(new dcBlock(seglist[1], seglist[2]));

        else if(!seglist[0].compare("change-"))
            net->addComponent(new Changed(seglist[1], seglist[2]));

        else if(!seglist[0].compare("delta-"))
            net->addComponent(new getDelta(seglist[1], seglist[2]));

        else if(!seglist[0].compare("history-"))
            net->addComponent(new History(seglist[1], seglist[2]));

        else if(!seglist[0].compare("elapsed-"))
            net->addComponent(new Elapsed(seglist[1]));

        else if(!seglist[0].compare("accum-"))
            net->addComponent(new accumulate(seglist[1], seglist[2]));

        else if(!seglist[0].compare("abs-"))
            net->addComponent(new absol(seglist[1], seglist[2]));

        else if(!seglist[0].compare("ceil-"))
            net->addComponent(new ceiling(seglist[1], seglist[2]));

        else if(!seglist[0].compare("floor-"))
            net->addComponent(new flor(seglist[1], seglist[2]));

        else if(!seglist[0].compare("scale-"))
            net->addComponent(new Scale(optargs, seglist[1], seglist[2], seglist[3], seglist[4], seglist[5], seglist[6]));

        else if(!seglist[0].compare("clip-"))
            net->addComponent(new Clip(optargs, seglist[1], seglist[2], seglist[3], seglist[4]));

        else if(!seglist[0].compare("gate-"))
            net->addComponent(new Gate(seglist[1], seglist[2], seglist[3]));
// Analog components
        else if(!seglist[0].compare("resistor"))
            net->addComponent(new Resistor(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("varres"))
            net->addComponent(new VariableResistor(std::stoi(seglist[1]), std::stoi(seglist[2]), seglist[3], optargs));

        else if(!seglist[0].compare("capacitor"))
            net->addComponent(new Capacitor(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("inductor"))
            net->addComponent(new Inductor(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("voltage"))
            net->addComponent(new Voltage(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("current"))
            net->addComponent(new Current(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("transformer"))
            net->addComponent(new Transformer(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4]), std::stoi(seglist[5])));


        else if(!seglist[0].compare("click"))
            net->addComponent(new Click(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("input"))
            net->addComponent(new InputSample(seglist[1], std::stof(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4])));

        else if(!seglist[0].compare("diode"))
            net->addComponent(new Diode(std::stoi(seglist[1]), std::stoi(seglist[2])));

        else if(!seglist[0].compare("bjt"))
            net->addComponent(new BJT(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4]), optargs));

        else if(!seglist[0].compare("opa"))
            net->addComponent(new OPA(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("potentiometer"))
            net->addComponent(new Potentiometer(stof(seglist[1]), stoi(seglist[2]), stoi(seglist[3]), stoi(seglist[4]), seglist[5]));


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
                !seglist[0].compare("!%-") ||
                !seglist[0].compare("pow-")  ||
                !seglist[0].compare("sqrt-"))
            net->addComponent(new digitalArithmetic(seglist[0], optargs, seglist[1], seglist[2],seglist[3]));

        else if(!seglist[0].compare("ln-")  ||
                !seglist[0].compare("log2-")  ||
                !seglist[0].compare("log10-"))
            net->addComponent(new logarithms(seglist[0], seglist[1], seglist[2]));

        else if(!seglist[0].compare("print-"))
            net->addComponent(new digitalPrinter(optargs, seglist[1]));

        else if(!seglist[0].compare("probe"))
        {
            net->addComponent(new Probe(stof(seglist[1]), std::stoi(seglist[2])));
            outamp = std::stof(seglist[3]);
        }

    }

    net->buildSystem();

// sets amount of time that is simulated between ticks (1/samplerate)

    net->setMidiInput(message);
    net->setOscBuffer(oscBuffer);
    //net->setAudioInput(inbuffer);

    net->simulateTick();
    net->setTimeStep((double)1/enginesamplerate);
    net->simulateTick();


    double* output;

    if (realtime == false)
    {

        int length = 8 * 44100;
        AudioFile<double> audioFile;
        audioFile.setAudioBufferSize (2, length);

        for (int i = 0; i<length; i++)
        {
            if(i%20000 == 0)
            {
                std::cout << int(i * (100./float(length))) << "%" << std::endl;
            }
            net->simulateTick();

            output = net->getAudioOutput();

            audioFile.samples[0][i] = *(output)*outamp;
            audioFile.samples[1][i] = *(output+1)*outamp;
        }


        audioFile.setBitDepth (bitdepth);
        audioFile.setSampleRate (outputsamplerate);

        std::cout << "Saved as: " << outputpath << std::endl;
        if (!outputformat.compare(".AIF") || !outputformat.compare(".AIFF") || !outputformat.compare(".aif") || !outputformat.compare(".aiff"))
            audioFile.save(outputpath, AudioFileFormat::Aiff);
        else
            audioFile.save(outputpath);
    }

    else
    {

        midiin->openVirtualPort("Halite Input Port 1");
        midiin->ignoreTypes( false, false, false );


        RtAudio dac;
        if ( dac.getDeviceCount() < 1 )
        {
            std::cout << "\nNo audio devices found!\n";
            exit( 1 );
        }

        channs = 2;


        double *data = (double *) calloc( channs, sizeof( double ) );

        // Let RtAudio print messages to stderr.
        dac.showWarnings( true );

        // Set our stream parameters for output only.
        bufferFrames = buffersize;
        RtAudio::StreamParameters oParams;
        oParams.deviceId = dac.getDefaultOutputDevice();
        oParams.nChannels = channs;
        oParams.firstChannel = offset;

        RtAudio::StreamParameters iParams;
        iParams.deviceId = dac.getDefaultInputDevice();
        iParams.nChannels = 1;
        iParams.firstChannel = offset;

        options.flags = RTAUDIO_HOG_DEVICE;
        options.flags = RTAUDIO_SCHEDULE_REALTIME;

        try
        {
            dac.openStream( &oParams, &iParams, RTAUDIO_FLOAT64, outputsamplerate, &bufferFrames, &inout, (void *)&bufferBytes, &options );
            dac.startStream();
        }
        catch ( RtAudioError& e )
        {
            e.printMessage();
            goto cleanup;
        }

        if ( checkCount )
        {
            while ( dac.isStreamRunning() == true ) usleep( 100 );
        }
        else
        {
            std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;

            while(true) {
              usleep(10000);
            }

            try
            {
                // Stop the stream
                dac.stopStream();
            }
            catch ( RtAudioError& e )
            {
                e.printMessage();
            }
        }

cleanup:
        if ( dac.isStreamOpen() ) dac.closeStream();
        free( data );

        return 0;

    }

};
