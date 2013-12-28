//
//  gCell.h
//  Colony
//
//  Created by Omer Shapira on 10/11/13.
//
//

#define MAP_SUBDIV 10

#pragma once

#include "ofMain.h"

/* Forward declarations */
class colonyCell;
class cellParams;
class colonyPartitionMap;
class coord2i;
class neighbor_iterator;

/* Types */
typedef ofPtr<colonyCell> cellPtr;

/* Classes */


class colonyCell {
private:
    
    //STATE
    ofPoint position, velocity, acceleration, forces;
    bool fertile, dead, hasReplicated;
    float nutrientLevel, cellSize, age, lastFeedValue, replicationChances;

    //INT. PARAMS
    float   maxSize, maxSpeed, maxForce, lifespan, fertilityAge, separationDist, alignmentDist;
    const cellParams& _params;
    
    ofPoint anchor;
    
public:
    
    colonyCell(const ofPoint initialPosition, const cellParams& params);
    void update();
    void draw();
    
    void doResetForces();
    void doAddForce(const ofPoint& _force);
    void doSetInertia();
    ofPoint getUpdatedAcceleration();
    
    void doScanAndFlock(neighbor_iterator& iter);
    void doApplyBorders(float padding);
    void doWrapXY();
    
    void doFeedCellNoise();
    void doFeedCellNoise(const ofFbo& texture);
    void doAddTurbulence();
    
    bool isFertile(); 
    bool isDead();
    bool isReadyToReplicate();
    const ofPoint getPosition() const;
    const ofPoint getVelocity() const;
    float getSize();
    void doSetPosition(const ofPoint& p);
    
    /* returns a new one in a ptr, resets the replacaiton status
     */
    cellPtr doGetReplicated();
    
    
    
    float getSeparationDist();
    float getAlignmentDist();
private:
    ofPoint getSteerToTarget(ofPoint _target);
    bool isInsideBoard(ofPoint p);
};


/* ========================== HELPER CLASSES ============================ */


class cellParams{
public:
    float deathThreshold, dynamicFrictionCoeff, amtTurbulence, spdTurbulence,
    amtAlign, amtCohere, amtSeparate, lifespanMin, lifespanMax, fertilityRate,
    nutrientAmount, nutrientTimeCoef, nutrientFalloff, maxSpeed_min, maxSpeed_max,
    maxSize_min, maxSize_max, maxForce_min, maxForce_max;
    
    cellParams(){
        dynamicFrictionCoeff = 0.1;
        deathThreshold = .002;
        amtTurbulence = .5;
        amtAlign = 2;
        amtSeparate = 80;
        amtCohere = .5;
        lifespanMin = 30;
        lifespanMax = 200;
        spdTurbulence = 10;
        fertilityRate = .8;
        
        nutrientAmount = 350;
        nutrientTimeCoef = 100;
        nutrientFalloff = 0.5;
        
        maxSpeed_min = 0.3;
        maxSpeed_max = 0.6;
        
        maxForce_min = 0.6;
        maxForce_max = 1.0;
        
        maxSize_min = 3;
        maxSize_max = 8;
    }
};

class coord2i {
    //TODO: Get rid of this class and use direct addressing via ordered().
    //      It's redundant.
public:
    int x, y;
    ~coord2i(){}
    coord2i(int x_, int y_): x(x_), y(y_){};
    coord2i(ofPoint const& p):
    x(int(ofClamp((p.x * MAP_SUBDIV) / ofGetWidth() , 0, MAP_SUBDIV - 0.01))),
    y(int(ofClamp((p.y * MAP_SUBDIV) / ofGetHeight(), 0, MAP_SUBDIV - 0.01))) {}
    bool operator<  (const coord2i& rhs) const {return (this->ordered() < rhs.ordered()); }
    bool operator>  (const coord2i& rhs) const {return (this->ordered() > rhs.ordered()); }
    bool operator== (const coord2i& rhs) const {return (this->ordered() == rhs.ordered());}
    int ordered() const { return y * MAP_SUBDIV + x; }
};



