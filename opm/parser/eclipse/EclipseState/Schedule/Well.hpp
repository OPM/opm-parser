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
#include <opm/parser/eclipse/EclipseState/Schedule/SegmentSet.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Segment.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/Compsegs.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/WellProductionProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/WellInjectionProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/WellPolymerProperties.hpp>
#include <opm/parser/eclipse/EclipseState/Grid/EclipseGrid.hpp>

#include <opm/common/utility/platform_dependent/disable_warnings.h>

#include <boost/optional.hpp>

#include <opm/common/utility/platform_dependent/reenable_warnings.h>

#include <memory>
#include <string>
#include <limits>

namespace Opm {



    class Well {
    public:
        Well(const std::string& name, std::shared_ptr<const EclipseGrid> grid , int headI, int headJ, Value<double> refDepth , Phase::PhaseEnum preferredPhase,
             TimeMapConstPtr timeMap, size_t creationTimeStep, WellCompletion::CompletionOrderEnum completionOrdering = WellCompletion::TRACK, bool allowCrossFlow = true);
        const std::string& name() const;

        bool hasBeenDefined(size_t timeStep) const;
        const std::string getGroupName(size_t timeStep) const;
        void setGroupName(size_t timeStep , const std::string& groupName);

        WellCommon::StatusEnum getStatus(size_t timeStep) const;
        bool                   setStatus(size_t timeStep, WellCommon::StatusEnum Status);

        int    getHeadI() const;
        int    getHeadJ() const;
        double getRefDepth() const;
        void   setRefDepth(const double ref_depth);
        Phase::PhaseEnum getPreferredPhase() const;

        bool isAvailableForGroupControl(size_t timeStep) const;
        void setAvailableForGroupControl(size_t timeStep, bool isAvailableForGroupControl);
        double getGuideRate(size_t timeStep) const;
        void setGuideRate(size_t timeStep, double guideRate);
        GuideRate::GuideRatePhaseEnum getGuideRatePhase(size_t timeStep) const;
        void setGuideRatePhase(size_t timeStep, GuideRate::GuideRatePhaseEnum phase);
        double getGuideRateScalingFactor(size_t timeStep) const;
        void setGuideRateScalingFactor(size_t timeStep, double scalingFactor);

        bool isProducer(size_t timeStep) const;
        bool isInjector(size_t timeStep) const;
        void addWELSPECS(DeckRecordConstPtr deckRecord);
        void addCompletions(size_t time_step , const std::vector<CompletionPtr>& newCompletions);
        void addCompletionSet(size_t time_step, const CompletionSetConstPtr newCompletionSet);
        CompletionSetConstPtr getCompletions(size_t timeStep) const;

        bool                            setProductionProperties(size_t timeStep , const WellProductionProperties properties);
        WellProductionProperties        getProductionPropertiesCopy(size_t timeStep) const;
        const WellProductionProperties& getProductionProperties(size_t timeStep)  const;

        bool                           setInjectionProperties(size_t timeStep , const WellInjectionProperties properties);
        WellInjectionProperties        getInjectionPropertiesCopy(size_t timeStep) const;
        const WellInjectionProperties& getInjectionProperties(size_t timeStep) const;

        bool                           setPolymerProperties(size_t timeStep , const WellPolymerProperties properties);
        WellPolymerProperties          getPolymerPropertiesCopy(size_t timeStep) const;
        const WellPolymerProperties&   getPolymerProperties(size_t timeStep) const;

        bool                           setSolventFraction(size_t timeStep , const double fraction);
        const double&                  getSolventFraction(size_t timeStep) const;

        bool getRFTActive(size_t time_step) const;
        void setRFTActive(size_t time_step, bool value);
        bool getPLTActive(size_t time_step) const;
        void setPLTActive(size_t time_step, bool value);
        int  findWellFirstOpen(int startTimeStep) const;
        void setRFTForWellWhenFirstOpen(int numSteps,size_t currentStep);

        static bool wellNameInWellNamePattern(const std::string& wellName, const std::string& wellNamePattern);

        WellCompletion::CompletionOrderEnum getWellCompletionOrdering() const;

        bool getAllowCrossFlow() const;



        // for multi-segment wells
        bool isMultiSegment() const;
        void setMultiSegment(const bool is_multi_segment);

        SegmentSetConstPtr getSegmentSet(size_t time_step) const;

        void addSegmentSet(size_t time_step, const SegmentSetPtr new_segmentset, const bool first_time);

        void addSegmentSetINC(size_t time_step, const SegmentSetPtr new_segmentset, const bool first_time);

        void addSegmentSetABS(size_t time_step, const SegmentSetPtr new_segmentset, const bool first_time);

        void processCOMPSEGS(size_t time_step, std::vector<CompsegsPtr>& compsegs);

    private:
        void setRefDepthFromCompletions() const;
        size_t m_creationTimeStep;
        std::string m_name;

        std::shared_ptr<DynamicState<WellCommon::StatusEnum> > m_status;

        std::shared_ptr<DynamicState<bool> > m_isAvailableForGroupControl;
        std::shared_ptr<DynamicState<double> > m_guideRate;
        std::shared_ptr<DynamicState<GuideRate::GuideRatePhaseEnum> > m_guideRatePhase;
        std::shared_ptr<DynamicState<double> > m_guideRateScalingFactor;

        std::shared_ptr<DynamicState<bool> > m_isProducer;
        std::shared_ptr<DynamicState<CompletionSetConstPtr> > m_completions;
        std::shared_ptr<DynamicState<WellProductionProperties> > m_productionProperties;
        std::shared_ptr<DynamicState<WellInjectionProperties> > m_injectionProperties;
        std::shared_ptr<DynamicState<WellPolymerProperties> > m_polymerProperties;
        std::shared_ptr<DynamicState<double> > m_solventFraction;
        std::shared_ptr<DynamicState<std::string> > m_groupName;
        std::shared_ptr<DynamicState<bool> > m_rft;
        std::shared_ptr<DynamicState<bool> > m_plt;

        // WELSPECS data - assumes this is not dynamic

        TimeMapConstPtr m_timeMap;
        int m_headI;
        int m_headJ;
        mutable Value<double> m_refDepth;
        Phase::PhaseEnum m_preferredPhase;
        std::shared_ptr<const EclipseGrid> m_grid;

        WellCompletion::CompletionOrderEnum m_comporder;
        bool m_allowCrossFlow;

        // WELSEGS data - for the multi-segment wells
        // assumes this is not dynamic either
        bool m_is_multi_segment; // flag for multi-segment well

        std::shared_ptr<DynamicState<SegmentSetPtr>> m_segmentset;
    };
    typedef std::shared_ptr<Well> WellPtr;
    typedef std::shared_ptr<const Well> WellConstPtr;
}



#endif /* WELL_HPP_ */
