//
// Created by landfried on 05.01.22.
//

#ifndef OPENFPM_PDATA_INITIALCONDITION_HPP
#define OPENFPM_PDATA_INITIALCONDITION_HPP

#include <iostream>
#include "Constants.hpp"



// primary template
template <typename IC, typename ParticleMethodType, typename SimulationParametersType>
class InitialCondition_Impl {
public:
    void initialization(ParticleData<ParticleMethodType, SimulationParametersType> &particleData) {
        std::cout << "primary template" << std::endl;
    }
};

template <typename ParticleMethodType, typename SimulationParametersType>
class InitialCondition_Impl<INITIALCONDITION_MESH, ParticleMethodType, SimulationParametersType> {
public:
    void initialization(ParticleData<ParticleMethodType, SimulationParametersType> &particleData) {
        std::cout << "Mesh particle placement" << std::endl;

        // create particle mesh
        particleData.getOpenFPMContainer().clear();
        auto meshIterator = particleData.getOpenFPMContainer().getGridIterator(SimulationParametersType::meshSize);
        while (meshIterator.isNext())
        {
            particleData.getOpenFPMContainer().add();
            auto node = meshIterator.get();
            for (int i = 0; i < ParticleMethodType::spaceDimension; i++) {
                particleData.getOpenFPMContainer().getLastPos()[i] = node.get(i) * meshIterator.getSpacing(i);
            }
            ++meshIterator;
        }
    }
};

template <typename ParticleMethodType, typename SimulationParametersType>
class InitialCondition_Impl<INITIALCONDITION_RANDOM, ParticleMethodType, SimulationParametersType> {
public:
    void initialization(ParticleData<ParticleMethodType, SimulationParametersType> &particleData) {

        using ParticleSignatureType = typename ParticleMethodType::ParticleSignature;
        static constexpr int dimension = ParticleSignatureType::dimension;
        using PositionType = typename ParticleSignatureType::position;
        using PropertyType = typename ParticleSignatureType::properties;


        SimulationParametersType simulationParameters;

        std::cout << "Random particle placement" << std::endl;

        // RNG
        std::random_device rd;  // Will be used to obtain a seed for the random number engine
        std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

        std::vector<std::uniform_real_distribution<>> dis_pos_v;
        for (int i = 0; i < dimension; ++i) {
            dis_pos_v.push_back(std::uniform_real_distribution<>(simulationParameters.domainMin[i], simulationParameters.domainMax[i]));
        }
        std::normal_distribution<> dis_vel(0, .5);

        // move particles to random positions
        auto iterator = particleData.getOpenFPMContainer().getDomainIterator();
        while (iterator.isNext())
        {
            auto p = iterator.get();
            for (int i = 0; i < dimension; i++) {
                // random positions
                particleData.getOpenFPMContainer().getPos(p)[i] = dis_pos_v[i](gen);
            }
            ++iterator;
        }
    }
};

#endif //OPENFPM_PDATA_INITIALCONDITION_HPP
