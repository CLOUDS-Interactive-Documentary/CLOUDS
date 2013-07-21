//
//  ofxVoro.cpp
//  example
//
//  Created by Patricio Gonzalez Vivo on 4/21/13.
//
//

#include "Voro.h"

using namespace voro;

float getCellRadius(voro::voronoicell &_c){
    return pow(3* _c.volume()/4*PI,1.0/3.0);
};

ofPoint getCellCentroid(voro::voronoicell &_c, ofPoint _pos ){
    double x,y,z;
    _c.centroid(x,y,z);
    return ofPoint(x,y,z)*0.5 + _pos;
};

vector<ofPoint> getCellVerteces(voro::voronoicell &_c, ofPoint _pos ){
    double *ptsp=_c.pts;
    vector<ofPoint> vertices;
    for(int i = 0; i < _c.p; i++, ptsp+=3){
        ofPoint newPoint;
        newPoint.x = _pos.x + ptsp[0]*0.5;
        newPoint.y = _pos.y + ptsp[1]*0.5;
        newPoint.z = _pos.z + ptsp[2]*0.5;
        vertices.push_back(newPoint);
    }
    return vertices;
};

ofMesh getCellMesh(voro::voronoicell &_c, ofPoint _pos ){
    if( _c.p ) {
        
        ofPoint centroid = getCellCentroid(_c,_pos);
        
        int i,j,k,l,m,n;
        
        //  Vertex
        //
        double *ptsp=_c.pts;
        vector<ofPoint> vertices;
        vector<ofPoint> normals;
        for(i = 0; i < _c.p; i++, ptsp+=3){
            ofPoint newPoint;
            newPoint.x = _pos.x + ptsp[0]*0.5;
            newPoint.y = _pos.y + ptsp[1]*0.5;
            newPoint.z = _pos.z + ptsp[2]*0.5;
            vertices.push_back(newPoint);
            
            ofPoint newNormal;
            newNormal = newPoint - _pos;//centroid ;
            newNormal = newNormal.normalize();
            normals.push_back(newNormal);
        }
        
        ofMesh mesh;
        mesh.setMode(OF_PRIMITIVE_TRIANGLES );
        mesh.addVertices(vertices);
        mesh.addNormals(normals);
        
        //  Index
        //
        for(i = 1; i < _c.p; i++){
            for(j = 0; j < _c.nu[i]; j++) {
                
                k = _c.ed[i][j];
                
                if( k >= 0 ) {
                    _c.ed[i][j]=-1-k;
                    l = _c.cycle_up( _c.ed[i][ _c.nu[i]+j], k);
                    m = _c.ed[k][l];
                    _c.ed[k][l]=-1-m;
                    
                    while(m!=i) {
                        n = _c.cycle_up( _c.ed[k][ _c.nu[k]+l],m);
                        mesh.addTriangle(i, k, m);
                        
                        k=m;
                        l=n;
                        m=_c.ed[k][l];
                        _c.ed[k][l]=-1-m;
                    }
                }
            }
        }
        
        return mesh;
    }
    
    return ofMesh();
};

vector<ofMesh>  getCellsFromContainer(voro::container &_con, float _wallsThikness){
    
    vector<ofMesh> cells;
    
    voro::c_loop_all vl( _con );
    int i = 0;
	if( vl.start() ){
        
        do {
            voro::voronoicell c;
            if( !_con.compute_cell(c, vl) ) {
                return cells;
            } else {
                double *pp = _con.p[vl.ijk] + _con.ps * vl.q;
                ofMesh cellMesh = getCellMesh(c, ofPoint(pp[0],pp[1],pp[2])*(float)(1.0+_wallsThikness) );
                cells.push_back( cellMesh );
                i++;
            }
            
        } while( vl.inc() );
    }
    
    return cells;
}

vector<float> getCellsRadius(voro::container &_con){
    vector<float> radius;
    
    voro::c_loop_all vl( _con );
    int i = 0;
	if( vl.start() ){
        
        do {
            voro::voronoicell c;
            if( !_con.compute_cell(c, vl) ) {
                return radius;
            } else {
                double *pp = _con.p[vl.ijk] + _con.ps * vl.q;
                float rad = getCellRadius(c);
                radius.push_back( rad );
                i++;
            }
            
        } while( vl.inc() );
    }
    
    return radius;
}

