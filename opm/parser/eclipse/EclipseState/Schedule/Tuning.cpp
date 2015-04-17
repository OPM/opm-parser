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


#include <opm/parser/eclipse/EclipseState/Schedule/Tuning.hpp>
#include <opm/parser/eclipse/Units/ConversionFactors.hpp>

namespace Opm {

    Tuning::Tuning(TimeMapConstPtr timemap):
        /* Hardcoding default values to support getting defaults before any TUNING keyword has occured */
        m_TSINIT( new DynamicState<double>(timemap, 1.0   * Metric::Time)),
        m_TSMAXZ( new DynamicState<double>(timemap, 365.0 * Metric::Time)),
        m_TSMINZ( new DynamicState<double>(timemap, 0.1   * Metric::Time)),
        m_TSMCHP( new DynamicState<double>(timemap, 0.15  * Metric::Time)),
        m_TSFMAX( new DynamicState<double>(timemap, 3.0)),
        m_TSFMIN( new DynamicState<double>(timemap, 0.3)),
        m_TSFCNV( new DynamicState<double>(timemap, 0.1)),
        m_TFDIFF( new DynamicState<double>(timemap, 1.25)),
        m_THRUPT( new DynamicState<double>(timemap, 1.0E20)),
        m_TMAXWC( new DynamicState<double>(timemap, 0.0   * Metric::Time)),
        m_TMAXWC_has_value( new DynamicState<bool>(timemap, false)),
        m_TRGTTE( new DynamicState<double>(timemap, 0.1)),
        m_TRGCNV( new DynamicState<double>(timemap, 0.001)),
        m_TRGMBE( new DynamicState<double>(timemap, 1.0E-7)),
        m_TRGLCV( new DynamicState<double>(timemap, 0.0001)),
        m_XXXTTE( new DynamicState<double>(timemap, 10.0)),
        m_XXXCNV( new DynamicState<double>(timemap, 0.01)),
        m_XXXMBE( new DynamicState<double>(timemap, 1.0E-6)),
        m_XXXLCV( new DynamicState<double>(timemap, 0.001)),
        m_XXXWFL( new DynamicState<double>(timemap, 0.001)),
        m_TRGFIP( new DynamicState<double>(timemap, 0.025)),
        m_TRGSFT( new DynamicState<double>(timemap, 0.0)),
        m_TRGSFT_has_value( new DynamicState<bool>(timemap, false)),
        m_THIONX( new DynamicState<double>(timemap, 0.01)),
        m_TRWGHT( new DynamicState<int>(timemap, 1)),
        m_NEWTMX( new DynamicState<int>(timemap, 12)),
        m_NEWTMN( new DynamicState<int>(timemap, 1)),
        m_LITMAX( new DynamicState<int>(timemap, 25)),
        m_LITMIN( new DynamicState<int>(timemap, 1)),
        m_MXWSIT( new DynamicState<int>(timemap, 8)),
        m_MXWPIT( new DynamicState<int>(timemap, 8)),
        m_DDPLIM( new DynamicState<double>(timemap, 1.0E6 * Metric::Pressure)),
        m_DDSLIM( new DynamicState<double>(timemap, 1.0E6)),
        m_TRGDPR( new DynamicState<double>(timemap, 1.0E6 * Metric::Pressure)),
        m_XXXDPR( new DynamicState<double>(timemap, 0.0   * Metric::Pressure)),
        m_XXXDPR_has_value( new DynamicState<bool>(timemap, false))
    {
    }


