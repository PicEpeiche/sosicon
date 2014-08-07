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
#include "shapefile.h"

sosicon::shape::ShapeType sosicon::shape::
getShapeEquivalent( sosi::ElementType sosiType ) {
    switch( sosiType ) {
        case sosi::sosi_element_curve:
        case sosi::sosi_element_surface:
            return shape_type_polyLine;
        case sosi::sosi_element_point:
        case sosi::sosi_element_text:
            return shape_type_point;
        default:
            return shape_type_none;
    }
}

sosicon::shape::Shapefile::
~Shapefile() {
    delete [ ] mShpBuffer;
    delete [ ] mShxBuffer;
    delete [ ] mDbfBuffer;
}

void sosicon::shape::Shapefile::
adjustMasterMbr( double xMin, double yMin, double xMax, double yMax ) {
    mXmin = std::min( mXmin, xMin );
    mYmin = std::min( mYmin, yMin );
    mXmax = std::max( mXmax, xMax );
    mYmax = std::max( mYmax, yMax );
}

void sosicon::shape::Shapefile::
build( ISosiElement* sosiTree, sosi::ElementType selection ) {

    ShapeType shapeTypeEquivalent = getShapeEquivalent( selection );

    if( shape_type_none != shapeTypeEquivalent ) {

        ISosiElement* sosi = 0;
        sosi::SosiElementSearch src;

        while( sosiTree->getChild( src ) ) {
            sosi = src.element();
            if( selection == sosi->getType() ) {
                buildShpElement( sosi, shapeTypeEquivalent );
                insertDbfRecord( sosi );
            }
        }

        buildDbf();
        buildShpHeader( shapeTypeEquivalent );
        buildShx();
    }
}

void sosicon::shape::Shapefile::
buildShpHeader( ShapeType type ) {

    Int32Field fileCode;
    Int32Field unused;
    Int32Field fileLength;
    Int32Field version;
    Int32Field shapeType;

    fileCode.i = 9994;
    unused.i = 0;
    fileLength.i = ( mShpSize / 2 ) + 50;
    version.i = 1000;
    shapeType.i = type;

    byteOrder::toBigEndian( fileCode.b,     &mShpHeader[  0 ], 4 );
    byteOrder::toBigEndian( unused.b,       &mShpHeader[  4 ], 4 );
    byteOrder::toBigEndian( unused.b,       &mShpHeader[  8 ], 4 );
    byteOrder::toBigEndian( unused.b,       &mShpHeader[ 12 ], 4 );
    byteOrder::toBigEndian( unused.b,       &mShpHeader[ 16 ], 4 );
    byteOrder::toBigEndian( unused.b,       &mShpHeader[ 20 ], 4 );
    byteOrder::toBigEndian( fileLength.b,   &mShpHeader[ 24 ], 4 );
    byteOrder::toLittleEndian( version.b,   &mShpHeader[ 28 ], 4 );
    byteOrder::toLittleEndian( shapeType.b, &mShpHeader[ 32 ], 4 );
    byteOrder::doubleToLittleEndian( mXmin, &mShpHeader[ 36 ] );
    byteOrder::doubleToLittleEndian( mYmin, &mShpHeader[ 44 ] );
    byteOrder::doubleToLittleEndian( mXmax, &mShpHeader[ 52 ] );
    byteOrder::doubleToLittleEndian( mYmax, &mShpHeader[ 60 ] );
    byteOrder::doubleToLittleEndian( 0.0,   &mShpHeader[ 68 ] );
    byteOrder::doubleToLittleEndian( 0.0,   &mShpHeader[ 76 ] );
    byteOrder::doubleToLittleEndian( 0.0,   &mShpHeader[ 84 ] );
    byteOrder::doubleToLittleEndian( 0.0,   &mShpHeader[ 92 ] );

}

void sosicon::shape::Shapefile::
buildShpElement( ISosiElement* sosi, ShapeType type ) {

    CoordinateCollection cc;
    cc.discoverCoords( sosi );

    switch( type ) {
    
    case shape_type_point:
        {
            int byteLength = 28;
            int contentLength = 10; // In 16-bit words, record header not included
            insertShxOffset( contentLength );
            int o = expandShpBuffer( byteLength );
            buildShpRecHeaderCommonPart( o, contentLength, type );
            buildShpRecCoordinate( o + 12, cc );
        }
        break;

    case shape_type_polyLine:
    case shape_type_polygon:
        {
            int byteLength = 52 + ( 4 ) + ( 16 * cc.getNumPointsGeom() );
            int contentLength = ( byteLength / 2 ) - 4; // Record header not included in contentLength
            insertShxOffset( contentLength );
            int o = expandShpBuffer( byteLength );
            buildShpRecHeaderCommonPart( o, contentLength, type );
            o = buildShpRecHeaderExtended( o, cc );
            o = buildShpRecHeaderOffsets( o, cc );
            o = buildShpRecCoordinates( o, cc );
        }
        break;

    default:
        ;
    }
}

