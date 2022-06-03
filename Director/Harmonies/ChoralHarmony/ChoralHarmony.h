//
// Created by Casper De Smet on 28/05/2022.
//

#ifndef MIDIASSISTANT_CHORALHARMONY_H
#define MIDIASSISTANT_CHORALHARMONY_H

#include <array>

#include "Voices.h"
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

        NotePointer lastPlayedNote;
        VoicesPointer lastVoices;
        std::map<NotePointer, VoicesPointer> generatedNotes;

        ChoralHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application);

        void generate(const MIDI::NoteMessagePointer& noteMessage) override;

        void handleNoteOn(const MIDI::NoteOnPointer& noteOn);

        bool handleLinks(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        static bool checkFifthLink(Key::Degree degree);
        void handleFifthLink(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        VoicesPointer makeSecondStep(const ChordPointer& chord, const NotePointer& commonNote,
                                     const std::set<NotePointer>& otherNotes, bool up);
        void handleSecondLink(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        NotePointer makeStep(const ChordPointer& chord, const NotePointer& note, bool up);

        void handleDefaultCase(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        ChordNotePointer computeSoprano(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        static int getIntendedOctave(const ChordPointer& chord);
        int getMinimumOffset(const ChordPointer& chord);

        void playVoices(const VoicesPointer& voices, const MIDI::NoteOnPointer& noteOn);
        void handleNoteOff(const MIDI::NoteOffPointer& noteOff);

        void getSoundingNotes(std::set<int>& soundingNotes) override;
        static std::string positionName(Position position);

    };
    using ChoralHarmonyPointer = std::shared_ptr<ChoralHarmony>;
}

#endif //MIDIASSISTANT_CHORALHARMONY_H
