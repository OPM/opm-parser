/*
  Copyright 2016 Statoil ASA.

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

#ifndef OPM_MESSAGES_HPP
#define OPM_MESSAGES_HPP

#include <opm/parser/eclipse/EclipseState/Schedule/DynamicState.hpp>
#include <opm/parser/eclipse/Parser/ParserKeywords/M.hpp>

namespace Opm {

    class TimeMap;

    struct MLimits {
        int message_print_limit = 1000000;
        int comment_print_limit = 1000000;
        int warning_print_limit = 10000;
        int problem_print_limit = 100;
        int error_print_limit   = 100;
        int bug_print_limit     = 100;
        int message_stop_limit  = 1000000;
        int comment_stop_limit  = 1000000;
        int warning_stop_limit  = 10000;
        int problem_stop_limit  = 100;
        int error_stop_limit    = 10;
        int bug_stop_limit      = 1;


        bool operator==(const MLimits& other) const {
            return  ((this->message_print_limit == other.message_print_limit) &&
                     (this->comment_print_limit == other.comment_print_limit) &&
                     (this->warning_print_limit == other.warning_print_limit) &&
                     (this->problem_print_limit == other.problem_print_limit) &&
                     (this->error_print_limit   == other.error_print_limit  ) &&
                     (this->bug_print_limit     == other.bug_print_limit    ) &&
                     (this->message_stop_limit  == other.message_stop_limit ) &&
                     (this->comment_stop_limit  == other.comment_stop_limit ) &&
                     (this->warning_stop_limit  == other.warning_stop_limit ) &&
                     (this->problem_stop_limit  == other.problem_stop_limit ) &&
                     (this->error_stop_limit    == other.error_stop_limit   ) &&
                     (this->bug_stop_limit      == other.bug_stop_limit     ));
        }

        bool operator!=(const MLimits& other) const {
            return !(*this == other);
        }

    };


    class MessageLimits {
    public:

        /*
           This constructor will create a new Messages object which is
           a copy of the input argument, and then all items explicitly
           set in the record are modified.
        */

        MessageLimits( const TimeMap& );

        ///Get all the value from MESSAGES keyword.
        int getMessagePrintLimit(size_t timestep) const;
        int getCommentPrintLimit(size_t timestep) const;
        int getWarningPrintLimit(size_t timestep) const;
        int getProblemPrintLimit(size_t timestep) const;
        int getErrorPrintLimit(size_t timestep) const;
        int getBugPrintLimit(size_t timestep) const;
        void setMessagePrintLimit(size_t timestep, int value);
        void setCommentPrintLimit(size_t timestep, int value);
        void setWarningPrintLimit(size_t timestep, int value);
        void setProblemPrintLimit(size_t timestep, int value);
        void setErrorPrintLimit(size_t timestep, int value);
        void setBugPrintLimit(size_t timestep, int value);

        int getMessageStopLimit(size_t timestep) const;
        int getCommentStopLimit(size_t timestep) const;
        int getWarningStopLimit(size_t timestep) const;
        int getProblemStopLimit(size_t timestep) const;
        int getErrorStopLimit(size_t timestep) const;
        int getBugStopLimit(size_t timestep) const;
        void setMessageStopLimit(size_t timestep, int value);
        void setCommentStopLimit(size_t timestep, int value);
        void setWarningStopLimit(size_t timestep, int value);
        void setProblemStopLimit(size_t timestep, int value);
        void setErrorStopLimit(size_t timestep, int value);
        void setBugStopLimit(size_t timestep, int value);

    private:
        void update(size_t timestep, const MLimits& value);

        DynamicState<MLimits> limits;
    };
}

#endif
