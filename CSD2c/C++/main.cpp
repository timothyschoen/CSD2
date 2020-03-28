// Circuitry designer by Timothy Schoen and Joeri Braams
// Based on mystran's Halite

// Source code can be found here: https://github.com/mbrucher/Halite/blob/master/halite.cpp
// Our system's structure is based on this program. We've added a faster solver, many more components (the original had like 7), and extensive digital capabilities
// which can be used to modulate analog components or for audio processing.


// If you want to make sense of this code you really need to read this:

// http://qucs.sourceforge.net/tech/node14.html
// For deeper research, use anything on http://qucs.sourceforge.net/tech/

// This is also interesting:
// https://www.swarthmore.edu/NatSci/echeeve1/Ref/mna/MNA2.html


#include <unistd.h>

// OSC libraries
#include <lo/lo.h>
#include <lo/lo_cpp.h>

// Libraries to parse strings from file
#include <sstream>
#include <fstream>
#include <streambuf>


// Library for parsing command line parameters
#include <tclap/CmdLine.h>

// Audio I/O libraries
#include "rtlibs/RtAudio.h"
#include "rtlibs/RtMidi.h"
#include "AudioFile.h"

// The main MNA system
#include "MNASystem.h"
#include "NetList.h"
#include "MNASolver.h"

// Component base classes
#include "IComponent.h"
#include "Component.h"

// Digital components (components that use direct communication instead of analog simulation!! Not simulations of digital electronics!!)
#include "Components/digitalSignalGenerators.h"
#include "Components/digitalIO.h"
#include "Components/digitalCalculus.h"
#include "Components/digitalUtility.h"
#include "Components/digitalTimeBased.h"

// Components to convert between digital and analog domains
#include "Components/domainConverters.h"

// Analog components (resistor, capacitors, etc.)
#include "Components/analogIO.h"
#include "Components/analogBasics.h"
#include "Components/analogJunctions.h"
#include "Components/analogInteraction.h"



// The netlist class holds the system and solver, and is also our way of interfacing with those parts
NetList * net;
// default output volume
double outamp = 0.2;
unsigned int bufferFrames, fs = 44100, offset = 0;

// Midi input
RtMidiIn *midiin = new RtMidiIn();
// Midi input buffer
std::vector<unsigned char> message;

// Osc input buffer
std::vector<double> oscBuffer(20, 0.5);



unsigned int channs;
RtAudio::StreamOptions options;
unsigned int frameCounter = 0;
bool checkCount = false;
unsigned int nFrames = 0;
unsigned int bufferBytes;



const unsigned int callbackReturnValue = 1;

// Rtaudio error callback
void errorCallback( RtAudioError::Type type, const std::string &errorText )
{
        std::cout << "in errorCallback" << std::endl;
        if ( type == RtAudioError::WARNING )
                std::cerr << '\n' << errorText << "\n\n";
        else if ( type != RtAudioError::WARNING )
                throw( RtAudioError( errorText, type ) );
}

// Audio loop for RtAudio
int inout( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
           double /*streamTime*/, RtAudioStreamStatus status, void *data )
{
        unsigned int i;
        extern unsigned int channs;
        double *buffer = (double *) outputBuffer; // output buffer that we pass back to rtaudio


        net->input = (double *)inputBuffer; // pass audio input buffer

        midiin->getMessage( &message ); // get midi messages

        for ( i=0; i<nBufferFrames; i++ )
        {
                net->simulateTick(); // Tell the network to advance by one tick
                *buffer++ = net->output[0]*outamp; // get left output from the netlist
                *buffer++ = net->output[1]*outamp; // get right output from the netlist

        }


        frameCounter += nBufferFrames;
        if ( checkCount && ( frameCounter >= nFrames ) ) return callbackReturnValue;
        return 0;
}




