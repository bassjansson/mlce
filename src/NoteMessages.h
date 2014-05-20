//
//  NoteMessages.h
//  MidiLiveCodingEnvironment
//
//  Created by Bass Jansson on 09/05/14.
//  Copyright (c) 2014 Bass Jansson. All rights reserved.
//

#ifndef __MidiLiveCodingEnvironment__NoteMessages__
#define __MidiLiveCodingEnvironment__NoteMessages__

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "portmidi.h"

using namespace std;

// Status
#define END_MSG    -1
#define NOTE        0
#define NOTE_OFF    128
#define NOTE_ON     144
#define CTRL_CHANGE 176

// Data1
#define CTRL_SUSTAIN    64
#define CTRL_SOFT_PEDAL 67

// Data2
#define CTRL_OFF 0
#define CTRL_ON  127


//==================================================================
struct MidiMessage
{
    int timeStamp;
    int status;
    int data1;
    int data2;
    
    bool operator <  (const MidiMessage& msg) const { return timeStamp <  msg.timeStamp; }
    bool operator <= (const MidiMessage& msg) const { return timeStamp <= msg.timeStamp; }
    bool operator >  (const MidiMessage& msg) const { return timeStamp >  msg.timeStamp; }
    bool operator >= (const MidiMessage& msg) const { return timeStamp >= msg.timeStamp; }
    bool operator == (const MidiMessage& msg) const { return timeStamp == msg.timeStamp; }
};

struct NoteMessage
{
    int timeStamp;
    int status;
    int data1;
    int data2;
    int duration;
};

typedef vector<MidiMessage> MidiMessageVector;
typedef vector<NoteMessage> NoteMessageVector;


//==================================================================
MidiMessage newMidiMessage (int timeStamp, int status, int data1, int data2);
NoteMessage newNoteMessage (int timeStamp, int status, int data1, int data2, int duration);

void addNote (NoteMessageVector& messages,
              int timeStamp,
              int pitch,
              int velocity,
              int duration);

void addSustain   (NoteMessageVector& messages, int timeStamp, int duration);
void addSoftPedal (NoteMessageVector& messages, int timeStamp, int duration);

void setEndTimeStamp (NoteMessageVector& messages, int timeStamp);
int  getEndTimeStamp (NoteMessageVector& messages);


//==================================================================
void midiToScalePitch (NoteMessage& message, int* scale, int size, int key);
void scaleToMidiPitch (NoteMessage& message, int* scale, int size, int key);

void midiToScalePitch (NoteMessageVector& messages, int* scale, int size, int key);
void scaleToMidiPitch (NoteMessageVector& messages, int* scale, int size, int key);


//==================================================================
void noteToMidiMessages (NoteMessage* noteMessages, int noteSize,
                         MidiMessageVector& midiMessages);

void writeMidiMessages (MidiMessage*  messages, int  size);
void  readMidiMessages (MidiMessage** messages, int* size);

void writeNoteMessages (NoteMessage*  messages, int  size);
void  readNoteMessages (NoteMessage** messages, int* size);

void writeNoteMessages (NoteMessageVector& messages);
void  readNoteMessages (NoteMessageVector& messages);

void writeMidiMessagesFinished (bool isFinished);


#endif /* defined(__MidiLiveCodingEnvironment__NoteMessages__) */
