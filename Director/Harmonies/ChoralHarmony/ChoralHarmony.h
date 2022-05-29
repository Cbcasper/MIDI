//
// Created by Casper De Smet on 28/05/2022.
//

#ifndef MIDIASSISTANT_CHORALHARMONY_H
#define MIDIASSISTANT_CHORALHARMONY_H

#include <array>

#include "Tessiture.h"
#include "../Harmony.h"
#include "../../../Theory/Key/Key.h"
#include "../../../Theory/Chord/Chord.h"
#include "../../../MIDI/Messages/Messages/NoteOn.h"

namespace Music
{
    class ChoralHarmony: public Harmony
    {
    public:
        enum Position
        {
            Narrow = 1,
            Wide = 2
        };

        Position position;
        KeyPointer key;

        Tessiture sopranoTessiture;
        Tessiture altoTessiture;
        Tessiture tenorTessiture;
        Tessiture bassTessiture;

        std::map<NotePointer, std::array<NotePointer, 3>> generatedNotes;

        ChoralHarmony(const MIDI::InstrumentPointer& output, const KeyPointer& key);

        void generate(const MIDI::NoteMessagePointer& noteMessage) override;

        void handleNoteOn(const MIDI::NoteOnPointer& noteOn);
        ChordNotePointer computeSoprano(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        IntervalSequence::Modulation getIntendedModulation(const ChordPointer& chord);
        int getMinimumOffset(const ChordPointer& chord);

        void handleNoteOff(const MIDI::NoteOffPointer& noteOff);

        void getSoundingNotes(std::set<int>& soundingNotes) override;

        static std::string positionName(Position position);
    };
    using ChoralHarmonyPointer = std::shared_ptr<ChoralHarmony>;
}

#endif //MIDIASSISTANT_CHORALHARMONY_H
