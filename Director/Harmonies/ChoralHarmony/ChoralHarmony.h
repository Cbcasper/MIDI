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
    // Harmony that generates three voices according to a choral position
    class ChoralHarmony: public Harmony
    {
    public:
        enum Position
        {
            Narrow = 1,
            Wide = 2
        };

        // Parameters for generation
        Position position;
        KeyPointer key;

        // Ranges different voices are in
        Tessiture sopranoTessiture;
        Tessiture altoTessiture;
        Tessiture tenorTessiture;
        Tessiture bassTessiture;

        // Keep track of which notes were played and generated previously in order to properly generate new notes
        NotePointer lastPlayedNote;
        VoicesPointer lastVoices;
        std::map<NotePointer, VoicesPointer> generatedNotes;

        ChoralHarmony(const MIDI::InstrumentPointer& output, const State::ApplicationPointer& application);

        // Inherited method called in superclass in order to generate new notes
        void generate(const MIDI::NoteMessagePointer& noteMessage) override;

        // Generate three note on messages when one comes in
        void handleNoteOn(const MIDI::NoteOnPointer& noteOn);

        // Check whether a link exist between the previously played note and the current one
        bool handleLinks(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        static bool checkFifthLink(Key::Degree degree);
        // Fifth link
        void handleFifthLink(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        VoicesPointer makeSecondStep(const ChordPointer& chord, const NotePointer& commonNote,
                                     const std::set<NotePointer>& otherNotes, bool up);
        // Second link
        void handleSecondLink(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        NotePointer makeStep(const ChordPointer& chord, const NotePointer& note, bool up);

        // Handle a single played note, not linked to any other note
        void handleDefaultCase(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        ChordNotePointer computeSoprano(const MIDI::NoteOnPointer& noteOn, const ChordPointer& chord);
        static int getIntendedOctave(const ChordPointer& chord);
        int getMinimumOffset(const ChordPointer& chord);

        // Finish off played note by generating the note off messages of the previously generated notes
        void handleNoteOff(const MIDI::NoteOffPointer& noteOff);
        void playVoices(const VoicesPointer& voices, const MIDI::NoteOnPointer& noteOn);

        void getSoundingNotes(std::set<int>& soundingNotes) override;
        static std::string positionName(Position position);
    };
    using ChoralHarmonyPointer = std::shared_ptr<ChoralHarmony>;
}

#endif //MIDIASSISTANT_CHORALHARMONY_H