    void Tuning::getTuningItemValue(const std::string& tuningItem, size_t timestep, double& value) {
        /*The following code diverges from coding standard to improve readabillity*/
        if        ("TSINIT" == tuningItem)  {  value = m_TSINIT->get(timestep); }  //RECORD 1
        else if   ("TSMAXZ" == tuningItem)  {  value = m_TSMAXZ->get(timestep); }
        else if   ("TSMINZ" == tuningItem)  {  value = m_TSMINZ->get(timestep); }
        else if   ("TSMCHP" == tuningItem)  {  value = m_TSMCHP->get(timestep); }
        else if   ("TSFMAX" == tuningItem)  {  value = m_TSFMAX->get(timestep); }
        else if   ("TSFMIN" == tuningItem)  {  value = m_TSFMIN->get(timestep); }
        else if   ("TSFCNV" == tuningItem)  {  value = m_TSFCNV->get(timestep); }
        else if   ("TFDIFF" == tuningItem)  {  value = m_TFDIFF->get(timestep); }
        else if   ("THRUPT" == tuningItem)  {  value = m_THRUPT->get(timestep); }
        else if   ("TMAXWC" == tuningItem)  {  value = m_TMAXWC->get(timestep); }

        else if   ("TRGTTE" == tuningItem)  {  value = m_TRGTTE->get(timestep); }  //RECORD 2
        else if   ("TRGCNV" == tuningItem)  {  value = m_TRGCNV->get(timestep); }
        else if   ("TRGMBE" == tuningItem)  {  value = m_TRGMBE->get(timestep); }
        else if   ("TRGLCV" == tuningItem)  {  value = m_TRGLCV->get(timestep); }
        else if   ("XXXTTE" == tuningItem)  {  value = m_XXXTTE->get(timestep); }
        else if   ("XXXCNV" == tuningItem)  {  value = m_XXXCNV->get(timestep); }
        else if   ("XXXMBE" == tuningItem)  {  value = m_XXXMBE->get(timestep); }
        else if   ("XXXLCV" == tuningItem)  {  value = m_XXXLCV->get(timestep); }
        else if   ("XXXWFL" == tuningItem)  {  value = m_XXXWFL->get(timestep); }
        else if   ("TRGFIP" == tuningItem)  {  value = m_TRGFIP->get(timestep); }
        else if   ("TRGSFT" == tuningItem)  {  value = m_TRGSFT->get(timestep); }
        else if   ("THIONX" == tuningItem)  {  value = m_THIONX->get(timestep); }

        else if   ("DDPLIM" == tuningItem)  {  value = m_DDPLIM->get(timestep); }  //RECORD 3
        else if   ("DDSLIM" == tuningItem)  {  value = m_DDSLIM->get(timestep); }
        else if   ("TRGDPR" == tuningItem)  {  value = m_TRGDPR->get(timestep); }
        else if   ("XXXDPR" == tuningItem)  {  value = m_XXXDPR->get(timestep); }

        else {
            throw std::invalid_argument("Method getTuningItemValue(): The TUNING keyword item: " + tuningItem + " was not recognized or has wrong type");
        }
    }



    void Tuning::getTuningItemValue(const std::string& tuningItem, size_t timestep, int& value) {

        /*The following code diverges from coding standard to improve readabillity*/
        if        ("TRWGHT" == tuningItem)  {  value = m_TRWGHT->get(timestep); }  //RECORD 2

        else if   ("NEWTMX" == tuningItem)  {  value = m_NEWTMX->get(timestep); }  //RECORD 3
        else if   ("NEWTMN" == tuningItem)  {  value = m_NEWTMN->get(timestep); }
        else if   ("LITMAX" == tuningItem)  {  value = m_LITMAX->get(timestep); }
        else if   ("LITMIN" == tuningItem)  {  value = m_LITMIN->get(timestep); }
        else if   ("MXWSIT" == tuningItem)  {  value = m_MXWSIT->get(timestep); }
        else if   ("MXWPIT" == tuningItem)  {  value = m_MXWPIT->get(timestep); }

        else {
            throw std::invalid_argument("Method getTuningItemValue(): The TUNING keyword item: " + tuningItem + " was not recognized or has wrong type");
        }
    }



