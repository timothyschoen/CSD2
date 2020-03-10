#include <sstream>
#include "./rtlibs/RtAudio.h"
#include "./rtlibs/RtMidi.h"

//#include "./gen/genlib.h"

double* inbuffer;

#include "./halite.cpp"
#include "./digitalComponents.cpp"
#include "./analogComponents.cpp"
//#include "./genComponents.cpp"



#include <fstream>
#include <streambuf>
#include <tclap/CmdLine.h>

std::vector<std::string> savefile;
std::vector<std::string> object;
NetList * net;
double outamp = 0.2;
unsigned int bufferFrames, fs = 44100, offset = 0;

RtMidiIn *midiin = new RtMidiIn();
std::vector<unsigned char> message;

#define FORMAT RTAUDIO_FLOAT64
#define SCALE  1.0

void errorCallback( RtAudioError::Type type, const std::string &errorText )
{
    // This example error handling function does exactly the same thing
    // as the embedded RtAudio::error() function.
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
    //unsigned int *bytes = (unsigned int *) data;

    inbuffer = (double *) inputBuffer;

    double *output;
    midiin->getMessage( &message );

    for ( i=0; i<nBufferFrames; i++ )
    {
        net->simulateTick();
        output = net->getAudioOutput();
        *buffer++ = output[0]*outamp;
        *(buffer+nBufferFrames) = output[1]*outamp;
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



    std::ifstream t(inputpath);
    std::string str; // = "output, 0.3, 2 \n input, ./samples/sample-44k.wav, 0.2, 1, 0 \n resistor, 200, 1, 2 \n ";


    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);


    str.assign((std::istreambuf_iterator<char>(t)),
               std::istreambuf_iterator<char>());

    std::stringstream ss(str);
    std::string obj;


    while(std::getline(ss,obj,'\n'))
    {
        std::vector<std::string> seglist;
        std::vector<std::string> optargs;
        std::string segment;
        std::stringstream inputcode(obj);


        while(std::getline(inputcode, segment, ','))
        {
            segment.erase(0,1);
            seglist.push_back(segment);
        }


        std::stringstream argstream(seglist[seglist.size()-1]);
        std::string parsedargs;


        while(std::getline(argstream, parsedargs, ':'))
        {
            parsedargs.erase(0, 1);
            parsedargs.erase(parsedargs.size()-1, 1);
            optargs.push_back(parsedargs);
        }

        std::cout << seglist[0] << '\n';

        if(!seglist[0].compare("setup"))
        {
            net = new NetList(stoi(seglist[1]), stoi(seglist[2]));
        }
        else if(!seglist[0].compare("ground"))
        {
            continue;
        }
// Digital components

        else if(!seglist[0].compare("cycle-"))
            net->addComponent(new sineGenerator(optargs, seglist[1], seglist[2]));

        else if(!seglist[0].compare("rect-"))
            net->addComponent(new squareGenerator(optargs, seglist[1], seglist[2]));

        else if(!seglist[0].compare("triangle-"))
            net->addComponent(new triangleGenerator(optargs, seglist[1], seglist[2]));

        else if(!seglist[0].compare("phasor-"))
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
            net->addComponent(new rtDigitalInput(std::stof(seglist[1]), seglist[2]));

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

        else if(!seglist[0].compare("gate-"))
            net->addComponent(new Gate(seglist[1], seglist[2], seglist[3]));
// Analog components
        else if(!seglist[0].compare("resistor"))
            net->addComponent(new Resistor(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("varres"))
            net->addComponent(new VariableResistor(std::stoi(seglist[1]), std::stoi(seglist[2]), seglist[3], optargs));

        else if(!seglist[0].compare("capacitor"))
            net->addComponent(new Capacitor(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("varcap"))
            net->addComponent(new VariableCapacitor(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]), seglist[4]));

        else if(!seglist[0].compare("inductor"))
            net->addComponent(new Inductor(stod(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("voltage"))
            net->addComponent(new Voltage(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("click"))
            net->addComponent(new Click(stof(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("input"))
            net->addComponent(new InputSample(seglist[1], std::stof(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4])));

        else if(!seglist[0].compare("diode"))
            net->addComponent(new Diode(std::stoi(seglist[1]), std::stoi(seglist[2])));

        else if(!seglist[0].compare("bjt"))
            net->addComponent(new BJT(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3]), std::stoi(seglist[4])));

        else if(!seglist[0].compare("opa"))
            net->addComponent(new OPA(std::stoi(seglist[1]), std::stoi(seglist[2]), std::stoi(seglist[3])));

        else if(!seglist[0].compare("potentiometer"))
            net->addComponent(new Potentiometer(stof(seglist[1]), stoi(seglist[2]), stoi(seglist[3]), stoi(seglist[4]), seglist[5]));

        else if(!seglist[0].compare("print"))
            net->addComponent(new Printer(std::stoi(seglist[1]), std::stoi(seglist[2])));

        else if(!seglist[0].compare("delay"))
            net->addComponent(new AnalogDelay(std::stoi(seglist[1]), std::stof(seglist[2]), std::stoi(seglist[3]), seglist[4], std::stoi(seglist[5])));


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
            net->addComponent(new digitalArithmetic(seglist[0], optargs, seglist[1], seglist[2],seglist[3]));


        else if(!seglist[0].compare("probe"))
        {
            net->addComponent(new Probe(stof(seglist[1]), std::stoi(seglist[2])));
            outamp = std::stof(seglist[3]);
        }

    }

    net->buildSystem();

// sets amount of time that is simulated between ticks (1/samplerate)
    std::cout << enginesamplerate << std::endl;
    std::cout << (double)1/enginesamplerate << std::endl;

    net->setMidiInput(message);
    net->setAudioInput(inbuffer);

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
            //std::cout << *output << '\n';

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

    else
    {

        midiin->openVirtualPort("Halite Input Port 1");
        midiin->ignoreTypes( false, false, false );

        /*
                JackModule jack;
                jack.init("halite");


                jack.onProcess = [&net, &output, &outamp, &midiin, &message](jack_default_audio_sample_t *inBuf,
                                 jack_default_audio_sample_t *outBufR, jack_default_audio_sample_t *outBufL, jack_nframes_t nframes)
                {
                    midiin->getMessage( &message ); // get midi once per buffer
                    //loop through frames, retrieve sample of sine per frame
                    for(int i = 0; i < nframes; i++) {

                        net->setAudioInput(inBuf[i]);
                        net->simulateTick();
                        output = net->getAudioOutput();

                        outBufL[i] = *(output)*outamp;
                        outBufR[i] = *(output+1)*outamp;
                    }


                    return 0;
                };
                jack.autoConnect(); */



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
        bufferFrames = 512;
        RtAudio::StreamParameters oParams;
        oParams.deviceId = dac.getDefaultOutputDevice();
        oParams.nChannels = channs;
        oParams.firstChannel = offset;

        RtAudio::StreamParameters iParams;
        iParams.deviceId = dac.getDefaultInputDevice();
        iParams.nChannels = 1;
        iParams.firstChannel = offset;

        options.flags = RTAUDIO_HOG_DEVICE;
        //options.flags = RTAUDIO_SCHEDULE_REALTIME;
        options.flags |= RTAUDIO_NONINTERLEAVED;

        try
        {
            dac.openStream( &oParams, &iParams, FORMAT, 44100, &bufferFrames, &inout, (void *)&bufferBytes, &options );
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
            char input;
            //std::cout << "Stream latency = " << dac.getStreamLatency() << "\n" << std::endl;
            std::cout << "\nPlaying ... press <enter> to quit (buffer size = " << bufferFrames << ").\n";
            std::cin.get( input );

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
