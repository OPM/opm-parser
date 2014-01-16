/*
  Copyright 2013 Statoil ASA.

  This file is part of the Open Porous Media project (OPM).

  OPM is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  OPM is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with OPM.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef WELL_HPP_
#define WELL_HPP_

#include <opm/parser/eclipse/EclipseState/Schedule/TimeMap.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/DynamicState.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/CompletionSet.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Completion.hpp>

#include <memory>
#include <string>

namespace Opm {

    class Well {
    public:
        Well(const std::string& name, TimeMapConstPtr timeMap , size_t creationTimeStep);
        const std::string& name() const;

        bool hasBeenDefined(size_t timeStep) const;
        const std::string getGroupName(size_t timeStep) const;
        void setGroupName(size_t timeStep , const std::string& groupName);

        double getOilRate(size_t timeStep) const;
        void   setOilRate(size_t timeStep, double oilRate);
        double getGasRate(size_t timeStep) const;
        void   setGasRate(size_t timeStep, double gasRate);
        double getWaterRate(size_t timeStep) const;
        void   setWaterRate(size_t timeStep, double waterRate);
        double getInjectionRate(size_t timeStep) const;
        void   setInjectionRate(size_t timeStep, double injectionRate);
        int    getHeadI(size_t timeStep) const;
        void   setHeadI(size_t timeStep, int headI);
        int    getHeadJ(size_t timeStep) const;
        void   setHeadJ(size_t timeStep, int headJ);
        double getRefDepth(size_t timeStep) const;
        void   setRefDepth(size_t timeStep, double headJ);
        
        bool isInPredictionMode(size_t timeStep) const;
        void setInPredictionMode(size_t timeStep, bool isInPredictionMode);
        bool isProducer(size_t timeStep) const;
        bool isInjector(size_t timeStep) const;
        void addWELSPECS(DeckRecordConstPtr deckRecord);
        void addCompletions(size_t time_step , const std::vector<CompletionConstPtr>& newCompletions);
        CompletionSetConstPtr getCompletions(size_t timeStep);

    private:
        void switch2Producer(size_t timeStep );
        void switch2Injector(size_t timeStep );
        
        size_t m_creationTimeStep;
        std::string m_name;
        std::shared_ptr<DynamicState<double> > m_oilRate;
        std::shared_ptr<DynamicState<double> > m_gasRate;
        std::shared_ptr<DynamicState<double> > m_waterRate;
        std::shared_ptr<DynamicState<double> > m_injectionRate;
        
        std::shared_ptr<DynamicState<bool> > m_inPredictionMode;
        std::shared_ptr<DynamicState<bool> > m_isProducer;
        std::shared_ptr<DynamicState<CompletionSetConstPtr> > m_completions;
        std::shared_ptr<DynamicState<std::string> > m_groupName;

        // WELSPECS data
        std::shared_ptr<DynamicState<int> > m_headI;
        std::shared_ptr<DynamicState<int> > m_headJ;
        std::shared_ptr<DynamicState<double> > m_refDepth;
    };
    typedef std::shared_ptr<Well> WellPtr;
    typedef std::shared_ptr<const Well> WellConstPtr;
}



#endif /* WELL_HPP_ */
