/*
 *  This file is part of the command-line tool sosicon.
 *  Copyright (C) 2014  Espen Andersen
 *
 *  This is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __I_COORDINATE_H__
#define __I_COORDINATE_H__

#include <string>

namespace sosicon {

    /*!
        \addtogroup interfaces Interfaces
        This is a listing of generic interfaces used within sosicon.
        @{
    */
    //! Interface: Coordinate
    /*!
        \author Espen Andersen
        \copyright GNU General Public License
    */
    class ICoordinate {
    public:

        //! Destructor
        virtual ~ICoordinate() { };
        
        //! Get east coordinate
        virtual double getE() = 0;

        //! Get north coordinate
        virtual double getN() = 0;

        //! Set east coordinate
        virtual void setE( std::string coordEast ) = 0;

        //! Set north coordinate
        virtual void setN( std::string coordNorth ) = 0;

        //! Shift coordinate by specified offset
        virtual void shift( int offsetN, int offsetE ) = 0;

        //! Divide coordinate by specified divisor
        virtual void divide( int divisor ) = 0;

        //! Make string representation
        virtual std::string toString() = 0;

    }; // class ICoordinate
   /*! @} end group interfaces */
}; // namespace sosicon
#endif