    void Tuning::updateDefaults(const std::string& keyworditem, double value) {

        /*The following code diverges from coding standard to improve readabillity*/
        if      ("TSINIT" == keyworditem) {  m_TSINIT->updateDefaults(value); }  // RECORD 1
        else if ("TSMAXZ" == keyworditem) {  m_TSMAXZ->updateDefaults(value); }
        else if ("TSMINZ" == keyworditem) {  m_TSMINZ->updateDefaults(value); }
        else if ("TSMCHP" == keyworditem) {  m_TSMCHP->updateDefaults(value); }
        else if ("TSFMAX" == keyworditem) {  m_TSFMAX->updateDefaults(value); }
        else if ("TSFMIN" == keyworditem) {  m_TSFMIN->updateDefaults(value); }
        else if ("TSFCNV" == keyworditem) {  m_TSFCNV->updateDefaults(value); }
        else if ("TFDIFF" == keyworditem) {  m_TFDIFF->updateDefaults(value); }
        else if ("THRUPT" == keyworditem) {  m_THRUPT->updateDefaults(value); }
        else if ("TMAXWC" == keyworditem) {  m_TMAXWC->updateDefaults(value); }
        else if ("TRGTTE" == keyworditem)  { m_TRGTTE->updateDefaults(value); }  // RECORD 2
        else if ("TRGCNV" == keyworditem) {  m_TRGCNV->updateDefaults(value); }
        else if ("TRGMBE" == keyworditem) {  m_TRGMBE->updateDefaults(value); }
        else if ("TRGLCV" == keyworditem) {  m_TRGLCV->updateDefaults(value); }
        else if ("XXXTTE" == keyworditem) {  m_XXXTTE->updateDefaults(value); }
        else if ("XXXCNV" == keyworditem) {  m_XXXCNV->updateDefaults(value); }
        else if ("XXXMBE" == keyworditem) {  m_XXXMBE->updateDefaults(value); }
        else if ("XXXLCV" == keyworditem) {  m_XXXLCV->updateDefaults(value); }
        else if ("XXXWFL" == keyworditem) {  m_XXXWFL->updateDefaults(value); }
        else if ("TRGFIP" == keyworditem) {  m_TRGFIP->updateDefaults(value); }
        else if ("TRGSFT" == keyworditem) {  m_TRGSFT->updateDefaults(value); }
        else if ("THIONX" == keyworditem) {  m_THIONX->updateDefaults(value); }
        else if ("NEWTMX" == keyworditem) {  m_NEWTMX->updateDefaults(value); }  //  RECORD 3
        else if ("NEWTMN" == keyworditem) {  m_NEWTMN->updateDefaults(value); }
        else if ("LITMAX" == keyworditem) {  m_LITMAX->updateDefaults(value); }
        else if ("LITMIN" == keyworditem) {  m_LITMIN->updateDefaults(value); }
        else if ("MXWSIT" == keyworditem) {  m_MXWSIT->updateDefaults(value); }
        else if ("MXWPIT" == keyworditem) {  m_MXWPIT->updateDefaults(value); }
        else {
            throw std::invalid_argument("Method updateDefaults(): The TUNING keyword item: " + keyworditem + " was not recognized or has wrong type");
        }
    }


    void Tuning::updateDefaults(const std::string& keyworditem, int value) {

        /*The following code diverges from coding standard to improve readabillity*/
        if      ("TRWGHT" == keyworditem) { m_TRWGHT->updateDefaults(value); } // RECORD 2
        else if ("DDPLIM" == keyworditem) { m_DDPLIM->updateDefaults(value); } // RECORD 3
        else if ("DDSLIM" == keyworditem) { m_DDSLIM->updateDefaults(value); }
        else if ("TRGDPR" == keyworditem) { m_TRGDPR->updateDefaults(value); }
        else if ("XXXDPR" == keyworditem) { m_XXXDPR->updateDefaults(value); }
        else {
            throw std::invalid_argument("Method updateDefaults(): The TUNING keyword item: " + keyworditem + " was not recognized or has wrong type");
        }
    }


