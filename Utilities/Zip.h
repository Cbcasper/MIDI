//
// Created by Casper De Smet on 30/05/2022.
//

#ifndef MIDIASSISTANT_ZIP_H
#define MIDIASSISTANT_ZIP_H

#include <vector>

namespace Utilities
{
    // Iterator for Zip class
    template <class Container, class ContainerIterator, class ContainerElement>
    class ZipIterator
    {
    public:
        std::vector<ContainerIterator> iterators;
        explicit ZipIterator(std::vector<ContainerIterator> containerIterators): iterators(containerIterators) {}

        // Check all iterators pairwise
        bool operator!=(ZipIterator zipIterator)
        {
            for (int i = 0; i < iterators.size(); ++i)
                if (iterators[i] == zipIterator.iterators[i])
                    return false;
            return true;
        }

        // Increment all iterators separately
        void operator++()
        {
            std::vector<ContainerIterator> newIterators;
            for (ContainerIterator iterator: iterators)
                newIterators.emplace_back(++iterator);
            iterators = newIterators;
        }

        // Return the de-referenced iterators in a container, the same type as the original containers
        Container operator*()
        {
            Container elements;
            std::insert_iterator<Container> insertIterator = std::inserter(elements, elements.begin());
            for (ContainerIterator iterator: iterators)
                insertIterator = *iterator;
            return elements;
        }
    };

    // Class that takes a number of containers and makes it possible to iterate over them simultaneously
    template <class Container, class ContainerElement>
    class Zip
    {
    public:
        std::vector<Container> containers;
        Zip(std::vector<Container> containers): containers(containers) {}

        // Return a ZipIterator containing all begin iterators of the containers
        ZipIterator<Container, typename Container::iterator, ContainerElement> begin()
        {
            std::vector<typename Container::iterator> iterators;
            for (const Container& container: containers)
                iterators.template emplace_back(container.begin());
            return ZipIterator<Container, typename Container::iterator, ContainerElement>(iterators);
        }

        // Return a ZipIterator containing all end iterators of the containers
        ZipIterator<Container, typename Container::iterator, ContainerElement> end()
        {
            std::vector<typename Container::iterator> iterators;
            for (const Container& container: containers)
                iterators.template emplace_back(container.end());
            return ZipIterator<Container, typename Container::iterator, ContainerElement>(iterators);
        }

        // Return a ZipIterator containing all const begin iterators of the containers
        ZipIterator<Container, typename Container::const_iterator, ContainerElement> begin() const
        {
            std::vector<typename Container::const_iterator> iterators;
            for (const Container& container: containers)
                iterators.template emplace_back(container.begin());
            return ZipIterator<Container, typename Container::const_iterator, ContainerElement>(iterators);
        }

        // Return a ZipIterator containing all const end iterators of the containers
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
