/*
 *  This file is part of the command-line tool sosicon.
 *  Copyright (C) 2014  Espen Andersen, Norwegian Broadcast Corporation (NRK)
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
#include "sosi_unit.h"

sosicon::sosi::SosiUnit::
SosiUnit() {
    mSosiElement = 0;
    mDivisor = 1;
    mInitialized = false;
}

void sosicon::sosi::SosiUnit::
init( ISosiElement* sosiElement ) {
    mSosiElement = sosiElement;
    std::string data = sosiElement->getData();
    double d = 0;
    std::strstream ss;
    ss << data;
    ss >> d;
    mDivisor = 1 / ( d > 0 ? d : 1 );
    mInitialized = true;
}
