'''
this changes a mesh's tessellation by subdividing it and 
then deleting it original edges.  select  one or more meshs
and run this script.
------------------------------------------------------------
The script was written by:
Lars Berg 2008. 

larsberg@thelablab.com

'''

def  hexalate():
    list=cmds.ls(sl=1,fl=1)
    for i in range (len(list)):
        cmds.polyTriangulate(list[i], ch = True)
        edgeCount = cmds.polyEvaluate( e=1)
        cmds.select( (list[i] + '.e[0:' + str(edgeCount) + ']'), r=True)
        hexEdgeSet = cmds.sets(name = 'edgeSetHex')
        cmds.polySubdivideFacet( list[i], dv = 1, m=0, ch=1)
        cmds.select(hexEdgeSet, r=1)
        cmds.polyDelEdge(cv=1, ch=1)
        cmds.select(hexEdgeSet, r = 1, ne = 1)
        cmds.delete()
        cmds.select(list)
hexalate()
        