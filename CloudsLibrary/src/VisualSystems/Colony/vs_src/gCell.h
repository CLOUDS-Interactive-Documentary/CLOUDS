//
//  gCell.h
//  Colony
//
//  Created by Omer Shapira on 10/11/13.
//
//

#define MAP_SUBDIVISIONS 8

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
    coord2i(ofPoint const& p){
        x = (int) floor(p.x * MAP_SUBDIVISIONS / ofGetWidth());
        y = (int) floor(p.y * MAP_SUBDIVISIONS / ofGetHeight());
    }
    bool operator<  (const coord2i& rhs) const {return (this->ordered() < rhs.ordered());      }
    bool operator>  (const coord2i& rhs) const {return (this->ordered() > rhs.ordered());      }
    bool operator== (const coord2i& rhs) const {return (this->x == rhs.x &&  this->y == rhs.y);}
    int ordered() const{ return y * MAP_SUBDIVISIONS + x;}
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
    void doApplyForce( ofPoint _force ); //TODO: switch to modern C++ with list of force functions
    void doApplyFlock(neighbor_iterator iter);
    void doApplyBorders();
    
    void doFeedCellWidth( ofPixels &_pixels); //TODO: What is this?
    
    bool isFertile(); //TODO: consider "how fertile" and probablistics
    bool isDead();
    bool isReadyToReplicate();
    const ofPoint& getPosition() const;
    void doSetPosition(const ofPoint& p);
    
    /* returns a new one in a ptr, resets the replacaiton status
     */
    cellPtr doGetReplicated();
    
private:
    ofPoint getSteerToTarget(ofPoint _target);
    bool isInsideBoard(ofPoint p);
};



/**
 * Concatenation iterator for iterating a bunch of vectors in series. Doesn't keep track of the vectors
 */

class neighbor_iterator : public virtual std::vector<cellPtr>::iterator {
    
    typedef vector<cellPtr>::iterator cell_iterator;
    typedef std::pair<cell_iterator,cell_iterator> value_type;
    
    vector<value_type> v;
    cell_iterator position;
    vector<value_type>::iterator meta;
    bool initialized;
    
public:
    
    neighbor_iterator(){
        initialized = false;
    }
    
    neighbor_iterator(std::vector<cellPtr>& _v){
        initialized = false;
        if (!_v.empty()){ add(_v); }
    }
    
    ~neighbor_iterator()
    {
        v.clear(); //TODO: Check if this is enough;
    }
    void add(std::vector<cellPtr>& _v){
        if (!initialized && !_v.empty()){
            v.push_back(value_type(_v.begin(), _v.end()));
        }
    }
    void initialize() {
        if (!v.empty()){
            meta = v.begin();
            position = meta->first; //TODO: Needs to return a const
        }
        initialized = true;
    }
    
    neighbor_iterator& operator++(int) {
        if (++position == meta -> second){
            if (++meta != v.end()){
                position = meta -> first;
            }
        }
        return *this;
    }
    bool hasNext(){ return ( (!v.empty()) && ((meta != v.end()) || (position != meta->second))); }
    reference operator*() const { return *position; }
    pointer operator->()  const { return &(*position); }
    friend bool operator==(const neighbor_iterator& a, const neighbor_iterator& b)
    {
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
    map<coord2i, vector<cellPtr> > partitions;
    map<coord2i, neighbor_iterator> neighbors;
    
public:
    
    colonyPartitionMap(){
        //Populating this in advance. Cost is very little for any reasonably sized partition.
        for (int i = 0 ; i < MAP_SUBDIVISIONS ; ++i){
            for (int j = 0; j < MAP_SUBDIVISIONS; ++j) {
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
    
    void put(cellPtr cp){
        coord2i coord = coord2i(cp -> getPosition());
        partitions.at(coord).push_back(cp);
    }
    
    void put(vector<cellPtr>::iterator from, vector<cellPtr>::iterator to ){
        while(from!=to){
            put(*from);
            from++;
        }
    }
    
    neighbor_iterator getNeighbours(coord2i c)
    {
        std::map<coord2i, neighbor_iterator>::iterator k = neighbors.find(c);
        if (k != neighbors.end()){
            k->second.initialize();
            return k->second;
        } else {
            neighbor_iterator iter = neighbor_iterator(); //FIXME: Might throw error
            for (int i = MAX(c.x - 1,0) ; i <= MIN(c.x +1, MAP_SUBDIVISIONS - 1); i++){
                for (int j = MAX(c.y - 1,0) ; j <= MIN(c.y +1, MAP_SUBDIVISIONS - 1); j++){
                        iter.add(partitions.at(c));
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