vector<ofPoint> getCellsPositions(voro::container &_con){
    vector<ofPoint> positions;
    
    voro::c_loop_all vl( _con );
    int i = 0;
	if( vl.start() ){

        do {
            voro::voronoicell c;
            if( !_con.compute_cell(c, vl) ) {
                return positions;
            } else {
                double *pp = _con.p[vl.ijk] + _con.ps * vl.q;
                positions.push_back(ofPoint(pp[0],pp[1],pp[2]));
                i++;
            }
            
        } while( vl.inc() );
    }
    
    return positions;
}

vector<ofPoint> getCellsCentroids(voro::container &_con){
    vector<ofPoint> centroids;
    
    voro::c_loop_all vl( _con );
    int i = 0;
	if( vl.start() ){
        
        do {
            voro::voronoicell c;
            if( !_con.compute_cell(c, vl) ) {
                return centroids;
            } else {
                double *pp = _con.p[vl.ijk] + _con.ps * vl.q;
                centroids.push_back( getCellCentroid(c) + ofPoint(pp[0],pp[1],pp[2]) );
                i++;
            }
            
        } while( vl.inc() );
    }
    
    return centroids;
}

vector< vector<ofPoint> > getCellsVertices(voro::container &_con){
    vector< vector<ofPoint> > cells;
    
    ofPoint pos;
    
    voro::c_loop_all vl( _con );
    int i = 0;
	if( vl.start() ){
        
        do {
            voro::voronoicell c;
            if( !_con.compute_cell(c, vl) ) {
                return cells;
            } else {
                double *pp = _con.p[vl.ijk] + _con.ps * vl.q;
                vector< ofPoint > cell = getCellVerteces(c, ofPoint(pp[0],pp[1],pp[2]) );
                cells.push_back( cell );
                i++;
            }
            
        } while( vl.inc() );
    }
    
    return cells;
}

vector< ofPolyline > getCellsPolylines(voro::container &_con){
    vector< ofPolyline > cells;
    
    ofPoint pos;
    
    voro::c_loop_all vl( _con );

	if( vl.start() ){
        
        do {
            voro::voronoicell c;
            int k = 0;
            if( !_con.compute_cell(c, vl) ) {
                return cells;
            } else {
                
                double *pp = _con.p[vl.ijk] + _con.ps * vl.q;
                ofPoint pos = ofPoint(pp[0],pp[1],pp[2]);
                
                ofPolyline cell;
                
                double *ptsp= c.pts;
                vector<ofPoint> points;
                
                //  Index
                //
                for(int i = 0; i < c.p; i++){
                    for(int j = 0; j < c.nu[i]; j++) {
                        int k = c.ed[i][0];
                        
                        ofPoint newPoint;
                        newPoint.x = pos.x + c.pts[3*k]*0.5;
                        newPoint.y = pos.y + c.pts[3*k+1]*0.5;
                        newPoint.z = pos.z + c.pts[3*k+2]*0.5;
                        cell.addVertex(newPoint);
                    }
                }
                
                cells.push_back( cell );
                
            }
            
        } while( vl.inc() );
    }
    
    return cells;
}

bool insideContainer(voro::container &_con, ofPoint _pos){
    return _con.point_inside(_pos.x, _pos.y, _pos.z);
}

void addCellSeed(voro::container &_con, ofPoint &_pnt, int _i, bool _checkInside){
    if (_checkInside){
        if ( insideContainer(_con, _pnt ))
            _con.put(_i, _pnt.x, _pnt.y, _pnt.z);
    } else {
        _con.put(_i, _pnt.x, _pnt.y, _pnt.z);
    }
}

void addCellSeed(voro::container &_con, ofPoint *_pnt, int _i, bool _checkInside){
    if (_checkInside){
        if ( insideContainer(_con, *_pnt));
        _con.put(_i, _pnt->x, _pnt->y, _pnt->z);
    } else {
        _con.put(_i, _pnt->x, _pnt->y, _pnt->z);
    }
}

void addCellsSeeds(voro::container &_con, vector<ofPoint> &_pnts, bool _checkInside){
    for (int i = 0; i < _pnts.size(); i++) {
        addCellSeed(_con, _pnts[i], i, _checkInside);
    }
}

void addCellsSeeds(voro::container &_con, vector<ofPoint*> &_pnts, bool _checkInside){
    for (int i = 0; i < _pnts.size(); i++) {
        addCellSeed(_con, _pnts[i], i, _checkInside);
    }
}

void addCellsSeeds(voro::container &_con, ofPoint *_pnts, int _nSize, bool _checkInside){
    for (int i = 0; i < _nSize; i++) {
        addCellSeed(_con, _pnts[i], i, _checkInside);
    }
}
