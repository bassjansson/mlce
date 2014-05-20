//
//  NoteMessages.cpp
//  MidiLiveCodingEnvironment
//
//  Created by Bass Jansson on 09/05/14.
//  Copyright (c) 2014 Bass Jansson. All rights reserved.
//

#include "NoteMessages.h"


//==================================================================
MidiMessage newMidiMessage (int timeStamp, int status, int data1, int data2)
{
    MidiMessage message;
    
    message.timeStamp = timeStamp;
    message.status    = status;
    message.data1     = data1;
    message.data2     = data2;
    
    return message;
}

NoteMessage newNoteMessage (int timeStamp, int status, int data1, int data2, int duration)
{
    NoteMessage message;
    
    message.timeStamp = timeStamp;
    message.status    = status;
    message.data1     = data1;
    message.data2     = data2;
    message.duration  = duration;
    
    return message;
}


void addNote (NoteMessageVector& messages,
              int timeStamp,
              int pitch,
              int velocity,
              int duration)
{
    messages.push_back(newNoteMessage(timeStamp,
                                      NOTE,
                                      pitch,
                                      velocity,
                                      duration));
}

void addSustain (NoteMessageVector& messages, int timeStamp, int duration)
{
    messages.push_back(newNoteMessage(timeStamp,
                                      CTRL_CHANGE,
                                      CTRL_SUSTAIN,
                                      0,
                                      duration));
}

void addSoftPedal (NoteMessageVector& messages, int timeStamp, int duration)
{
    messages.push_back(newNoteMessage(timeStamp,
                                      CTRL_CHANGE,
                                      CTRL_SOFT_PEDAL,
                                      0,
                                      duration));
}


void setEndTimeStamp (NoteMessageVector& messages, int timeStamp)
{
    if (messages.size() < 1)
        messages.push_back(newNoteMessage(timeStamp, END_MSG, 0, 0, 0));
    else
    {
        if (messages.front().status == END_MSG)
            messages.front().timeStamp = timeStamp;
        else
        {
            messages.clear();
            messages.push_back(newNoteMessage(timeStamp, END_MSG, 0, 0, 0));
        }
    }
}

int getEndTimeStamp (NoteMessageVector& messages)
{
    if (messages.size() > 0)
        if (messages.front().status == END_MSG)
            return messages.front().timeStamp;
    
    return 0;
}


//==================================================================
void midiToScalePitch (NoteMessage& message, int* scale, int size, int key)
{
    if (message.status == NOTE)
    {
        int pitch = message.data1;
        
        key   %= 12;
        pitch -= key;
        
        int base   = pitch / 12 * size;
        int remain = pitch % 12;
        
        for (int i = size - 1; i >= 0; i--)
        {
            if (remain >= scale[i])
            {
                message.data1 = base + i;
                break;
            }
        }
    }
}

void scaleToMidiPitch (NoteMessage& message, int* scale, int size, int key)
{
    if (message.status == NOTE)
    {
        int pitch = message.data1;
        
        int base   = pitch / size * 12;
        int remain = scale[pitch % size];
        
        message.data1 = base + remain + key;
    }
}


void midiToScalePitch (NoteMessageVector& messages, int* scale, int size, int key)
{
    for (int i = 0; i < messages.size(); i++)
        midiToScalePitch(messages[i], scale, size, key);
}

void scaleToMidiPitch (NoteMessageVector& messages, int* scale, int size, int key)
{
    for (int i = 0; i < messages.size(); i++)
        scaleToMidiPitch(messages[i], scale, size, key);
}


//==================================================================
void noteToMidiMessages (NoteMessage* noteMessages, int noteSize,
                         MidiMessageVector& midiMessages)
{
    // I love you Frederike! :)
    // Clear midi messages before pushing
    midiMessages.clear();
    
    // Set endTimeStamp default to 0
    int endTimeStamp = 0;
    
    // Check if there is an end message
    if (noteSize > 0)
    {
        if (noteMessages[0].status == END_MSG)
        {
            // Set endTimeStamp
            endTimeStamp = noteMessages[0].timeStamp;
            
            // Push end message into midiMessages
            midiMessages.push_back(newMidiMessage(noteMessages[0].timeStamp,
                                                  noteMessages[0].status,
                                                  noteMessages[0].data1,
                                                  noteMessages[0].data2));
        }
    }
    
    // Walk note messages
    for (int i = 0; i < noteSize; i++)
    {
        // Check if time stamp is smaller than endTimeStamp
        if (noteMessages[i].timeStamp < endTimeStamp - 20)
        {
            // Clip timeStampDuration to endTimeStamp
            int timeStampDuration = noteMessages[i].timeStamp + noteMessages[i].duration;
            if (timeStampDuration > endTimeStamp - 10)
                timeStampDuration = endTimeStamp - 10;
            
            // Switch status of note message, convert note message
            // into midi message and push it into midiMessages
            switch (noteMessages[i].status)
            {
                case NOTE:
                    midiMessages.push_back(newMidiMessage(noteMessages[i].timeStamp,
                                                          NOTE_ON,
                                                          noteMessages[i].data1,
                                                          noteMessages[i].data2));
                    
                    midiMessages.push_back(newMidiMessage(timeStampDuration,
                                                          NOTE_OFF,
                                                          noteMessages[i].data1,
                                                          noteMessages[i].data2)); break;
                    
                case CTRL_CHANGE:
                    midiMessages.push_back(newMidiMessage(noteMessages[i].timeStamp,
                                                          noteMessages[i].status,
                                                          noteMessages[i].data1,
                                                          CTRL_ON));
                    
                    midiMessages.push_back(newMidiMessage(timeStampDuration,
                                                          noteMessages[i].status,
                                                          noteMessages[i].data1,
                                                          CTRL_OFF));              break;
            }
        }
    }
    
    // Sort midi messages
    sort(midiMessages.begin(), midiMessages.end());
}


