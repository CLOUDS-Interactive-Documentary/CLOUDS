
#include "ofxMeshUtils.h"


//--------------------------------------------------------------------------
ofMesh ofxMeshUtils::getIndiciedMeshFromNonIndiciedMesh( ofMesh & mesh ){
        
    ofMesh newMesh;
    vector < ofPoint > uniqueVertices;
    for (int i = 0; i < mesh.getVertices().size(); i++){
        
        int id = -1;
        for (int j = 0; j < uniqueVertices.size(); j++){
            if ( (uniqueVertices[j] - mesh.getVertices()[i]).length() < 0.1f){
                id = j;
            }
        }
        
        if (id == -1){
            uniqueVertices.push_back(mesh.getVertices()[i]);
            id = uniqueVertices.size() - 1;
            newMesh.addVertex(mesh.getVertices()[i]);
        }
        
        newMesh.addIndex(id);
    }
    return newMesh;
}

    
    
//--------------------------------------------------------------------------
void ofxMeshUtils::calcNormals( ofMesh & mesh, bool bNormalize ){
    
    for( int i=0; i < mesh.getVertices().size(); i++ ) mesh.addNormal(ofPoint(0,0,0));
    
    for( int i=0; i < mesh.getIndices().size(); i+=3 ){
        const int ia = mesh.getIndices()[i];
        const int ib = mesh.getIndices()[i+1];
        const int ic = mesh.getIndices()[i+2];
        
        ofVec3f e1 = mesh.getVertices()[ia] - mesh.getVertices()[ib];
        ofVec3f e2 = mesh.getVertices()[ic] - mesh.getVertices()[ib];
        ofVec3f no = e2.cross( e1 );
        
        // depending on your clockwise / winding order, you might want to reverse the e2 / e1 above if your normals are flipped. 
        
        mesh.getNormals()[ia] += no;
        mesh.getNormals()[ib] += no;
        mesh.getNormals()[ic] += no;
    }
    
    if (bNormalize)
    for(int i=0; i < mesh.getNormals().size(); i++ ) {
        mesh.getNormals()[i].normalize();   
    }
}


//--------------------------------------------------------------------------
ofMesh ofxMeshUtils::loadObj(string filename) {
    ofMesh m;
    bool smooth = false;
    vector<ofVec3f> v;
    vector<ofVec2f> vt;
    ofFile f(filename);
    while(!f.eof()) {
        string c;
        f >> c;
        if(c.size()) {
            if(c == "v") {
                float x, y, z;
                f >> x >> y >> z;
                if(smooth) {
                    m.addVertex(ofVec3f(x, y, z));
                } else {
                    v.push_back(ofVec3f(x, y, z));
                }
            } else if(c == "vt") {
                float u, v;
                f >> u >> v;
                if(!smooth) {
                    vt.push_back(ofVec2f(u, v));
                }
            } else if(c == "f") {
                string l;
                getline(f, l);
                replace(l.begin(), l.end(), '/', ' ');
                istringstream ls(l);
                int vi1, vti1, vi2, vti2, vi3, vti3;
                ls >> vi1 >> vti1 >> vi2 >> vti2 >> vi3 >> vti3;
                if(smooth) {
                    m.addIndex(vi1-1);
                    m.addIndex(vi2-1);
                    m.addIndex(vi3-1);
                } else {
                    m.addVertex(v[vi1-1]);
                    m.addVertex(v[vi2-1]);
                    m.addVertex(v[vi3-1]);
                    m.addTexCoord(vt[vti1-1]);
                    m.addTexCoord(vt[vti2-1]);
                    m.addTexCoord(vt[vti3-1]);
                }
                if(ls.peek() == ' ') {
                    int vi4, vti4;
                    ls >> vi4 >> vti4;
                    if(smooth) {
                        m.addIndex(vi1-1);
                        m.addIndex(vi3-1);
                        m.addIndex(vi4-1);
                    } else {
                        m.addVertex(v[vi1-1]); 
                        m.addVertex(v[vi3-1]);
                        m.addVertex(v[vi4-1]);
                        m.addTexCoord(vt[vti1-1]);
                        m.addTexCoord(vt[vti2-1]);
                        m.addTexCoord(vt[vti3-1]);
                    }
                }
            }
        }
    }
    return m;

}

