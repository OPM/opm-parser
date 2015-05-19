/*
  Copyright 2015 Statoil ASA.

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

#ifndef OPM_IO_CONFIG_HPP
#define OPM_IO_CONFIG_HPP

#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/TimeMap.hpp>
#include <opm/parser/eclipse/EclipseState/Schedule/DynamicState.hpp>


namespace Opm {

    class IOConfig {

    public:

        IOConfig(DeckConstPtr deck);


        bool getWriteRestartFile(size_t timestep) const;
        bool getWriteEGRIDFile() const;
        bool getWriteINITFile() const;
        bool getUNIFOUT() const;
        bool getFMTOUT() const;
        const std::string& getEclipseInputPath() const;


        void setEclipseInputPath(const std::string& path);
        void handleRPTRSTBasic(TimeMapConstPtr timemap, size_t timestep, size_t basic, size_t frequency=1);


    private:

        void handleGridSection(std::shared_ptr<const GRIDSection> gridSection);
        void handleRunspecSection(std::shared_ptr<const RUNSPECSection> runspecSection);

        void initRestartOutputConfig(TimeMapConstPtr timemap);
        bool getWriteRestartFileFrequency(size_t timestep,
                                          size_t start_index,
                                          size_t frequency,
                                          bool first_timesteps_years  = false,
                                          bool first_timesteps_months = false) const;


        TimeMapConstPtr m_timemap;
        bool            m_write_INIT_file;
        bool            m_write_EGRID_file;
        bool            m_UNIFOUT;
        bool            m_FMTOUT;
        std::string     m_eclipse_input_path;


        struct restartConfig {
          size_t timestep;
          size_t basic;
          size_t frequency;
        };

        std::shared_ptr<DynamicState<restartConfig>> m_restart_output_config;

    };


    typedef std::shared_ptr<IOConfig> IOConfigPtr;
    typedef std::shared_ptr<const IOConfig> IOConfigConstPtr;

} //namespace Opm



#endif