void writeMidiMessages (MidiMessage* messages, int size)
{
    string file = "./MidiMessages.mlce";
    
    ofstream outputStream (file, ios::out | ios::binary);
    
    if (outputStream.is_open())
    {
        // Write messages to outputStream
        outputStream.seekp(0, ios::beg);
        outputStream.write((char*)messages, size * sizeof(MidiMessage));
        outputStream.close();
        
        cout << "\nWriting to " << file << " succeeded!\n\n";
    }
    else
        cout << "\nWriting to " << file << " failed...\n\n";
}

void readMidiMessages (MidiMessage** messages, int* size)
{
    string file = "./MidiMessages.mlce";
    
    ifstream inputStream (file, ios::in | ios::binary | ios::ate);
    
    if (inputStream.is_open())
    {
        // Get size and reallocate messages
        *size     = int(inputStream.tellg()) / sizeof(MidiMessage);
        *messages = new MidiMessage[*size];
        
        // Read inputStream to messages
        inputStream.seekg(0, ios::beg);
        inputStream.read ((char*)*messages, *size * sizeof(MidiMessage));
        inputStream.close();
        
        cout << "\nReading from " << file << " succeeded!\n\n";
    }
    else
        cout << "\nReading from " << file << " failed...\n\n";
}


void writeNoteMessages (NoteMessage* messages, int size)
{
    string file = "./NoteMessages.mlce";
    
    ofstream outputStream (file, ios::out | ios::binary);
    
    if (outputStream.is_open())
    {
        // Write messages to outputStream
        outputStream.seekp(0, ios::beg);
        outputStream.write((char*)messages, size * sizeof(NoteMessage));
        outputStream.close();
        
        cout << "\nWriting to " << file << " succeeded!\n\n";
    }
    else
        cout << "\nWriting to " << file << " failed...\n\n";
}

void readNoteMessages (NoteMessage** messages, int* size)
{
    string file = "./NoteMessages.mlce";
    
    ifstream inputStream (file, ios::in | ios::binary | ios::ate);
    
    if (inputStream.is_open())
    {
        // Get size and reallocate messages
        *size     = int(inputStream.tellg()) / sizeof(NoteMessage);
        *messages = new NoteMessage[*size];
        
        // Read inputStream to messages
        inputStream.seekg(0, ios::beg);
        inputStream.read ((char*)*messages, *size * sizeof(NoteMessage));
        inputStream.close();
        
        cout << "\nReading from " << file << " succeeded!\n\n";
    }
    else
        cout << "\nReading from " << file << " failed...\n\n";
}


void writeNoteMessages (NoteMessageVector& messages)
{
    // Write note messages
    writeNoteMessages(messages.data(), int(messages.size()));
    
    // Convert note messages into midi messages
    MidiMessageVector midiMessages;
    noteToMidiMessages(messages.data(), int(messages.size()), midiMessages);
    
    // Write midi messages
    writeMidiMessages(midiMessages.data(), int(midiMessages.size()));
}

void readNoteMessages (NoteMessageVector& messages)
{
    // Read note messages
    NoteMessage* noteMessages;
    int          noteSize;
    readNoteMessages(&noteMessages, &noteSize);
    
    // Clear messages before pushing
    messages.clear();
    
    // Push each note message into messages
    for (int i = 0; i < noteSize; i++)
        messages.push_back(noteMessages[i]);
    
    // Delete noteMessages
    delete[] noteMessages;
}


void writeMidiMessagesFinished (bool isFinished)
{
    string file = "./MidiMessagesFinished.mlce";
    
    ofstream outputStream (file, ios::out | ios::binary);
    
    if (outputStream.is_open())
    {
        // Write isFinished to outputStream
        outputStream.clear();
        outputStream << isFinished;
        
        cout << "\nWriting to " << file << " succeeded!\n\n";
    }
    else
        cout << "\nWriting to " << file << " failed...\n\n";
}
