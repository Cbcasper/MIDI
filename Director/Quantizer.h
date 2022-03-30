//
// Created by Casper De Smet on 15/03/2022.
//

#ifndef MIDIASSISTANT_QUANTIZER_H
#define MIDIASSISTANT_QUANTIZER_H

#include <memory>

#include "../Application/Application.h"

namespace Music
{
    class Quantizer
    {
    public:
        std::shared_ptr<State::Application> applicationState;

        explicit Quantizer(const std::shared_ptr<State::Application>& applicationState);
    };
}


#endif //MIDIASSISTANT_QUANTIZER_H