void sosicon::shape::Shapefile::
buildShpRecCoordinate( int o, CoordinateCollection& cc ) {

    ICoordinate* c = 0;

    cc.getNextInGeom( c );
    if( c ) {
        buildShpRecCoordinate( o, c );
    }
}

void sosicon::shape::Shapefile::
buildShpRecCoordinate( int o, ICoordinate* c ) {

    byteOrder::doubleToLittleEndian( c->getE(), &mShpBuffer[ o ] );
    byteOrder::doubleToLittleEndian( c->getN(), &mShpBuffer[ o + 8 ] );
    adjustMasterMbr( c->getE(), c->getN(), c->getE(), c->getN() );
}

int sosicon::shape::Shapefile::
buildShpRecCoordinates( int o, CoordinateCollection& cc ) {

    std::vector<ICoordinate*> theGeom = getNormalized( cc );
    for( std::vector<ICoordinate*>::size_type i = 0; i < theGeom.size(); i++ ) {
        buildShpRecCoordinate( o, theGeom[ i ] );
        o += 16;
    }

    return o;
}

int sosicon::shape::Shapefile::
buildShpRecHeaderExtended( int o, CoordinateCollection& cc ) {

    Int32Field numParts;
    numParts.i = 1; //cc.getNumPartsGeom(); 

    Int32Field numPoints;
    numPoints.i = cc.getNumPointsGeom();

    double xMin = cc.getXmin();
    double yMin = cc.getYmin();
    double xMax = cc.getXmax();
    double yMax = cc.getYmax();

    byteOrder::doubleToLittleEndian( xMin,   &mShpBuffer[ o + 12 ] );    // Box minX
    byteOrder::doubleToLittleEndian( yMin,   &mShpBuffer[ o + 20 ] );    // Box minY
    byteOrder::doubleToLittleEndian( xMax,   &mShpBuffer[ o + 28 ] );    // Box maxX
    byteOrder::doubleToLittleEndian( yMax,   &mShpBuffer[ o + 36 ] );    // Box maxY
    byteOrder::toLittleEndian( numParts.b,   &mShpBuffer[ o + 44 ], 4 ); // NumParts
    byteOrder::toLittleEndian( numPoints.b,  &mShpBuffer[ o + 48 ], 4 ); // NumPoints

    adjustMasterMbr( xMin, yMin, xMax, yMax );

    return o + 52;
}

int sosicon::shape::Shapefile::
buildShpRecHeaderOffsets( int o, CoordinateCollection& cc ) {

    int part = -1;

    std::vector<ICoordinate*> theGeom = getNormalized( cc );
    while( cc.getNextOffsetInGeom( part ) ) {
        Int32Field offset;
        offset.i = part;
        byteOrder::toLittleEndian( offset.b,  &mShpBuffer[ o ], 4 );
        o += 4;
        break;
    }

    return o;
}

void sosicon::shape::Shapefile::
buildShpRecHeaderCommonPart( int o, int contentLength, ShapeType type ) {

    Int32Field len;
    len.i = contentLength;

    Int32Field shapeType;
    shapeType.i = type;

    Int32Field recordNumber;
    recordNumber.i = ++mRecordNumber;

    byteOrder::toBigEndian( recordNumber.b,  &mShpBuffer[ o +  0 ], 4 ); // Record serial
    byteOrder::toBigEndian( len.b, &mShpBuffer[ o +  4 ], 4 ); // Record content length
    byteOrder::toLittleEndian( shapeType.b,  &mShpBuffer[ o +  8 ], 4 ); // Shape type
}

void sosicon::shape::Shapefile::
buildDbf() {

    Int16Field recordLength;
    recordLength.i = 1; // Deleted flag == 1 byte
    for( DbfFieldLengths::iterator i = mDbfFieldLengths.begin(); i != mDbfFieldLengths.end(); i++ ) {
        recordLength.i += i->second;
    }

    mDbfBufferSize =                        

        /* Field description array */ ( mDbfFieldLengths.size() * 32 ) +
        /* Terminator              */   1 +
        /* Record structure        */ ( recordLength.i * mDbfRecordSet.size() ) +
        /* EOF                     */   1 ;

    try {
        mDbfBuffer = 0;
        mDbfBuffer = new char [ mDbfBufferSize ];
    }
    catch( ... ) {
        std::cout << "Memory allocation error\n";
        throw;
    }

    // Header
    buildDbfHeader( recordLength );

    // Field descriptor
    int o = buildDbfFieldDescriptor();

    // Terminator
    mDbfBuffer[ o++ ] = 0x0d;

    // Records
    o = buildDbfRecordSection( o, recordLength );

    // End of file
    mDbfBuffer[ o ] = 0x1a;
}

