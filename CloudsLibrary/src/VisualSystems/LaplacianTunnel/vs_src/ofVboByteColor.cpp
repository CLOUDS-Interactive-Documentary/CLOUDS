//TODO:
//GL Error checking
// handle idling of arrays: possibly let gl create memory and use map buffers for streaming
// index updating/deleting?
// setVertexData with float* should know about ofVec3f vs ofVec2f?

#include "ofVboByteColor.h"
#include "ofUtils.h"
#include <map>

static map<GLuint,int> & getIds(){
	static map<GLuint,int> * ids = new map<GLuint,int>;
	return *ids;
}

//--------------------------------------------------------------
static void retain(GLuint id){
	if(id==0) return;
	if(getIds().find(id)!=getIds().end()){
		getIds()[id]++;
	}else{
		getIds()[id]=1;
	}
}

//--------------------------------------------------------------
static void release(GLuint id){
	if(getIds().find(id)!=getIds().end()){
		getIds()[id]--;
		if(getIds()[id]==0){
			glDeleteBuffers(1, &id);
			getIds().erase(id);
		}
	}else{
		ofLog(OF_LOG_WARNING,"ofVbo: releasing id not found, this shouldn't be happening releasing anyway");
		glDeleteBuffers(1, &id);
	}
}

//--------------------------------------------------------------
ofVboByteColor::ofVboByteColor()
{
	bUsingVerts = false;
	bUsingTexCoords = false;
	bUsingColors = false;
	bUsingNormals = false;
	bUsingIndices = false;

	vertSize		= -1;
	vertStride      = 0;

	vertUsage		= -1;
	colorUsage		= -1;
	normUsage		= -1;
	texUsage		= -1;

	vertId     = 0;
	normalId   = 0;
	colorId    = 0;
	texCoordId = 0;
	indexId    = 0;


	bAllocated		= false;
}

ofVboByteColor::ofVboByteColor(const ofVboByteColor & mom){
	bUsingVerts = mom.bUsingVerts;
	bUsingTexCoords = mom.bUsingTexCoords;
	bUsingColors = mom.bUsingColors;
	bUsingNormals = mom.bUsingNormals;
	bUsingIndices = mom.bUsingIndices;

	vertSize		= mom.vertSize;
	vertStride      = mom.vertStride;
	colorStride		= mom.colorStride;
	normalStride    = mom.normalStride;
	texCoordStride  = mom.texCoordStride;

	vertUsage		= mom.vertUsage;
	colorUsage		= mom.colorUsage;
	normUsage		= mom.normUsage;
	texUsage		= mom.texUsage;

	vertId     = mom.vertId;
	retain(vertId);
	normalId   = mom.normalId;
	retain(normalId);
	colorId    = mom.colorId;
	retain(colorId);
	texCoordId = mom.texCoordId;
	retain(texCoordId);
	indexId    = mom.indexId;
	retain(indexId);


	bAllocated		= mom.bAllocated;
}

ofVboByteColor & ofVboByteColor::operator=(const ofVboByteColor& mom){
	if(&mom==this) return *this;
	bUsingVerts = mom.bUsingVerts;
	bUsingTexCoords = mom.bUsingTexCoords;
	bUsingColors = mom.bUsingColors;
	bUsingNormals = mom.bUsingNormals;
	bUsingIndices = mom.bUsingIndices;

	vertSize		= mom.vertSize;
	vertStride      = mom.vertStride;
	colorStride		= mom.colorStride;
	normalStride    = mom.normalStride;
	texCoordStride  = mom.texCoordStride;

	vertUsage		= mom.vertUsage;
	colorUsage		= mom.colorUsage;
	normUsage		= mom.normUsage;
	texUsage		= mom.texUsage;

	vertId     = mom.vertId;
	retain(vertId);
	normalId   = mom.normalId;
	retain(normalId);
	colorId    = mom.colorId;
	retain(colorId);
	texCoordId = mom.texCoordId;
	retain(texCoordId);
	indexId    = mom.indexId;
	retain(indexId);


	bAllocated		= mom.bAllocated;
	return *this;
}

//--------------------------------------------------------------
ofVboByteColor::~ofVboByteColor(){
	clear();
}

