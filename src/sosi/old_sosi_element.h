/*
 *  This file is part of the command-line tool sosicon.
 *  Copyright (C) 2012  Espen Andersen
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
#ifndef __SOSI_ELEMENT_H__
#define __SOSI_ELEMENT_H__

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include "../interface/i_lookup_table.h"
#include "../interface/i_sosi_element.h"
#include "coord_sys.h"
#include "address_unit.h"
#include "cadastral_unit.h"
#include "coord_list.h"
#include "../string_utils.h"

namespace sosicon {

    //! SOSI
    namespace sosi {

        /*!
            \addtogroup sosi_elements SOSI Elements
            Implemented representation of SOSI file elements.
            @{
        */
        //! Basic SOSI element
        /*!
            Implements basic characteristics of a SOSI element. Mostly a key/value container with
            ISosiElement stubs. Other SOSI elements delegates basic functionality to this class,
            whilst taking care of more specialized tasks themselves.
         */
        class SosiElement : public ISosiElement {

            //! Mapped field values
            /*!
                String vector containing textual data for current SOSI element. Key/value pairs are
                inserted by the parser.
             */
            std::map<std::string, std::string> mData;

            //! List of field names
            /*!
                String vector containing the keys (names) of all SOSI fields associated with current
                element. This is a list of data fields that may be included in the exported format.
             */
            std::vector<std::string> mFields;

            //! List of references
            /*!
                String vector containing referenced SOSI element IDs for geometries to be added 
                to current element.
             */
            ReferenceList mReferencesAdd;

            //! List of parentizized references
            /*!
                String vector containing SOSI element IDs referenced within parenthesis. This
                represents shapes that should be subtracted (inverted) from current element.
             */
            ReferenceList mReferencesSub;

            //! Shared lookup table.
            /*!
                Lookup table for finding SOSI elements by ID.
             */
            ILookupTable* mSosiReferenceLookup;

        public:

            //! Constructor
            SosiElement();

            //! Destructor
            virtual ~SosiElement();

            //! Get next AddressUnit
            /*!
                Not implemented in this class.
                \sa ISosiElement::getData( AddressUnit*& )
            
                \param aunit Reference to pointer to AddressUnit will be set to 0.
                \return Empty string.
             */
            virtual std::string getData( AddressUnit* &aunit ) { aunit = 0; return ""; };

            //! Get next CadastralUnit
            /*!
                Not implemented in this class.
                \sa ISosiElement::getData( CadastralUnit*& )
        
                \param cunit Reference to pointer to CadastralUnit will be set to 0.
                \return Empty string.
             */
            virtual std::string getData( CadastralUnit* &cunit ) { cunit = 0; return ""; };

            //! Get next CoordList
            /*!
                Not implemented in this class.
        
                \param clist Reference to pointer to CoordList will be set to 0.
                \return Empty string.
             */
            virtual std::string getData( CoordList* &clist ) { clist = 0; return ""; };

            // Described in ISosiElement::getData( ReferenceList* )
            virtual std::string getData( ReferenceList* &rlist, bool inv = false );

            // Described in ISosiElement::getData( const char* )
            virtual std::string getData( const char* key );

            // Described in ISosiElement::getData( ILookupTable* )
            virtual std::string getData( ILookupTable* &lookup );

            // Described in ISosiElement::getFields()
            virtual std::vector<std::string>& getFields();

            // Described in ISosiElement::getType()
            virtual ElementType getType();

            // Described in ISosiElement::set( std::string, const std::string& )
            virtual void set( std::string key, const std::string& val );

            // Described in ISosiElement::set( ILookupTable* )
            virtual void set( ILookupTable* lookup );

            // Described in ISosiElement::append()
            virtual void append( std::string key, char val );

        };
       /*! @} end group sosi_elements */

    }; // namespace sosi

}; // namespace sosicon

#endif