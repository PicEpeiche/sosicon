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
#include "converter_sosi2shp.h"

sosicon::ConverterSosi2shp::
ConverterSosi2shp() {

}

sosicon::ConverterSosi2shp::
~ConverterSosi2shp() {

}

void sosicon::ConverterSosi2shp::
init( sosicon::CommandLine cmd ) {
    mCmd = cmd;
}

void sosicon::ConverterSosi2shp::
makeShp( ISosiElement* sosiTree ) {
    shape::Shapefile shp;
    for( std::vector<std::string>::iterator g = mCmd.mGeomTypes.begin(); g != mCmd.mGeomTypes.end(); g++ ) {
        shp.build( sosiTree, sosi::sosiNameToType( *g ) );
    }
    std::ofstream shpfs;

    shpfs.open( "test.shp", std::ios::out | std::ios::trunc | std::ios::binary );
    shpfs << *( static_cast<IShapefileShpPart*>( &shp ) );
    shpfs.close();
}

void sosicon::ConverterSosi2shp::
run() {
    for( std::vector<std::string>::iterator f = mCmd.mSourceFiles.begin(); f != mCmd.mSourceFiles.end(); f++ ) {
        Parser p;
        char ln[ 1024 ];
        std::ifstream ifs( ( *f ).c_str() );
        while( !ifs.eof() ) {
            memset( ln, 0x00, sizeof ln );
            ifs.getline( ln, sizeof ln );
            p.ragelParseSosiLine( ln );
        }
        p.complete();
        ifs.close();
        ISosiElement* root = p.getRootElement();
        makeShp( root );
    }
}