int sosicon::shape::Shapefile::
buildDbfFieldDescriptor() {
    
    int o = 0;

    for( DbfFieldLengths::iterator i = mDbfFieldLengths.begin(); i != mDbfFieldLengths.end(); i++ ) {

        std::string fieldName = i->first;
        fieldName.resize( 10, ' ' );
        const char* sz = fieldName.c_str();
        std::copy( sz, sz + 11, &mDbfBuffer[ o ] );

        // Field data type (char)
        mDbfBuffer[ o + 11 ] = 'C';

        // Field data address (N/A)
        for( int j = 12; j < 16; j++ ) {
            mDbfBuffer[ o + j ] = 0x00;
        }
        mDbfBuffer[ o + 16 ] = char( i->second );

        // Reserved or N/A
        for( int i = 17; i < 32; i++ ) {
            mDbfBuffer[ o + i ] = 0x00;
        }
        o += 32;
    }

    return o;
}

void sosicon::shape::Shapefile::
buildDbfHeader( Int16Field recordLength ) {

    Int16Field headerLength;
    headerLength.i =
        /* Fixed header size       */   sizeof( mDbfHeader ) +
        /* Field description array */ ( mDbfFieldLengths.size() * 32 ) +
        /* Terminator              */   1;

    time_t rawTime;
    struct tm* timeInfo;
    time( &rawTime );
    timeInfo = localtime( &rawTime );
    Int32Field numRecords;
    numRecords.i = mDbfRecordSet.size();

    mDbfHeader[  0 ] = 0x03;                         // Version number
    mDbfHeader[  1 ] = char( timeInfo->tm_year );    // Year of last update
    mDbfHeader[  2 ] = char( timeInfo->tm_mon + 1 ); // Month of last update
    mDbfHeader[  3 ] = char( timeInfo->tm_mday );    // Day of last update

    byteOrder::toLittleEndian( numRecords.b,    &mDbfHeader[  4 ], 4 ); // Number of records
    byteOrder::toLittleEndian( headerLength.b,  &mDbfHeader[  8 ], 2 ); // Length of header structure
    byteOrder::toLittleEndian( recordLength.b,  &mDbfHeader[ 10 ], 2 ); // Length of record

    // Reserved or N/A
    for( int i = 12; i < 32; i++ ) {
        mDbfHeader[ i ] = 0x00;
    }
}

int sosicon::shape::Shapefile::
buildDbfRecordSection( int o, Int16Field recordLength ) {

    for( DbfRecordSet::iterator i = mDbfRecordSet.begin(); i != mDbfRecordSet.end(); i++ ) {
        char* recordBuffer = 0;
        try {
            recordBuffer = new char[ recordLength.i ];
        }
        catch( ... ) {
            std::cout << "Memory allocation error\n";
        }
        int recNumber = 0;
        int fldOffset = 0;
        recordBuffer[ fldOffset++ ] = 0x20; // Record deleted flag
        DbfRecord& rec = *i;
        for( DbfFieldLengths::iterator j = mDbfFieldLengths.begin(); j != mDbfFieldLengths.end(); j++ ) {
            std::string fieldName = j->first;
            std::string fieldValue;
            if( rec.find( fieldName ) != rec.end() ) {
                fieldValue = rec[ fieldName ];
            }
            int fieldLength = mDbfFieldLengths[ fieldName ];
            fieldValue.resize( fieldLength, ' ' );
            const char* sz = fieldValue.c_str();
            std::copy( sz, sz + fieldLength, &recordBuffer[ fldOffset ] );
            fldOffset += fieldLength;
        }
        std::copy( recordBuffer, recordBuffer + recordLength.i, &mDbfBuffer[ o ] );
        delete [ ] recordBuffer;
        o += recordLength.i;
    }
    return o;
}

