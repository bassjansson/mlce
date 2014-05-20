//
//  Schedular.cpp
//  MidiLiveCodingEnvironment
//
//  Created by Bass Jansson on 21/04/14.
//  Copyright (c) 2014 Bass Jansson. All rights reserved.
//

#include <iostream>
#include "portmidi.h"
#include "porttime.h"
#include "NoteMessages.h"

using namespace std;

#define TIMER_INTERVAL     1
#define OUTPUT_BUFFER_SIZE 1
#define OUTPUT_LATANCY     1

#define PROGRAM_STOP -1
#define USER_STOP     0
#define RUNNING       1


//==================================================================
void checkForError (PmError error)
{
    if (error != pmNoError)
    {
        cout << "Schedular exited with PmError: " << Pm_GetErrorText(error) << endl;
        exit(error);
    }
}


void checkForError (PtError error)
{
    if (error != ptNoError)
    {
        cout << "Schedular exited with PtError: " << error << endl;
        exit(error);
    }
}


//==================================================================
void portTimeCallback (PtTimestamp timeStamp, void* userData)
{
    // Route userData
    void** userDataPointer = (void**)userData;
    
    PortMidiStream** portMidiStream = (PortMidiStream**)userDataPointer[0];
    MidiMessage**    midiMessages   =    (MidiMessage**)userDataPointer[1];
    int*             size           =             (int*)userDataPointer[2];
    int*             position       =             (int*)userDataPointer[3];
    int*             startTimeStamp =             (int*)userDataPointer[4];
    int*             programRun     =             (int*)userDataPointer[5];
    
    
    // Check if portMidiStream is initialised and program is running
    if (*portMidiStream != NULL && *programRun != PROGRAM_STOP)
    {
        // Check if it is time for the next midi message
        if ((*midiMessages)[*position].timeStamp + *startTimeStamp < timeStamp)
        {
            // Check if it is an endMidiMessage
            if ((*midiMessages)[*position].status == END_MSG)
            {
                // Go to the end of the midi messages
                *position = *size;
            }
            else
            {
                // Write midi message to portMidiStream
                Pm_WriteShort(*portMidiStream, 0, Pm_Message((*midiMessages)[*position].status,
                                                             (*midiMessages)[*position].data1,
                                                             (*midiMessages)[*position].data2));
                
                // Write midi message to cout
                cout << (*midiMessages)[*position].timeStamp << "\t";
                cout << (*midiMessages)[*position].status    << "\t";
                cout << (*midiMessages)[*position].data1     << "\t";
                cout << (*midiMessages)[*position].data2     << endl;
                
                // Go to the next midi message
                (*position)++;
            }
            
            
            // Check if we are at the end of the midi messages
            if (*position >= *size)
            {
                // Check if we have to stop the pogram or to read again
                if (*programRun == USER_STOP)
                {
                    // Stop the program
                    *programRun = PROGRAM_STOP;
                }
                else
                {
                    // Read midi messages
                    readMidiMessages(midiMessages, size);
                    
                    // Reset position and update startTimeStamp
                    *position       = 0;
                    *startTimeStamp = timeStamp;
                    
                    // Write true to MidiMessagesFinished
                    writeMidiMessagesFinished(true);
                }
            }
        }
    }
}


//==================================================================
int main (int argc, const char* argv[])
{
    // Initialising PortMidi
    checkForError(Pm_Initialize());
    
    
    // Creating a PortMidiStream and a PmDeviceID
    PortMidiStream* portMidiStream;
    PmDeviceID      outputDeviceID;
    
    
    // Output a list of the available devices
    cout << endl;
    for (PmDeviceID deviceID = 0; deviceID < Pm_CountDevices(); deviceID++)
    {
        cout << deviceID << ":\t" << Pm_GetDeviceInfo(deviceID)->name;
        if (Pm_GetDeviceInfo(deviceID)->input)  cout << "\t\tINPUT";
        if (Pm_GetDeviceInfo(deviceID)->output) cout << "\t\tOUTPUT";
        cout << endl;
    }
    cout << endl;
    
    
    // Let the user choose the output device
    cout << "Choose output device number: ";
    cin  >> outputDeviceID;
    cout << endl;
    
    
    // Creating a userData for the portTimeCallback
    void** userData = new void*[6];
    
    
    // Start PortTime and open output of portMidiStream
    checkForError(Pt_Start(TIMER_INTERVAL, &portTimeCallback, userData));
    checkForError(Pm_OpenOutput(&portMidiStream, outputDeviceID, NULL,
                                OUTPUT_BUFFER_SIZE, NULL, NULL, OUTPUT_LATANCY));
    
    
    // Initialising userData
    MidiMessage* midiMessages   = new MidiMessage[1];
    int          size           = 1;
    int          position       = 0;
    int          startTimeStamp = 0;
    int          programRun     = RUNNING;
    
    midiMessages[0] = newMidiMessage(0, END_MSG, 0, 0);
    
    userData[0] = &portMidiStream;
    userData[1] = &midiMessages;
    userData[2] = &size;
    userData[3] = &position;
    userData[4] = &startTimeStamp;
    userData[5] = &programRun;
    
    
    // Stop running when user gives 0
    while (programRun != PROGRAM_STOP)
    {
        if (programRun == RUNNING)
        {
            bool input;
            cout << "Give 0 to stop the schedular...\n\n";
            cin  >> input;
            programRun = input;
        }
    }
    
    
    // Abort and close portMidiStream and stop PortTime
    checkForError(Pm_Abort(portMidiStream));
    checkForError(Pm_Close(portMidiStream));
    checkForError(Pt_Stop());
    
    
    // Terminating PortMidi
    checkForError(Pm_Terminate());
    
    
    // End of program
    cout << "\nSchedular exited succesfully!\n\n";
    return 0;
}
