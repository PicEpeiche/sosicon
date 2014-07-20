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
#include "sosi_north_east.h"

void sosicon::sosi::
deleteNorthEasts( NorthEastList& lst ) {
    for( NorthEastList::iterator i = lst.begin(); i != lst.end(); i++ ) {
        delete *i;
    }
    lst.clear();
}

sosicon::sosi::SosiOrigoNE sosicon::sosi::SosiNorthEast::mOrigo = sosicon::sosi::SosiOrigoNE();
sosicon::sosi::SosiUnit sosicon::sosi::SosiNorthEast::mUnit = sosicon::sosi::SosiUnit();

sosicon::sosi::SosiNorthEast::
SosiNorthEast( ISosiElement* e ) {
    mSosiElement = e;
    mMinX = +9999999999;
    mMinY = +9999999999;
    mMaxX = -9999999999;
    mMaxY = -9999999999;
    ragelParseCoordinates( mSosiElement->getData() );
    initHeadMember( mOrigo, sosi_element_origo_ne );
    initHeadMember( mUnit, sosi_element_unit );
    *this += mOrigo;
    *this /= mUnit;
}

sosicon::sosi::SosiNorthEast::
~SosiNorthEast() {
    for( CoordinateList::iterator i = mCoordinates.begin(); i != mCoordinates.end(); i++ ) {
        delete *i;
    }
    mCoordinates.clear();
}

void sosicon::sosi::SosiNorthEast::
initHeadMember( ISosiHeadMember& headMember, ElementType type ) {
    if( !headMember.initialized() ) {
        SosiElementSearch head;
        SosiElementSearch transpar;
        SosiElementSearch target;
        ISosiElement* root = mSosiElement->getRoot();
        if( root->getChild( head, sosi_element_head ) &&
            head.element()->getChild( transpar, sosi_element_transpar ) &&
            transpar.element()->getChild( target, type ) )
        {
            headMember.init( target.element() );
        }
    }
}

void sosicon::sosi::SosiNorthEast::
dump() {
    for( CoordinateList::iterator i = mCoordinates.begin(); i != mCoordinates.end(); i++ ) {
        std::cout << ( *i )->toString() << "\n";
    }
}

void sosicon::sosi::SosiNorthEast::
expandBoundingBox( double& minX, double& minY, double& maxX, double& maxY ) {
    minX = std::min( minX, mMinX );
    minY = std::min( minY, mMinY );
    maxX = std::max( maxX, mMaxX );
    maxY = std::max( maxY, mMaxY );
}

bool sosicon::sosi::SosiNorthEast::
getNext( ICoordinate*& coord ) {
    bool moreToGo = mCoordinates.size() > 0;
    if( moreToGo ) {
        if( 0 == coord ) {
            mCoordinatesIterator = mCoordinates.begin();
        }
        moreToGo = mCoordinatesIterator != mCoordinates.end();
        if( moreToGo ) {
            coord = *mCoordinatesIterator;
            mCoordinatesIterator++;
        }
    }
    return moreToGo;
}

sosicon::sosi::SosiNorthEast& sosicon::sosi::SosiNorthEast::
operator+= ( SosiOrigoNE& origo ) {
    int offsetN = origo.getN();
    int offsetE = origo.getE();
    ICoordinate* c = 0;
    while( getNext( c ) ) {
        c->shift( offsetN, offsetE );
    }
    mMinX += offsetE;
    mMinY += offsetN;
    mMaxX += offsetE;
    mMaxY += offsetN;
    return *this;
}

sosicon::sosi::SosiNorthEast& sosicon::sosi::SosiNorthEast::
operator/= ( SosiUnit& unit ) {
    ICoordinate* c = 0;
    int divisor = unit.getDivisor();
    while( getNext( c ) ) {
        c->divide( divisor );
    }
    mMinX /= divisor;
    mMinY /= divisor;
    mMaxX /= divisor;
    mMaxY /= divisor;
    return *this;
}