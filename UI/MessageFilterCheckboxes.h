//
// Created by Casper De Smet on 06/03/2022.
//

#ifndef MIDIASSISTANT_MESSAGEFILTERCHECKBOXES_H
#define MIDIASSISTANT_MESSAGEFILTERCHECKBOXES_H

#include "../MIDI/MessageFilter.h"

namespace UI
{
    class MessageFilterCheckboxes: public MIDI::MessageFilter
    {
    public:
        int rowHeight;
        std::map<MIDI::MessageType, std::string> typeNames =
                  {{MIDI::MessageType::AFTERTOUCH,      "Aftertouch"},
                   {MIDI::MessageType::POLY_PRESSURE,   "Channel Pressure"},
                   {MIDI::MessageType::CONTROL_CHANGE,  "Controller"},
                   {MIDI::MessageType::NOTE_OFF,        "Note Off"},
                   {MIDI::MessageType::NOTE_ON,         "Note On"},
                   {MIDI::MessageType::PITCH_BEND,      "Pitch Wheel"},
                   {MIDI::MessageType::PROGRAM_CHANGE,  "Program Change"}};

        bool renderCheckboxes();
        void manageGroups();
    };
}

#endif //MIDIASSISTANT_MESSAGEFILTERCHECKBOXES_H
