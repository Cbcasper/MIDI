//
// Created by Casper De Smet on 30/05/2022.
//

#ifndef MIDIASSISTANT_VOICES_H
#define MIDIASSISTANT_VOICES_H

#include <map>
#include <memory>
#include <utility>

#include "../../../Theory/Key/Key.h"
#include "../../../Theory/Note/Note.h"
#include "../../../Theory/Chord/Chord.h"

namespace Music
{
    enum Voice
    {
        Soprano,
        Alto,
        Tenor,
        Bass,
        Invalid
    };

    class Voices
    {
    public:
        KeyPointer key;
        ChordPointer chord;

        NotePointer discard;
        NotePointer sopranoNote;
        NotePointer altoNote;
        NotePointer tenorNote;

        explicit Voices(const KeyPointer& key, const ChordPointer& chord);

        Voice voice(const NotePointer& note);
        NotePointer& operator[](Voice voice);
        NotePointer& operator[](Key::Degree degree);
    };
    using VoicesPointer = std::shared_ptr<Voices>;

    template<std::size_t Index>
    std::shared_ptr<Note> get(VoicesPointer& voices)
    {
        if constexpr(Index == 0) return voices->sopranoNote;
        if constexpr(Index == 1) return voices->altoNote;
        if constexpr(Index == 2) return voices->tenorNote;
    }
}

namespace std
{
    template<> struct tuple_size<Music::VoicesPointer>
    {
        static constexpr size_t value = 3;
    };

    template<size_t Index> struct tuple_element<Index, Music::VoicesPointer>:
            tuple_element<Index, tuple<Music::NotePointer, Music::NotePointer, Music::NotePointer>>
    {
    };
}


#endif //MIDIASSISTANT_VOICES_H
