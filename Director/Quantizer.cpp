//
// Created by Casper De Smet on 15/03/2022.
//

#include "Quantizer.h"

namespace Music
{
    Quantizer::Quantizer(const std::shared_ptr<State::Application>& applicationState):
                         applicationState(applicationState)
    {}
}
