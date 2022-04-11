//
// Created by Casper De Smet on 09/04/2022.
//

#include "Key.h"

#include "MelodicMinor.h"

namespace Music
{
    Key::Key()
    {
        rootNote = std::make_shared<RootNote>(RootNote::C, false);
        intervalSequence = std::make_shared<IntervalSequence>(Scale::Major);
        intervalSequence->apply(rootNote);
    }

    void Key::setRootNote(RootNote::Name name, bool sharp)
    {
        rootNote = std::make_shared<RootNote>(name, sharp);
        intervalSequence->apply(rootNote);
    }

    void Key::setIntervalSequence(IntervalSequence::Type type)
    {
        switch (type)
        {
            case Scale::MelodicMinor:
                intervalSequence = std::make_shared<MelodicMinor>();
                break;
            default:
                intervalSequence = std::make_shared<IntervalSequence>(type);
                break;
        }
        intervalSequence->apply(rootNote);
    }
}