    /*The following "get" method declarations diverges from coding standard to improve readability*/
    double Tuning::getTSINIT(size_t timestep) const {  return m_TSINIT->get(timestep); }
    double Tuning::getTSMAXZ(size_t timestep) const {  return m_TSMAXZ->get(timestep); }
    double Tuning::getTSMINZ(size_t timestep) const {  return m_TSMINZ->get(timestep); }
    double Tuning::getTSMCHP(size_t timestep) const {  return m_TSMCHP->get(timestep); }
    double Tuning::getTSFMAX(size_t timestep) const {  return m_TSFMAX->get(timestep); }
    double Tuning::getTSFMIN(size_t timestep) const {  return m_TSFMIN->get(timestep); }
    double Tuning::getTSFCNV(size_t timestep) const {  return m_TSFCNV->get(timestep); }
    double Tuning::getTFDIFF(size_t timestep) const {  return m_TFDIFF->get(timestep); }
    double Tuning::getTHRUPT(size_t timestep) const {  return m_THRUPT->get(timestep); }
    double Tuning::getTMAXWC(size_t timestep) const {  return m_TMAXWC->get(timestep); }
    bool   Tuning::getTMAXWChasValue(size_t timestep) const { return m_TMAXWC_has_value->get(timestep); }
    double Tuning::getTRGTTE(size_t timestep) const {  return m_TRGTTE->get(timestep); }
    double Tuning::getTRGCNV(size_t timestep) const {  return m_TRGCNV->get(timestep); }
    double Tuning::getTRGMBE(size_t timestep) const {  return m_TRGMBE->get(timestep); }
    double Tuning::getTRGLCV(size_t timestep) const {  return m_TRGLCV->get(timestep); }
    double Tuning::getXXXTTE(size_t timestep) const {  return m_XXXTTE->get(timestep); }
    double Tuning::getXXXCNV(size_t timestep) const {  return m_XXXCNV->get(timestep); }
    double Tuning::getXXXMBE(size_t timestep) const {  return m_XXXMBE->get(timestep); }
    double Tuning::getXXXLCV(size_t timestep) const {  return m_XXXLCV->get(timestep); }
    double Tuning::getXXXWFL(size_t timestep) const {  return m_XXXWFL->get(timestep); }
    double Tuning::getTRGFIP(size_t timestep) const {  return m_TRGFIP->get(timestep); }
    double Tuning::getTRGSFT(size_t timestep) const {  return m_TRGSFT->get(timestep); }
    bool   Tuning::getTRGSFThasValue(size_t timestep) const { return m_TRGSFT_has_value->get(timestep);  }
    double Tuning::getTHIONX(size_t timestep) const {  return m_THIONX->get(timestep); }
    int    Tuning::getTRWGHT(size_t timestep) const {  return m_TRWGHT->get(timestep); }
    int    Tuning::getNEWTMX(size_t timestep) const {  return m_NEWTMX->get(timestep); }
    int    Tuning::getNEWTMN(size_t timestep) const {  return m_NEWTMN->get(timestep); }
    int    Tuning::getLITMAX(size_t timestep) const {  return m_LITMAX->get(timestep); }
    int    Tuning::getLITMIN(size_t timestep) const {  return m_LITMIN->get(timestep); }
    int    Tuning::getMXWSIT(size_t timestep) const {  return m_MXWSIT->get(timestep); }
    int    Tuning::getMXWPIT(size_t timestep) const {  return m_MXWPIT->get(timestep); }
    double Tuning::getDDPLIM(size_t timestep) const {  return m_DDPLIM->get(timestep); }
    double Tuning::getDDSLIM(size_t timestep) const {  return m_DDSLIM->get(timestep); }
    double Tuning::getTRGDPR(size_t timestep) const {  return m_TRGDPR->get(timestep); }
    double Tuning::getXXXDPR(size_t timestep) const {  return m_XXXDPR->get(timestep); }
    bool   Tuning::getXXXDPRhasValue(size_t timestep) const { return m_XXXDPR_has_value->get(timestep);
    }

