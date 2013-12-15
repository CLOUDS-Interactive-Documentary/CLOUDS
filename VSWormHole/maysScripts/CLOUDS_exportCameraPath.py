# CLOUDS_exportCameraPath.py
# 
# by Lars Berg
# laserstorm@gmail.com
# 
# this script samples a selected camera and exports it's position, lookat and up info
# overt the course of the current time slider
# 
# a prompt will ask you for the number of frames between samples, try a number between 5 - 20,
# but if you have lots of tight motion maybe you'd like to export everyframe.
# 


if 'cmds' not in locals():
	import maya.cmds as cmds
# import maya.mel as mel

class CloudsCameraPathExporter:
	output = "<!-- clouds camera path -->\n"
	tags = []
	spacer = ""

	def getEveryNFrames(self):
		result = cmds.promptDialog(
		message='how many frames per sample? ',
		button=['OK', 'Cancel'],
		defaultButton='OK',
		cancelButton='Cancel',
		dismissString='Cancel')

		if result == 'OK':
			text = int(cmds.promptDialog(query=True, text=True))
			return text

		return 5

	def pushTag(self, tag):
		self.tags.append( tag );
		self.output += self.spacer + "<" + str(tag) + ">\n"
		self.spacer += "	"

	def popTag(self):
		self.spacer = self.spacer[:-1]
		self.output += self.spacer + "</" + str( self.tags[len(self.tags)-1]) + ">\n"
		self.tags.pop()

	def addValue(self, name, val):
		self.output += self.spacer + "<" + str(name) + ">" + str(val) + "</" + name + ">\n"


	def export(self, everyNFrames=15):
		self.getCameraKeyframes( everyNFrames )

		fileLoc = cmds.fileDialog2(fileMode = 0, fileFilter = ".xml", selectFileFilter = ".xml")[0]; #"/Users/laserstorm/Desktop/temp.xml"	#
		t = open( fileLoc, "w" );

		# print self.output
		t.write( self.output )
		t.close()

	def getCurrentCamera(self):
		selected=cmds.ls(sl=1,fl=1)
		currentCamera = False
		if cmds.objectType( selected ) == "camera":
			currentCamera = selected	
		else:
			children = cmds.listRelatives( selected, c=1)
			
			for i in children:
				if cmds.objectType( i ) == "camera":
					currentCamera = i
		
		if( currentCamera == False and cmds.objectType("perspShape") == "camera" ):
			currentCamera = "perspShape"
		
		return currentCamera

	def addCameraInfo(self, currentCamera, _time):	
		cmds.currentTime( _time );

		target = cmds.camera( currentCamera, q=True, worldCenterOfInterest=True)
		translate = cmds.camera( currentCamera, q=True, position=True)
		up = cmds.camera( currentCamera, q=True, worldUp=True)

		self.pushTag("frame")
		self.addValue( "frame_number", int(_time) )
		self.addValue( "x", translate[0])
		self.addValue( "y", translate[1])
		self.addValue( "z", translate[2])	
		self.addValue( "tx", target[0])
		self.addValue( "ty", target[1])
		self.addValue( "tz", target[2])
		self.addValue( "upx", up[0])
		self.addValue( "upy", up[1])
		self.addValue( "upz", up[2])
		self.popTag()

	def getCameraKeyframes(self, everyNFrames=15):

		currentCamera = self.getCurrentCamera();
		print "exporting camera info for: " + currentCamera + " at " + str(everyNFrames) + " frames per sample"

		# cmds.select( currentCamera, r=True)
		cameraTransform = cmds.listRelatives( currentCamera, p=True )[0]

		startTime = cmds.playbackOptions( q=True, animationStartTime=True )
		endTime = cmds.playbackOptions( q=True, animationEndTime=True )

		i = startTime
		while i < endTime:
			self.addCameraInfo( currentCamera, i)
			i += everyNFrames
		self.addCameraInfo( currentCamera, endTime)


camPath = CloudsCameraPathExporter()
camPath.export( camPath.getEveryNFrames() )
