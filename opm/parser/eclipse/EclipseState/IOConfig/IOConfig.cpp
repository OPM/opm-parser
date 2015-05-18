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

#include <iterator>

#include <opm/parser/eclipse/EclipseState/IOConfig/IOConfig.hpp>
#include <opm/parser/eclipse/Deck/Deck.hpp>
#include <opm/parser/eclipse/Deck/DeckIntItem.hpp>
#include <opm/parser/eclipse/Deck/Section.hpp>


namespace Opm {

    IOConfig::IOConfig(DeckConstPtr deck, TimeMapConstPtr timemap):
        m_timemap(timemap),
        m_write_INIT_file(false),
        m_write_EGRID_file(true),
        m_UNIFOUT(false),
        m_FMTOUT(false),
        m_eclipse_input_path(""){

        restartConfig rs;
        rs.timestep  = 0;
        rs.basic     = 0;
        rs.frequency = 1;

        m_restart_output_config = std::make_shared<DynamicState<restartConfig>>(timemap, rs);

        if (Section::hasGRID(deck)) {
            std::shared_ptr<const GRIDSection> gridSection = std::make_shared<const GRIDSection>(deck);
            handleGridSection(gridSection);
        }
        if (Section::hasRUNSPEC(deck)) {
            std::shared_ptr<const RUNSPECSection> runspecSection = std::make_shared<const RUNSPECSection>(deck);
            handleRunspecSection(runspecSection);
        }
    }

    bool IOConfig::getWriteEGRIDFile() const {
        return m_write_EGRID_file;
    }

    bool IOConfig::getWriteINITFile() const {
        return m_write_INIT_file;
    }

    bool IOConfig::getWriteRestartFile(size_t timestep) const {
        bool write_restart_ts = false;

        restartConfig ts_restart_config = m_restart_output_config->get(timestep);

        switch (ts_restart_config.basic) {
            case 1: //Write restart file every timestep
                write_restart_ts = true;
                break;
            case 2: //Write restart file every timestep
                write_restart_ts = true;
                break;
            case 3: //Every n'th report time
                write_restart_ts = getWriteRestartFileFrequency(timestep, ts_restart_config.timestep, ts_restart_config.frequency);
                break;
            case 4: //First reporttime of every year, or if n > 1, n'th years
                write_restart_ts = getWriteRestartFileFrequency(timestep, ts_restart_config.timestep, ts_restart_config.frequency, true);
                break;
            case 5: //First reporttime of every month, or if n > 1, n'th months
                write_restart_ts = getWriteRestartFileFrequency(timestep, ts_restart_config.timestep, ts_restart_config.frequency, false, true);
                break;
            case 6: //Write restart file every timestep
                write_restart_ts = true;
                break;
            default:
                // do nothing
                break;
        }


        return write_restart_ts;
    }



    bool IOConfig::getWriteRestartFileFrequency(size_t timestep,
                                                size_t start_index,
                                                size_t frequency,
                                                bool   first_timesteps_years,
                                                bool   first_timesteps_months) const {
        bool write_restart_file = false;
        if (!first_timesteps_years && !first_timesteps_months) {
            write_restart_file = (((timestep-start_index) % frequency) == 0) ? true : false;
        } else {
            std::vector<size_t> timesteps;
            if (first_timesteps_years) {
                m_timemap->getFirstTimestepsYears(timesteps, start_index);
            } else {
                m_timemap->getFirstTimestepsMonths(timesteps, start_index);
            }
            std::vector<size_t>::const_iterator ci_timestep = std::find(timesteps.begin(), timesteps.end(), timestep);

            if (ci_timestep != timesteps.end()) {
                if (1 >= frequency) {
                    write_restart_file = true;
                } else {
                    std::vector<size_t>::const_iterator ci_start = timesteps.begin();
                    int index = std::distance( ci_start, ci_timestep );
                    if( (index % frequency) == 0) {
                        write_restart_file = true;
                    }
                 }
            }
        }
        return write_restart_file;
    }


    void IOConfig::handleRPTRSTBasic(size_t timestep, size_t basic, size_t frequency) {
        restartConfig rs;
        rs.timestep  = timestep;
        rs.basic     = basic;
        rs.frequency = frequency;

        m_restart_output_config->add(timestep, rs);
    }


    void IOConfig::handleGridSection(std::shared_ptr<const GRIDSection> gridSection) {
        m_write_INIT_file = gridSection->hasKeyword("INIT");

        if (gridSection->hasKeyword("GRIDFILE")) {
            auto gridfilekeyword = gridSection->getKeyword("GRIDFILE");
            if (gridfilekeyword->size() > 0) {
                auto rec = gridfilekeyword->getRecord(0);
                auto item1 = rec->getItem(0);
                if ((item1->hasValue(0)) && (item1->getInt(0) !=  0)) {
                    throw std::runtime_error("IOConfig: Reading GRIDFILE keyword from GRID section: Output of GRID file is not supported") ;
                }
                if (rec->size() > 1) {
                    auto item2 = rec->getItem(1);
                    if ((item2->hasValue(0)) && (item2->getInt(0) ==  0)) {
                        m_write_EGRID_file = false;
                    }
                }
            }
        }
        if (gridSection->hasKeyword("NOGGF")) {
            m_write_EGRID_file = false;
        }
    }


    void IOConfig::handleRunspecSection(std::shared_ptr<const RUNSPECSection> runspecSection) {
        m_FMTOUT  = runspecSection->hasKeyword("FMTOUT");  //Output files are formatted
        m_UNIFOUT = runspecSection->hasKeyword("UNIFOUT"); //Output files are to be unified
    }


    bool IOConfig::getUNIFOUT() const {
        return m_UNIFOUT;
    }

    bool IOConfig::getFMTOUT() const {
        return m_FMTOUT;
    }


    void IOConfig::setEclipseInputPath(const std::string& path) {
        m_eclipse_input_path = path;
    }

    const std::string& IOConfig::getEclipseInputPath() const {
        return m_eclipse_input_path;
    }


} //namespace Opm