    /*The following "set" method declarations diverges from coding standard to improve readability*/
    void Tuning::setTSINIT(size_t timestep, double TSINIT) { m_TSINIT->add(timestep, TSINIT); }
    void Tuning::setTSMAXZ(size_t timestep, double TSMAXZ) { m_TSMAXZ->add(timestep, TSMAXZ); }
    void Tuning::setTSMINZ(size_t timestep, double TSMINZ) { m_TSMINZ->add(timestep, TSMINZ); }
    void Tuning::setTSMCHP(size_t timestep, double TSMCHP) { m_TSMCHP->add(timestep, TSMCHP); }
    void Tuning::setTSFMAX(size_t timestep, double TSFMAX) { m_TSFMAX->add(timestep, TSFMAX); }
    void Tuning::setTSFMIN(size_t timestep, double TSFMIN) { m_TSFMIN->add(timestep, TSFMIN); }
    void Tuning::setTSFCNV(size_t timestep, double TSFCNV) { m_TSFCNV->add(timestep, TSFCNV); }
    void Tuning::setTFDIFF(size_t timestep, double TFDIFF) { m_TFDIFF->add(timestep, TFDIFF); }
    void Tuning::setTHRUPT(size_t timestep, double THRUPT) { m_THRUPT->add(timestep, THRUPT); }
    void Tuning::setTMAXWC(size_t timestep, double TMAXWC) { m_TMAXWC->add(timestep, TMAXWC); m_TMAXWC_has_value->add(timestep, true); }
    void Tuning::setTRGTTE(size_t timestep, double TRGTTE) { m_TRGTTE->add(timestep, TRGTTE); }
    void Tuning::setTRGCNV(size_t timestep, double TRGCNV) { m_TRGCNV->add(timestep, TRGCNV); }
    void Tuning::setTRGMBE(size_t timestep, double TRGMBE) { m_TRGMBE->add(timestep, TRGMBE); }
    void Tuning::setTRGLCV(size_t timestep, double TRGLCV) { m_TRGLCV->add(timestep, TRGLCV); }
    void Tuning::setXXXTTE(size_t timestep, double XXXTTE) { m_XXXTTE->add(timestep, XXXTTE); }
    void Tuning::setXXXCNV(size_t timestep, double XXXCNV) { m_XXXCNV->add(timestep, XXXCNV); }
    void Tuning::setXXXMBE(size_t timestep, double XXXMBE) { m_XXXMBE->add(timestep, XXXMBE); }
    void Tuning::setXXXLCV(size_t timestep, double XXXLCV) { m_XXXLCV->add(timestep, XXXLCV); }
    void Tuning::setXXXWFL(size_t timestep, double XXXWFL) { m_XXXWFL->add(timestep, XXXWFL); }
    void Tuning::setTRGFIP(size_t timestep, double TRGFIP) { m_TRGFIP->add(timestep, TRGFIP); }
    void Tuning::setTRGSFT(size_t timestep, double TRGSFT) { m_TRGSFT->add(timestep, TRGSFT); m_TRGSFT_has_value->add(timestep, true); }
    void Tuning::setTHIONX(size_t timestep, double THIONX) { m_THIONX->add(timestep, THIONX); }
    void Tuning::setTRWGHT(size_t timestep, int TRWGHT) {    m_TRWGHT->add(timestep, TRWGHT); }
    void Tuning::setNEWTMX(size_t timestep, int NEWTMX) {    m_NEWTMX->add(timestep, NEWTMX); }
    void Tuning::setNEWTMN(size_t timestep, int NEWTMN) {    m_NEWTMN->add(timestep, NEWTMN); }
    void Tuning::setLITMAX(size_t timestep, int LITMAX) {    m_LITMAX->add(timestep, LITMAX); }
    void Tuning::setLITMIN(size_t timestep, int LITMIN) {    m_LITMIN->add(timestep, LITMIN); }
    void Tuning::setMXWSIT(size_t timestep, int MXWSIT) {    m_MXWSIT->add(timestep, MXWSIT); }
    void Tuning::setMXWPIT(size_t timestep, int MXWPIT) {    m_MXWPIT->add(timestep, MXWPIT); }
    void Tuning::setDDPLIM(size_t timestep, double DDPLIM) { m_DDPLIM->add(timestep, DDPLIM); }
    void Tuning::setDDSLIM(size_t timestep, double DDSLIM) { m_DDSLIM->add(timestep, DDSLIM); }
    void Tuning::setTRGDPR(size_t timestep, double TRGDPR) { m_TRGDPR->add(timestep, TRGDPR); }
    void Tuning::setXXXDPR(size_t timestep, double XXXDPR) { m_XXXDPR->add(timestep, XXXDPR); m_XXXDPR_has_value->add(timestep, true); }

} //namespace Opm





