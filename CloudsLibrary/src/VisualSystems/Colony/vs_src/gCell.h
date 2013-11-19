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
class colonyPartitionMap;
class coord2i;
class neighbor_iterator;

/* Types */
typedef ofPtr<colonyCell> cellPtr;

/* Classes */

class coord2i {
public:
    int x, y;
    ~coord2i(){}
    coord2i(int x_, int y_): x(x_), y(y_){};
    coord2i(ofPoint const& p):
        x(int(floor(p.x * MAP_SUBDIV / ofGetWidth() ))),
        y(int(floor(p.y * MAP_SUBDIV / ofGetHeight()))) {} //FIXME: GetWidth and GetHeight are called millions of time a second. cache.
    bool operator<  (const coord2i& rhs) const {return (this->ordered() < rhs.ordered());      }
    bool operator>  (const coord2i& rhs) const {return (this->ordered() > rhs.ordered());      }
    bool operator== (const coord2i& rhs) const {return (this->x == rhs.x &&  this->y == rhs.y);}
    int ordered() const { return y * MAP_SUBDIV + x;}
};



class colonyCell {
    ofPoint position;
    ofVec2f acceleration, velocity;
    float cellSize, age, nutrientLevel, maxSize, maxSpeed, maxForce, lifespan, fertilityAge,
            deathThreshold, separationDist, alignmentDist, lastFeedValue;
    bool fertile, dead, hasReplicated;
    
public:
    
    colonyCell(const ofPoint initialPosition = ofPoint(-1,-1));
    void update();
    void draw();
    void doApplyForce(const ofPoint& _force );
    void doApplyFlock(neighbor_iterator& iter);
    void doApplyBorders();
    
    void doFeedCellWidth( ofPixels &_pixels);
    
    bool isFertile(); //TODO: consider "how fertile" and probablistics
    bool isDead();
    bool isReadyToReplicate();
    const ofPoint getPosition() const;
    const ofPoint getVelocity() const;
    float getSize();
    void doSetPosition(const ofPoint& p);
    
    /* returns a new one in a ptr, resets the replacaiton status
     */
    cellPtr doGetReplicated();
    
private:
    ofPoint getSteerToTarget(ofPoint _target);
    bool isInsideBoard(ofPoint p);
};



/**
 * Concatenation iterator for iterating a bunch of vectors in series.
 */

class neighbor_iterator : public vector<cellPtr>::const_iterator{
    
    typedef const vector <cellPtr>* vecPtr;
    vector<vecPtr> v;
    int position, meta, subVecSize, metaVecSize;
    vecPtr currentVecPtr;
    
public:
    
    neighbor_iterator(){}
    
    neighbor_iterator(const neighbor_iterator& other) : v(other.v){
        initialize();
    }
    
    ~neighbor_iterator(){
        //FIXME: Change this
        v.clear(); //TODO: Check if this is enough;
    }
    
    void add(const std::vector<cellPtr>& _v){
        if (!_v.empty()){
            v.push_back( &_v ); //Add as POINTER
            metaVecSize = v.size();
        }
    }
    
    void initialize(){
        meta = 0;
        position = 0;
        currentVecPtr = v[meta];
        subVecSize = currentVecPtr->size();
    }
    
    void increment() {
        if (++position >= subVecSize){
            if (++meta < metaVecSize){
                position = 0;
                currentVecPtr = v[meta];
                subVecSize = currentVecPtr->size();
            }
        }
    }
    
    neighbor_iterator& operator=(const neighbor_iterator& other){ //TODO: Check if necessary
        v = vector<vecPtr>(other.v);
        initialize();
    }
    bool hasNext(){ return ((meta < metaVecSize) || (position < subVecSize)); } //FIXME: This does not check for 0 - sized subarrays. Need to check elsewhere.
    reference operator*() const { return (*currentVecPtr)[position]; }
    pointer operator->()  const { return &((*currentVecPtr)[position]); }
    friend bool operator==(const neighbor_iterator& a, const neighbor_iterator& b){
        return ((a.v == b.v) && (a.position == b.position) && (a.meta == b.meta)); //TODO: Can we really find a counterexample that requires meta?
    }
    friend bool operator!=(const neighbor_iterator& a, const neighbor_iterator& b) { return !(a==b); }
};



/**
 * Data structure for keeping track of the map
 */

class colonyPartitionMap {
    typedef std::map<coord2i, vector<cellPtr> >::value_type value_type;
    typedef std::map<coord2i, vector<cellPtr> >::iterator iter_type;
    map<coord2i, vector<cellPtr> > partitions; //TODO: Change to array. Is simpler and enumerable
    map<coord2i, neighbor_iterator> neighbors;
    
public:
    
    colonyPartitionMap(){
        //Populating this in advance. Cost is very little for any reasonably sized partition.
        for (int i = 0 ; i < MAP_SUBDIV ; ++i){
            for (int j = 0; j < MAP_SUBDIV; ++j) {
                vector<cellPtr> v;
                coord2i c = coord2i(i, j);
                partitions.insert(value_type(c, v));
            }
        }
    }

    ~colonyPartitionMap(){
        clear();
        partitions.clear();
        neighbors.clear();
    }
    
    void clear(){
        for (iter_type iter = partitions.begin(); iter != partitions.end(); ++iter) {
            iter->second.clear();
        }
        neighbors.clear();
    }
    
    void put(const cellPtr& cp){
        coord2i coord = coord2i(cp -> getPosition());
        partitions.at(coord).push_back(cp); //FIXME: Has roundoff errors
    }
    
    void put(vector<cellPtr>::iterator from, const vector<cellPtr>::iterator& to ){
        while(from!=to){
            put(*from);
            from++;
        }
    }
    
    neighbor_iterator getNeighbours(const coord2i& c)
    {
        std::map<coord2i, neighbor_iterator>::iterator k = neighbors.find(c);
        if (k != neighbors.end()){
            k->second.initialize();
            return k->second;
        } else {
            neighbor_iterator iter = neighbor_iterator(); //FIXME: Might throw error
            for (int i = MAX((c.x - 1),0) ; i <= MIN((c.x +1), (MAP_SUBDIV - 1)); i++){
                for (int j = MAX((c.y - 1),0) ; j <= MIN((c.y +1), (MAP_SUBDIV - 1)); j++){
                    iter.add(partitions.at(coord2i(i,j))); //This *should* be 'at' because all vectors have been initialized
                    }}
            iter.initialize();
            neighbors.insert(std::pair<coord2i, neighbor_iterator>(c, iter));
            return iter;
        }
    }
    
private:

    colonyPartitionMap(colonyPartitionMap const& c);//unimplemented; singleton
    void operator=(colonyPartitionMap const& c);    //unimplemented; singleton
};

