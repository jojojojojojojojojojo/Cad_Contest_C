#include "placerrow.h"
#include "placer.h"

#include <cfloat>

namespace rowpl {
//#define DEBUG_ROWPL
RowPlacer::RowPlacer(Circuit *cir): _cir( cir )
{
    initRowStructures(false);
}

bool RowPlacer::put_legal_pos_for_mod(Module &mod, Point &pos)
{
    //cout << "put legal " << mod.name() << " " << mod.width() << " to " << pos << endl;
    delete_modInRow( mod, true );

    if( !is_pos_legal_for_mod( mod, pos ) ) return false;

    set_mod_rowM( mod, pos );

    return true;
}

// Procedure: find_legal_pos_for_mod
// Find a closest legal position for the module and
// update the row data structure in a legal placement
bool RowPlacer::find_legal_pos_for_mod(Module &mod, Point &pos, ModR *obsModR)
{
    assert( mod.numRects()==1 );

    //_cir->bound_pos_in_max_disp( mod, pos, true );

    int targRowId = _cir->y_2_row_id( pos.y() );
    double xOnSiteStep = _cir->g_x_on_site( pos.x(), targRowId, Circuit::ALIGN_HERE );
    pos.set_x_y( xOnSiteStep, pos.y() );

    if( !delete_modInRow( mod, false ) ) return false;
    if( is_pos_legal_for_mod(mod, pos, obsModR) )
    {
        set_mod_rowM( mod, pos ); return true;
    }
    set_mod_rowM( mod, Point(mod.pos()) );

    int numRowWindows = 1;
    int lRowId = max(0, targRowId-numRowWindows),
        hRowId = min( int(_cir->numRows())-1, targRowId+numRowWindows );

    Point tmpTargPos = pos, bestTargPos = Point(-1,-1);
    ModR *tmpObsModR = NULL;
    double bestDisp = MAX_DOUBLE;
    int bestRId = MAX_INT;
    for( int curRId=lRowId; curRId<=hRowId; ++curRId )
    {
        tmpTargPos.set_x_y( pos.x(), _cir->row_id_2_y( curRId ) );

        tmpObsModR = ( curRId==targRowId ) ? obsModR : NULL;

        if( find_legal_pos_for_mod_row( mod, tmpTargPos, tmpObsModR ) ) // change tmpTargPos & rowStructure
        {
            mod.setPosition( tmpTargPos.x(), tmpTargPos.y() );
            if( tmpTargPos!=pos && Point::dist( tmpTargPos, pos ) < bestDisp )
            {
                bestTargPos = tmpTargPos;
                bestDisp = Point::dist( tmpTargPos, pos );
                bestRId = curRId;
            }
        }
    }

    if( bestRId==MAX_INT )
    {
        return false;
    }
    else
    {
        tmpObsModR = (bestRId==targRowId) ? obsModR : NULL;
        if( !find_legal_pos_for_mod_row( mod, bestTargPos, tmpObsModR ) )
        {
            cout << "fails..." << endl; getchar();
        }
        assert( bestDisp==Point::dist( bestTargPos, pos ) );
        pos.set_x_y( bestTargPos.x(), bestTargPos.y() );

        return true;
    }
}

bool RowPlacer::find_legal_pos_for_mod_row(Module &mod, Point &pos, ModR *obsModR)
{
//if( mod.name() == "o487964" || mod.name() == "o487963" ){
//    cout << "find_legal_pos_for_mod_row " << mod.name() << " @ " << pos << endl;
//}
        assert( mod.numRects()==1 ); // standard cell

        if( !delete_modInRow( mod, false ) ) return false;

        //_cir->bound_pos_in_max_disp( mod, pos, true );

        // target position
        int targRowId = _cir->y_2_row_id( pos.y() );
        double yLegal = _cir->row_id_2_y( targRowId );

        double xOnSiteStep = _cir->g_x_on_site( pos.x(), targRowId, Circuit::ALIGN_HERE );
        pos.set_x_y( xOnSiteStep, pos.y() );

        if( is_pos_legal_for_mod(mod, pos, obsModR) )
        {
//if( mod.name() == "A1_B1_C16_D18_E1_F1_G3_I1_o478115" ){
//cout << mod.name() << " 1" << endl;
//}
            set_mod_rowM( mod, pos ); return true;
        }

        double xLeftLegal = pos.x(), xRightLegal = pos.x();
        bool isLeftFound  = find_legal_x_on_row( targRowId, mod, LEFT, xLeftLegal, obsModR );
        bool isRightFound = find_legal_x_on_row( targRowId, mod, RIGHT, xRightLegal, obsModR );

        if( !isLeftFound && !isRightFound )
        {
            set_mod_rowM( mod, mod.pos() ); // insert back
            return false;
        }else
        {
            // choose the min cost position
            double xLegal = 0;
            double distLeft = DBL_MAX, distRight = DBL_MAX;
            if( isLeftFound )   distLeft = fabs( xOnSiteStep - xLeftLegal );
            if( isRightFound ) distRight = fabs( xOnSiteStep - xRightLegal );
            xLegal = ( distLeft < distRight ) ? xLeftLegal : xRightLegal;

            if( gArg.checkExist( "oriDist" ) &&
                Point::dist( Point( mod.x(), mod.y() ), Point( xOnSiteStep, yLegal ) ) <
                Point::dist( Point( xLegal,  yLegal ),  Point( xOnSiteStep, yLegal ) ) ){
                set_mod_rowM( mod, mod.pos() ); // insert back
                return false;
            }
//if( mod.name() == "A1_B1_C16_D18_E1_F1_G3_I1_o478115" ){
//cout << mod.name() << " 2" << endl;
//}
            pos.set_x_y( xLegal, yLegal );
            set_mod_rowM( mod, pos );

            return true;
        }
}

bool RowPlacer::ensure_legal_pos_for_mod(Module &mod, Point &pos, vector< pair<Module*,Point> > &moveMods)
{
    assert( mod.numRects()==1 ); // standard cell

    if( !delete_modInRow( mod, false ) ) return false;

    //_cir->bound_pos_in_max_disp( mod, pos, true );

    // target position
    int targRowId = _cir->y_2_row_id( pos.y() );
    double yLegal = _cir->row_id_2_y( targRowId );

    double xOnSiteStep = _cir->g_x_on_site( pos.x(), targRowId, Circuit::ALIGN_HERE );
    pos.set_x_y( xOnSiteStep, pos.y() );


    // legal it directly
    if( is_pos_legal_for_mod(mod, pos) )
    {
//if( mod.name() == "A1_B2_C5_D17_o402608" ){
//cout << mod.name() << " 3" << endl;
//}
        set_mod_rowM( mod, pos ); return true;
    }

    // search legal position for other overlapped cells
    ModR fakeModR( pos.x(), mod.width(), &mod );
    RowM &targRowMap = g_rowModMap( targRowId );
    vector< pair<Module*,Point> > moveMods4Recover(0);
    bool retBool = false;

    while( true )
    {
        RMMIte ite = targRowMap.find( &fakeModR );
        if( ite==targRowMap.end() )
        {
            set_mod_rowM( mod, pos );
            retBool = true;
            break;
        }

        ModR &overlapModR = *ite->first;
        Module &overlapMod = overlapModR.modCir();
        Point targOverPos( overlapModR.x1(), yLegal );
        moveMods4Recover.push_back( make_pair( &overlapMod, targOverPos ) );

        if( !overlapMod.isFixed() && find_legal_pos_for_mod_row( overlapMod, targOverPos, &fakeModR ) )
        {
            moveMods.push_back( make_pair( &overlapMod, targOverPos ) );
        }
        else
        {
            retBool = false;
            break;
        }
    }

    if( retBool == false )
    {
        for( int i=moveMods4Recover.size()-1; i>=0; --i )
        {
            Module &moveMod = *moveMods4Recover[i].first;
            Point &recoverPos = moveMods4Recover[i].second;
            if( !moveMod.isFixed() )
            {
                if( !put_legal_pos_for_mod( moveMod, recoverPos ) ){
                    cout << "[ERROR]::ensure_put_legal_pos_for_mod : recovery fails." << endl;
                    getchar();
                }
            }
        }

        set_mod_rowM( mod, mod.pos() ); // insert back
        moveMods.clear();
    }
    return retBool;
}

// Procedure: find_legal_x_on_row
// Starting from initialed xLegal, find a non-overlapped
// xLegal toward dir (e.g., right, left)
bool RowPlacer::find_legal_x_on_row(const int &rowId, Module &mod, const RowPlacer::Dir &dir, double &xLegal, ModR *obsModR)
{
    if( obsModR!=NULL )
    {
        if( dir==LEFT ){
            xLegal = _cir->g_x_on_site( obsModR->x1() - mod.width(), rowId, Circuit::ALIGN_LEFT );
            if( obsModR->x1() - mod.width() < xLegal ) { return false; }
        }else if( dir==RIGHT ){
            xLegal = _cir->g_x_on_site( obsModR->x2(), rowId, Circuit::ALIGN_RIGHT );
            if( xLegal + mod.width() > _cir->row( rowId ).right() ) { return false; }
        }

    }

//    double yLegal = _cir->row_id_2_y( rowId );
    double xLegalLast = DBL_MAX;
    RowM &targRowMap = g_rowModMap( rowId );
    bool isFound = false;
    do
    {
        //if( !_cir->is_pos_in_max_disp( mod, Point( xLegal, yLegal ) ) ) break;

        ModR tmpMod( xLegal, mod.width(), &mod );

        RMMIte ite = targRowMap.find( &tmpMod );
        if( ite==targRowMap.end() ) // no overlapping module
        {
            isFound = true;
            break;
        }

        ModR &overlapMod = *ite->first;
        if( dir==LEFT )
        {
            if( ite==targRowMap.begin() ) break;

            xLegal = _cir->g_x_on_site( overlapMod.x1() - mod.width(), rowId, Circuit::ALIGN_LEFT );
            if( overlapMod.x1() - mod.width() < xLegal ) { break; }
        }
        else if( dir==RIGHT )
        {
            xLegal = _cir->g_x_on_site( overlapMod.x2(), rowId, Circuit::ALIGN_RIGHT );
            if( xLegal + mod.width() > _cir->row( rowId ).right() ) { break; }
        }

        if( xLegalLast == xLegal ) break;

        xLegalLast = xLegal;

    }while( true );

    return isFound;
}

// Procedure: is_pos_legal_for_mod
// Check if the position is not overlapped with other modules
bool RowPlacer::is_pos_legal_for_mod(Module &mod, const Point &pos, ModR *obsModR)
{
    size_t targRowId = _cir->y_2_row_id( pos.y() );
    Row &targRow = _cir->row(targRowId);
    if( pos.x() < targRow.x() || pos.x()+mod.width() > targRow.right() )
    {
        return false;
    }

    RowM &targRowMap = g_rowModMap( targRowId );
    ModR fakeModR( pos.x(), mod.width(), &mod );
    if( targRowMap.find( &fakeModR )==targRowMap.end() ){
        if( obsModR!=NULL )
        {
            if( pos.x() <= obsModR->x1() && obsModR->x1() <= pos.x()+mod.width() )
                return false;
            else if( obsModR->x1() <= pos.x() && pos.x() <= obsModR->x2() )
                return false;
        }
        return true;

    }else
        return false;
}

// Procedure: set_mod_pos
// Insert the corresponding ModR into the RowM
void RowPlacer::set_mod_rowM(Module &mod, const Point &pos)
{
    RowM &rowM = g_rowModMap( _cir->y_2_row_id( pos.y() ) );
    ModR &modR = mod_2_modR( mod );
    modR.set_x_interval( pos.x(), mod.width() );
    rowM[ &modR ] = &mod;

    assert( rowM.find( &modR )!=rowM.end() );
    ModR &checkModR = *rowM.find( &modR )->first;
    if( !(checkModR.x1()==modR.x1() &&
            checkModR.x2()==modR.x2() &&
            checkModR.modCir().name()==modR.modCir().name()) ){

        cout << "insertion to rowM fails ..." << endl;
        getchar();
    }
}

// Procedure: delete_modInRow
// Remove the module from a row
bool RowPlacer::delete_modInRow(Module &mod, bool isIgnoreNotFound)
{
    RowM &oriRowModMap = g_rowModMap( _cir->y_2_row_id( mod.y() ) );
    RMMIte ite = oriRowModMap.find( &mod_2_modR( mod ) );
    if( ite==oriRowModMap.end() )
    {
        if( isIgnoreNotFound ) return true;

        mod.showInfo();
        cout << "mod " << mod.name() << " not in row " << _cir->y_2_row_id( mod.y() ) << endl;
        getchar();
        return false;
    }
    else
    {
        oriRowModMap.erase( ite );
        return true;
    }
}

void RowPlacer::showAllRowsModules()
{
    for(size_t i=0; i<_cir->numRows(); ++i)
        showRowModules( i );
}

void RowPlacer::showRowModules(int rowId)
{
    cout << "row(" << rowId << "):" << endl;
    for( RMMIte ite = g_rowModMap( rowId ).begin(); ite!=g_rowModMap( rowId ).end(); ++ite )
        cout << *ite->first << endl;
}

bool RowPlacer::initRowStructures(bool isOnlyConsiderFixedMacros)
{
    int numOverlaps = 0;
    //cout << "init row structures ..." << flush;
    if( isOnlyConsiderFixedMacros==false && checkStdCellsOnRowSite()==false ){
        //cout << "WARNING: initRowStructures:: not a legal placement => skip rowInitial." << endl;
        //getchar();
        return false;
    }

    size_t numTotalRects = 0;
    for( size_t i=0; i<_cir->numModules(); ++i ){
        Module &mod = _cir->module(i);
        if( mod.isIOPin() ){
            continue;
        }

        assert( mod.numRects()!=0 );
        for( size_t j=0; j<mod.numRects(); ++j ){
            const Rect &rect = mod.rect(j);
            int rowIdBottom = 0, rowIdTop = 0;
            _cir->rect2RowIds( rect, rowIdBottom, rowIdTop );

            numTotalRects += rowIdTop - rowIdBottom + 1;
        }
    }

    _modsInRow.clear();         _modsInRow.resize( numTotalRects );
    _rowsModMap.clear();        _rowsModMap.resize( _cir->numRows() );
    _modId2ModInRow.clear();    _modId2ModInRow.resize( _cir->numModules() );

    size_t idCurRects = 0;
    for( size_t i=0; i<_cir->numModules(); ++i )
    {
        Module &mod = _cir->module(i);

        if( mod.isIOPin() ) continue;

        for( size_t j=0; j<mod.numRects(); ++j )
        {
            const Rect &rect = mod.rect(j);

            int rowIdBottom = 0, rowIdTop = 0;
            _cir->rect2RowIds( rect, rowIdBottom, rowIdTop );

            for( int k=rowIdBottom; k<=rowIdTop; ++k )
            {
                ModR &curModInRow = _modsInRow[ idCurRects ];
                curModInRow.set_x_interval( rect.left(), rect.width() );
                curModInRow.setModCir( mod );
                ++idCurRects;

                if( !isOnlyConsiderFixedMacros || (isOnlyConsiderFixedMacros && mod.isFixed()) )
                {
                    RowM &rmMap = _rowsModMap[k];
                    if( rmMap.find( &curModInRow )!=rmMap.end() ){
#ifdef DEBUG_ROWPL
                        cout << "overlap: " << mod.name() << mod.pos() << " & " << rmMap.find( &curModInRow )->second->name() << rmMap.find( &curModInRow )->second->pos() << endl;
#endif
                        if( !rmMap.find( &curModInRow )->second->isFixed() || !mod.isFixed() ){
                            ++numOverlaps;
                            Module &modO = *(rmMap.find( &curModInRow )->second);
                            mod.showInfo();
                            modO.showInfo();
                            cout << "overlap: " << mod.name() << mod.pos() << " & " << modO.name() << modO.pos() << endl;

                        }
                    }
                    _rowsModMap[ k ][ &curModInRow ] = &mod;

                }

                _modId2ModInRow[ mod.dbId() ] = &curModInRow;
            }
        }
    }
    assert( numTotalRects==idCurRects);

    if( !isOnlyConsiderFixedMacros && numOverlaps>0 )
    {
        return false;
    }

    //cout << " Done!" << endl;
    return true;

}

bool RowPlacer::checkStdCellsOnRowSite()
{
    bool isStdOnRowSite = true;
    for( size_t i=0; i<_cir->numModules(); ++i ){
        Module &mod = _cir->module(i);
        if( !mod.isStdCell() ) { continue; }

        int rowId = _cir->y_2_row_id( mod.y() );
        bool isModOnRow = is_equal( mod.y(), _cir->row_id_2_y( rowId ) );
        if( !isModOnRow ){
#ifdef DEBUG_ROWPL
            cerr << "WARNING: checkStdCellsOnRowSite:: mod " << mod.name() << " not on the rowSite." << endl;
            mod.showInfo();
#endif
            isStdOnRowSite = false;
            break;
        }

        double xSiteStep = _cir->g_x_on_site( mod.x(), rowId, Circuit::ALIGN_HERE );
        bool isModOnSiteStep = is_equal( mod.x(), xSiteStep );
        if( !isModOnSiteStep ){
#ifdef DEBUG_ROWPL
            cerr << "WARNING: checkStdCellsOnRowSite:: mod " << mod.name() << " not on the siteStep." << endl;
            mod.showInfo();
#endif
            isStdOnRowSite = false;
            break;
        }
    }

    return isStdOnRowSite;
}

}
