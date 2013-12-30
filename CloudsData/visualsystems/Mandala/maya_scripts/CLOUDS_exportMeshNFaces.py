# CLOUDS_exportMeshNFaces.py
if 'cmds' not in locals():
	import maya.cmds as cmds

mesh = cmds.ls(sl=1,fl=1)
vertexCount = cmds.polyEvaluate( mesh, v=True )
faceCount = cmds.polyEvaluate( mesh, f=True )

output = "vertexCount " + str(vertexCount) + "\n"
output += "faceCount " + str(faceCount) + "\n"

def textLineToOutput( prefix, content ):
	outString = prefix;
	for i in content:
		outString += " " + str(i)

	return outString + "\n"

for i in xrange(0,vertexCount):
	v = cmds.pointPosition( mesh[0] + '.vtx[' + str(i) + ']' )
	output += textLineToOutput( "v", v )

for i in xrange(0,faceCount):
	fv = cmds.polyInfo( mesh[0] + ".f[" +str(i)+"]", fv=True )[0].split()[2:]
	output += textLineToOutput( "fv", fv )

	faceCenter = [0,0,0]
	for j in fv:
		v = cmds.pointPosition( mesh[0] + '.vtx[' + str(j) + ']' )
		faceCenter[0] += v[0]
		faceCenter[1] += v[1]
		faceCenter[2] += v[2]
	faceCenter[0] /= len(fv)
	faceCenter[1] /= len(fv)
	faceCenter[2] /= len(fv)
	output += textLineToOutput( "fc", faceCenter )

	fn = cmds.polyInfo( mesh[0] + ".f[" +str(i)+"]", fn=True )[0].split()[2:]
	output += textLineToOutput( "fn", fn )

# print output

fileLoc = cmds.fileDialog2(fileMode = 0)[0]#, fileFilter = ".txt", selectFileFilter = ".txt")[0];

print fileLoc
t = open( fileLoc, "w" );
t.write( output )
t.close()