int main(int argc, char* argv[])
{

        // Parameter variables
        bool realtime;
        int outputsamplerate;
        int enginesamplerate;
        int bitdepth;
        int buffersize;
        int newtonIterations;
        std::string inputpath;
        std::string outputpath;
        std::string outputformat;


        // Set up Osc server
        lo::ServerThread st(9000);

        if (!st.is_valid()) {
                std::cout << "Could not open OSC Server. Make sure channel 9000 is free" << std::endl;
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
                TCLAP::ValueArg<int> iter("n","iterations", "maximum newton iterations", false, 2, "integer");
                TCLAP::ValueArg<std::string> format("f","format", "output file format", false, ".WAV", "string");
                TCLAP::SwitchArg rtswitch("r", "realtime", "enable realtime playback");

                cmd.add(inputFile);
                cmd.add(outputFile);
                cmd.add(esr);
                cmd.add(osr);
                cmd.add(bd);
                cmd.add(bs);
                cmd.add(iter);
                cmd.add(rtswitch);
                cmd.add(format);
                cmd.parse(argc, argv);

                // assign them to variables
                outputformat = format.getValue();
                bitdepth = bd.getValue();
                buffersize = bs.getValue();
                newtonIterations = iter.getValue();
                outputsamplerate = osr.getValue();
                enginesamplerate = esr.getValue();
                inputpath = inputFile.getValue();
                outputpath = outputFile.getValue();
                realtime = rtswitch.getValue();

                std::cout << newtonIterations << '\n';

        }
        catch (TCLAP::ArgException &e) // catch any exceptions
        {
                // This will inform the user what argument caused the error
                // The user actually sees the error this program throws in the JS console (which is in the sidebar)
                std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
        }

        // Here we start parsing the input file that we generate in JS

        // Open file from path
        std::ifstream t(inputpath);
        std::string str;

        // claim space
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

                // Now it's time to read what was passed to us!
                // First we check the name, if we know that then we will know exactly what arguments will follow and parse them accordingly
                // For example: if the name is Resistor, I know that it has 1 argument, float (resistance in ohms), so I can parse it as a float

                // The setup initializes the netlist, and tells it how many nodes and components it has to make room for
                if(!seglist[0].compare("setup"))
                {
                        net = new NetList(stoi(seglist[1]), stoi(seglist[2]));
                }
                // These variables mean nothing so we skip over them
                else if(!seglist[0].compare("ground") || !seglist[0].compare("comment"))
                {
                        continue;
                }
                // Digital components
                // Our UI slider
                else if(!seglist[0].compare("slider-")) {
                        // read address
                        std::string address(optargs[0]);
                        // read what the unique index number for this slider is
                        int idx(std::stoi(optargs[1]));

                        net->addComponent(new slider(seglist[1], idx));
                        // Set up the OSC listener
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

        // The number of newton iterations decides how accurately the transistors and diodes will be modelled
        // Each iteration will vastly increase the quality, but it currently requires the whole system to be recalculated
        // For realtime playback you don't want this higher than 2
        // For non-realtime, it can be something like 20 or higher

        // You can set this value from the sidebar in JS

        net->setIterations(newtonIterations);

        // Build the system!
        net->buildSystem();

        // Pass MIDI and OSC buffers
        net->setMidiInput(message);
        net->setOscBuffer(oscBuffer);

        net->input = new double[buffersize](); // set init values

        // Simulating a tick before the timestep removes any DC clicks
        net->simulateTick();

        // sets amount of time that is simulated between ticks (1/samplerate)
        net->setTimeStep((double)1/enginesamplerate);
        net->simulateTick();


        // Audio output:

        double* output;


        if (realtime == false)
        {
                // Set up audio file for non-realtime mode
                int length = 8 * 44100;
                AudioFile<double> audioFile;
                audioFile.setAudioBufferSize (2, length);

                // Tick the system and put the output in a file
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

                // Export
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
                // We only do midi in realtime

                midiin->openVirtualPort("Halite Input Port 1");
                midiin->ignoreTypes( false, false, false );

                // initialize RTaudio
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

                // Set our stream parameters
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
                        // Open the stream, passes the audio processing function
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