/**
 * Concatenation iterator for iterating a bunch of vectors in series.
 */
class neighbor_iterator : private vector<cellPtr>::const_iterator{
    typedef const vector <cellPtr>* vecPtr;
    vector<vecPtr> v;
    int position, meta;
    
public:
    neighbor_iterator(){ initialize(); }
    neighbor_iterator(const neighbor_iterator& other) : v(other.v){ initialize(); }
    ~neighbor_iterator(){ v.clear(); }
    void add(const std::vector<cellPtr>& _v){
        if (!_v.empty()){ v.push_back( &_v ); /* Add as POINTER */}
    }
    void initialize(){
        meta = 0;
        position = 0;
    }
    void increment() {
        if (++position >= v[meta]->size()){ //Can be optimized with caching of v[meta]
            if (++meta < v.size()){
                position = 0;
            }}}
    neighbor_iterator& operator=(const neighbor_iterator& other){
        v = vector<vecPtr>(other.v);
        initialize();
    }
    bool hasNext(){ return ((meta < v.size())); }
    reference operator*() const { return (*(v[meta]))[position]; }
    pointer operator->()  const { return &((*(v[meta]))[position]); }
    friend bool operator!=(const neighbor_iterator& a, const neighbor_iterator& b) { return !(a==b); }
    friend bool operator==(const neighbor_iterator& a, const neighbor_iterator& b){
        return ((a.v == b.v) && (a.position == b.position) && (a.meta == b.meta));
    }
};




/**
 * Data structure for keeping track of the map
 */

class colonyPartitionMap {
private:
    vector<cellPtr>* partitions[MAP_SUBDIV * MAP_SUBDIV];
    neighbor_iterator* neighbors[MAP_SUBDIV * MAP_SUBDIV];
    
public:
    colonyPartitionMap(){
        //Populating this in advance. Cost is very little for any reasonably sized partition.
        for (int i = 0 ; i < MAP_SUBDIV ; ++i){
            for (int j = 0; j < MAP_SUBDIV; ++j) {
                coord2i c = coord2i(i, j);
                partitions[c.ordered()] = new vector<cellPtr>();
            }}}
    ~colonyPartitionMap(){
        clear();
        for ( int i = 0 ; i < MAP_SUBDIV * MAP_SUBDIV ; i++ ){
            delete partitions[i];
            partitions[i] = NULL; //UGH C++
        }
    }
    void clear(){
        for (int i = 0 ; i < MAP_SUBDIV * MAP_SUBDIV ; i++) {
            partitions[i]->clear();
            delete neighbors[i];
            neighbors[i] = NULL; //YOU ARE A BAD MAN, BJARNE
        }
    }
    void put(const cellPtr& cp){partitions[coord2i(cp->getPosition()).ordered()]->push_back(cp);}
    void put(const vector<cellPtr>& vec){ for (int i = 0 ; i < vec.size() ; i++){ put(vec[i]); }}
    neighbor_iterator getNeighbours(const coord2i& c)
    {
        neighbor_iterator* k = neighbors[c.ordered()];
        if (k != NULL){
            return *k; //Returns a copy!
        } else {
            neighbor_iterator* iter = new neighbor_iterator();
            for (int i = MAX((c.x - 1),0) ; i <= MIN((c.x +1), (MAP_SUBDIV - 1)); i++){
                for (int j = MAX((c.y - 1),0) ; j <= MIN((c.y +1), (MAP_SUBDIV - 1)); j++){
                    iter->add(*partitions[coord2i(i,j).ordered()]);
                    }}
            iter->initialize();
            neighbors[c.ordered()] = iter;
            return *iter; //return a copy
        }}
    
private:
    colonyPartitionMap(colonyPartitionMap const& c);//singleton
    void operator=(colonyPartitionMap const& c);    //singleton
};