void sosicon::shape::Shapefile::
buildShx() {

    mShxBufferSize = 8 * mDbfRecordSet.size();
    try {
        mShxBuffer = 0;
        mShxBuffer = new char [ mShxBufferSize ];
    }
    catch( ... ) {
        std::cout << "Memory allocation error\n";
    }

    Int32Field fileLength;
    fileLength.i = ( sizeof( mShxHeader ) + mShxBufferSize ) / 2;
    std::copy( &mShpHeader[ 0 ], &mShpHeader[ 99 ], mShxHeader );
    byteOrder::toBigEndian( fileLength.b,   &mShxHeader[ 24 ], 4 );
    int o = 0;

    for( ShxOffsets::iterator i = mShxOffsets.begin(); i != mShxOffsets.end(); i++ ) {
        byteOrder::toBigEndian( ( *i ).offset.b,  &mShxBuffer[ o +  0 ], 4 ); // Offset
        byteOrder::toBigEndian( ( *i ).length.b,  &mShxBuffer[ o +  4 ], 4 ); // Length
        o += 8;
    }
}

int sosicon::shape::Shapefile::
expandShpBuffer( int byteLength ) {

    int offset = 0;

    if( 0 == mShpSize ) {
        mShpSize = byteLength;
        while( mShpBufferSize < mShpSize ) {
            mShpBufferSize += BUFFER_CHUNK_SIZE;
        }
        try {
            mShpBuffer = new char [ mShpBufferSize ];
        }
        catch( ... ) {
            std::cout << "Memory allocation error\n";
            throw;
        }
    }
    else {
        offset = mShpSize;
        mShpSize += byteLength;
        if( mShpBufferSize < mShpSize ) {
            while( mShpBufferSize < mShpSize ) {
                mShpBufferSize += BUFFER_CHUNK_SIZE;
            }
            char* oldBuffer = mShpBuffer;
            try {
                mShpBuffer = new char [ mShpBufferSize ];
            }
            catch( ... ) {
                std::cout << "Memory allocation error\n";
                throw;
            }
            std::copy( oldBuffer, oldBuffer + offset, mShpBuffer );
            delete [ ] oldBuffer;
        }
    }
    return offset;
}

void sosicon::shape::Shapefile::
extractDbfFields( ISosiElement* sosi, DbfRecord& rec ) {

    std::string field;
    std::string data;
    ISosiElement* child = 0;
    sosi::SosiElementSearch src;
    std::vector<ISosiElement*>& children = sosi->children();

    while( sosi->getChild( src ) ) {
        child = src.element();
        // No need to write coordinates to DBF
        if( child->getType() != sosi::sosi_element_ne ) {
            data = utils::trim( child->getData() );
            saveToDbf( rec, child->getName(), data );
            extractDbfFields( child, rec );
        }
    }
}

std::vector<sosicon::ICoordinate*> sosicon::shape::Shapefile::
getNormalized( CoordinateCollection& cc ) {
    std::vector<sosicon::ICoordinate*> theGeom;
    ICoordinate* c = 0;
    while( cc.getNextInGeom( c ) ) {
        theGeom.push_back( c );
    }
    if( theGeom.size() > 1 ) {
        if( theGeom[ 0 ]->rightOf( theGeom[ 1 ] ) ) {
            std::reverse( theGeom.begin(), theGeom.end() );
        }
    }
    return theGeom;
}

void sosicon::shape::Shapefile::
insertDbfRecord( ISosiElement* sosi ) {
    DbfRecord rec;

    saveToDbf( rec, "SOSI_ID", sosi->getSerial() );
    saveToDbf( rec, "TYPE", sosi->getName() );
    
    extractDbfFields( sosi, rec );
    mDbfRecordSet.push_back( rec );
}

void sosicon::shape::Shapefile::
insertShxOffset( int contentLength ) {

    ShxIndex shxIndex;
    shxIndex.offset.i = 50 + ( mShpSize / 2 );
    shxIndex.length.i = contentLength;

    mShxOffsets.push_back( shxIndex );
}

void sosicon::shape::Shapefile::
saveToDbf( DbfRecord& rec, std::string field, std::string data ) {

    int length = data.size();

    if( !data.empty() && length < 254 ) {
        if( mDbfFieldLengths.find( field ) != mDbfFieldLengths.end() ) {
            mDbfFieldLengths[ field ] = std::max( mDbfFieldLengths[ field ], length );
        }
        else {
            mDbfFieldLengths[ field ] = length;
        }
        rec[ field ] = data;
    }
}

void sosicon::shape::Shapefile::
writeShp( std::ostream &os ) {
    os.write( mShpHeader, sizeof( mShpHeader ) );
    os.write( mShpBuffer, mShpSize );
}

void sosicon::shape::Shapefile::
writeShx( std::ostream &os ) {
    os.write( mShxHeader, sizeof( mShxHeader ) );
    os.write( mShxBuffer, mShxBufferSize );
}

void sosicon::shape::Shapefile::
writeDbf( std::ostream &os ) {
    os.write( mDbfHeader, sizeof( mDbfHeader ) );
    os.write( mDbfBuffer, mDbfBufferSize );
}