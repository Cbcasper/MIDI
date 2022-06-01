//
// Created by Casper De Smet on 30/05/2022.
//

#ifndef MIDIASSISTANT_ZIP_H
#define MIDIASSISTANT_ZIP_H

#include <vector>

namespace Utilities
{
    template <class Container, class ContainerIterator, class ContainerElement>
    class ZipIterator
    {
    public:
        std::vector<ContainerIterator> iterators;
        explicit ZipIterator(std::vector<ContainerIterator> containerIterators): iterators(containerIterators) {}

        bool operator!=(ZipIterator zipIterator)
        {
            for (int i = 0; i < iterators.size(); ++i)
                if (iterators[i] == zipIterator.iterators[i])
                    return false;
            return true;
        }

        void operator++()
        {
            std::vector<ContainerIterator> newIterators;
            for (ContainerIterator iterator: iterators)
                newIterators.emplace_back(++iterator);
            iterators = newIterators;
        }

        Container operator*()
        {
            Container elements;
            std::insert_iterator<Container> insertIterator = std::inserter(elements, elements.begin());
            for (ContainerIterator iterator: iterators)
                insertIterator = *iterator;
            return elements;
        }
    };

    template <class Container, class ContainerElement>
    class Zip
    {
    public:
        std::vector<Container> containers;
        Zip(std::vector<Container> containers): containers(containers) {}

        ZipIterator<Container, typename Container::iterator, ContainerElement> begin()
        {
            std::vector<typename Container::iterator> iterators;
            for (const Container& container: containers)
                iterators.template emplace_back(container.begin());
            return ZipIterator<Container, typename Container::iterator, ContainerElement>(iterators);
        }

        ZipIterator<Container, typename Container::iterator, ContainerElement> end()
        {
            std::vector<typename Container::iterator> iterators;
            for (const Container& container: containers)
                iterators.template emplace_back(container.end());
            return ZipIterator<Container, typename Container::iterator, ContainerElement>(iterators);
        }

        ZipIterator<Container, typename Container::const_iterator, ContainerElement> begin() const
        {
            std::vector<typename Container::const_iterator> iterators;
            for (const Container& container: containers)
                iterators.template emplace_back(container.begin());
            return ZipIterator<Container, typename Container::const_iterator, ContainerElement>(iterators);
        }

        ZipIterator<Container, typename Container::const_iterator, ContainerElement> end() const
        {
            std::vector<typename Container::const_iterator> iterators;
            for (const Container& container: containers)
                iterators.template emplace_back(container.end());
            return ZipIterator<Container, typename Container::const_iterator, ContainerElement>(iterators);
        }
    };
}

#endif //MIDIASSISTANT_ZIP_H