//--------------------------------------------------------------
void ofVboByteColor::setMesh(const ofMesh & mesh, int usage){
	setVertexData(mesh.getVerticesPointer(),mesh.getNumVertices(),usage);
	setColorData(mesh.getColorsPointer(),mesh.getNumColors(),usage);
	setNormalData(mesh.getNormalsPointer(),mesh.getNumNormals(),usage);
	setTexCoordData(mesh.getTexCoordsPointer(),mesh.getNumTexCoords(),usage);
	setIndexData(mesh.getIndexPointer(), mesh.getNumIndices(), usage);
}

//--------------------------------------------------------------
void ofVboByteColor::setVertexData(const ofVec3f * verts, int total, int usage) {
	setVertexData(&verts[0].x,3,total,usage,sizeof(ofVec3f));
}

//--------------------------------------------------------------
void ofVboByteColor::setVertexData(const ofVec2f * verts, int total, int usage) {
	setVertexData(&verts[0].x,2,total,usage,sizeof(ofVec2f));
}

//--------------------------------------------------------------
void ofVboByteColor::setVertexData(const float * vert0x, int numCoords, int total, int usage, int stride) {
	if(vert0x == NULL) {
		ofLog(OF_LOG_WARNING,"ofVboByteColor: bad vertex data!\n");
		return;	
	}
	if(vertId==0) {
		bAllocated  = true;
		bUsingVerts = true;
		glGenBuffers(1, &(vertId));
		retain(vertId);
	}

	vertUsage = usage;
	vertSize = numCoords;
	vertStride = stride;
	totalVerts = total;
	
	glBindBuffer(GL_ARRAY_BUFFER, vertId);
	glBufferData(GL_ARRAY_BUFFER, total * stride, vert0x, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void ofVboByteColor::setColorData(const ofFloatColor * colors, int total, int usage) {
	setColorData(&colors[0].r,total,usage,sizeof(ofFloatColor));
}

//--------------------------------------------------------------
void ofVboByteColor::setColorData(const float * color0r, int total, int usage, int stride) {
	if(color0r == NULL) {
		ofLog(OF_LOG_WARNING,"ofVboByteColor: bad color data!\n");
		return;	
	}
	if(colorId==0) {
		bUsingColors = true;
		glGenBuffers(1, &(colorId));
		retain(colorId);
	}
	colorUsage = usage;
	colorStride = stride;
	
	glBindBuffer(GL_ARRAY_BUFFER, colorId);
	glBufferData(GL_ARRAY_BUFFER, total * stride, color0r, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void ofVboByteColor::setNormalData(const ofVec3f * normals, int total, int usage) {
	setNormalData(&normals[0].x,total,usage,sizeof(ofVec3f));
}

//--------------------------------------------------------------
void ofVboByteColor::setNormalData(const float * normal0x, int total, int usage, int stride) {
	if(normal0x == NULL) {
		ofLog(OF_LOG_WARNING,"ofVboByteColor: bad normal data!\n");
		return;	
	}
	if(normalId==0) {
		bUsingNormals = true;
		glGenBuffers(1, &(normalId));
		retain(normalId);
	}
	normUsage = usage;
	normalStride = stride;
	
	glBindBuffer(GL_ARRAY_BUFFER, normalId);
	glBufferData(GL_ARRAY_BUFFER, total * stride, normal0x, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void ofVboByteColor::setTexCoordData(const ofVec2f * texCoords, int total, int usage) {
	setTexCoordData(&texCoords[0].x,total,usage,sizeof(ofVec2f));
}

//--------------------------------------------------------------
void ofVboByteColor::setTexCoordData(const float * texCoord0x, int total, int usage, int stride) {
	if(texCoord0x == NULL) {
		ofLog(OF_LOG_WARNING,"ofVboByteColor: bad texCoord data!\n");
		return;	
	}
	if(texCoordId==0) {
		bUsingTexCoords = true;
		glGenBuffers(1, &(texCoordId));
		retain(texCoordId);
	}
	texUsage = usage;
	texCoordStride = stride;
	
	glBindBuffer(GL_ARRAY_BUFFER, texCoordId);
	glBufferData(GL_ARRAY_BUFFER, total * stride, texCoord0x, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


//--------------------------------------------------------------
void ofVboByteColor::setIndexData(const ofIndexType * indices, int total, int usage){
	if(indices == NULL){
		ofLog(OF_LOG_WARNING,"ofVboByteColor: bad index data!\n");
		return;
	}
	if(indexId==0){
		bUsingIndices = true;
		glGenBuffers(1, &(indexId));
		retain(indexId);
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(ofIndexType) * total, &indices[0], usage);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void ofVboByteColor::updateMesh(const ofMesh & mesh){
	ofMesh * nonconstMesh = (ofMesh*)&mesh;
	if(nonconstMesh->haveVertsChanged()) updateVertexData(mesh.getVerticesPointer(),mesh.getNumVertices());
	if(nonconstMesh->haveColorsChanged()) updateColorData(mesh.getColorsPointer(),mesh.getNumColors());
	if(nonconstMesh->haveNormalsChanged()) updateNormalData(mesh.getNormalsPointer(),mesh.getNumNormals());
	if(nonconstMesh->haveTexCoordsChanged()) updateTexCoordData(mesh.getTexCoordsPointer(),mesh.getNumTexCoords());
}

//--------------------------------------------------------------
void ofVboByteColor::updateVertexData(const ofVec3f * verts, int total) {
	updateVertexData(&verts[0].x,total);
}

//--------------------------------------------------------------
void ofVboByteColor::updateVertexData(const ofVec2f * verts, int total) {
	updateVertexData(&verts[0].x,total);
}

//--------------------------------------------------------------
void ofVboByteColor::updateVertexData(const float * vert0x, int total) {
	if(vertId!=0){
		glBindBuffer(GL_ARRAY_BUFFER, vertId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, total*vertStride, vert0x);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

//--------------------------------------------------------------
void ofVboByteColor::updateColorData(const ofFloatColor * colors, int total) {
	updateColorData(&colors[0].r,total);
}

//--------------------------------------------------------------
void ofVboByteColor::updateColorData(const float * color0r, int total) {
	if(colorId!=0) {
		glBindBuffer(GL_ARRAY_BUFFER, colorId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, total*colorStride, color0r);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

//--------------------------------------------------------------
void ofVboByteColor::updateNormalData(const ofVec3f * normals, int total) {
	updateNormalData(&normals[0].x,total);
}

//--------------------------------------------------------------
void ofVboByteColor::updateNormalData(const float * normal0x, int total) {
	if(normalId!=0) {
		glBindBuffer(GL_ARRAY_BUFFER, normalId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, total*normalStride, normal0x);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

//--------------------------------------------------------------
void ofVboByteColor::updateTexCoordData(const ofVec2f * texCoords, int total) {
	updateTexCoordData(&texCoords[0].x,total);
}

//--------------------------------------------------------------
void ofVboByteColor::updateTexCoordData(const float * texCoord0x, int total) {
	if(texCoordId!=0) {
		glBindBuffer(GL_ARRAY_BUFFER, texCoordId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, total*texCoordStride, texCoord0x);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

//--------------------------------------------------------------
void ofVboByteColor::updateIndexData(const ofIndexType * indices, int total) {
	if(indexId!=0) {
		glBindBuffer(GL_ARRAY_BUFFER, indexId);
		glBufferSubData(GL_ARRAY_BUFFER, 0, total*sizeof(ofIndexType), &indices[0]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}


void ofVboByteColor::enableColors(){
	if(colorId!=0) bUsingColors=true;
}

void ofVboByteColor::enableNormals(){
	if(normalId!=0) bUsingNormals=true;
}

void ofVboByteColor::enableTexCoords(){
	if(texCoordId!=0) bUsingTexCoords=true;
}

void ofVboByteColor::enableIndices(){
	if(indexId!=0) bUsingIndices=true;
}

void ofVboByteColor::disableColors(){
	bUsingColors=false;
}

void ofVboByteColor::disableNormals(){
	bUsingNormals=false;
}

void ofVboByteColor::disableTexCoords(){
	bUsingTexCoords=false;
}

void ofVboByteColor::disableIndices(){
	bUsingIndices=false;
}

//--------------------------------------------------------------
bool ofVboByteColor::getIsAllocated(){
	return bAllocated;
}	

//--------------------------------------------------------------
bool ofVboByteColor::getUsingVerts(){
	return bUsingVerts;
}	

//--------------------------------------------------------------
bool ofVboByteColor::getUsingColors(){
	return bUsingColors;
}	

//--------------------------------------------------------------
bool ofVboByteColor::getUsingNormals(){
	return bUsingNormals;
}

//--------------------------------------------------------------
bool ofVboByteColor::getUsingTexCoords(){
	return bUsingTexCoords;
}

//--------------------------------------------------------------
bool ofVboByteColor::getUsingIndices(){
	return  bUsingIndices;
}

//--------------------------------------------------------------
GLuint ofVboByteColor::getVertId(){
	return vertId;
}

//--------------------------------------------------------------
GLuint ofVboByteColor::getColorId(){
	return colorId;
}

//--------------------------------------------------------------
GLuint ofVboByteColor::getNormalId(){
	return normalId;
}

//--------------------------------------------------------------
GLuint ofVboByteColor::getTexCoordId(){
	return texCoordId;
}

//--------------------------------------------------------------
GLuint ofVboByteColor::getIndexId(){
	return indexId;
}

//--------------------------------------------------------------
void ofVboByteColor::bind(){
	
//#ifndef TARGET_OPENGLES
//	glPushAttrib(GL_ALL_ATTRIB_BITS);
//	glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
//#endif
	
	if(bUsingVerts){
		glEnableClientState(GL_VERTEX_ARRAY);		
		glBindBuffer(GL_ARRAY_BUFFER, vertId);
		glVertexPointer(vertSize, GL_FLOAT, vertStride, 0);
	}
	
	if(bUsingColors) {
		glEnableClientState(GL_COLOR_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, colorId);
		//glColorPointer(4, GL_FLOAT, colorStride, 0);
		glColorPointer(4,GL_UNSIGNED_BYTE, 0, 0);
	}
	
	if(bUsingNormals) {
		glEnableClientState(GL_NORMAL_ARRAY);		
		glBindBuffer(GL_ARRAY_BUFFER, normalId);
		glNormalPointer(GL_FLOAT, normalStride, 0);
	}
	
	if(bUsingTexCoords) {
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);		
		glBindBuffer(GL_ARRAY_BUFFER, texCoordId);
		glTexCoordPointer(2, GL_FLOAT, texCoordStride, 0);
	}
}

//--------------------------------------------------------------
void ofVboByteColor::unbind() {
//	if(bUsingVerts)  glDisableClientState(GL_VERTEX_ARRAY);
	if(bUsingColors) glDisableClientState(GL_COLOR_ARRAY);
	if(bUsingNormals) glDisableClientState(GL_NORMAL_ARRAY);
	if(bUsingTexCoords) glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_EDGE_FLAG_ARRAY);
	
//#ifndef	TARGET_OPENGLES
//	glPopClientAttrib();
//	glPopAttrib();	
//#endif
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//--------------------------------------------------------------
void ofVboByteColor::draw(int drawMode, int first, int total) {
	if(bAllocated) {
		bind();
		glDrawArrays(drawMode, first, total);
		unbind();
	}
}

//--------------------------------------------------------------
void ofVboByteColor::drawElements(int drawMode, int amt) {
	if(bAllocated){
		bind();
		if(bUsingIndices){
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexId);
#ifdef TARGET_OPENGLES
			glDrawElements(drawMode, amt, GL_UNSIGNED_SHORT, NULL);
#else
			glDrawElements(drawMode, amt, GL_UNSIGNED_INT, NULL);
#endif
		}
		unbind();
	}
}

//--------------------------------------------------------------
void ofVboByteColor::clear(){
	clearVertices();
	clearNormals();
	clearColors();
	clearTexCoords();
	clearIndices();
}


//--------------------------------------------------------------
void ofVboByteColor::clearVertices(){
	if(vertId!=0){
		release(vertId);
		vertId = 0;
		bUsingVerts = false;
	}
}

//--------------------------------------------------------------
void ofVboByteColor::clearNormals(){
	if(normalId!=0){
		release(normalId);
		normalId = 0;
		bUsingNormals = false;
	}
}

//--------------------------------------------------------------
void ofVboByteColor::clearColors(){
	if(colorId!=0){
		release(colorId);
		colorId = 0;
		bUsingColors = false;
	}
}

//--------------------------------------------------------------
void ofVboByteColor::clearTexCoords(){
	if(texCoordId!=0){
		release(texCoordId);
		texCoordId = 0;
		bUsingTexCoords = false;
	}
}

//--------------------------------------------------------------
void ofVboByteColor::clearIndices(){
	if(indexId!=0){
		release(indexId);
		indexId = 0;
		bUsingIndices = false;
	}
}
