#import sys                                 #for sys.argv, sys.exit
#from os.path import basename, splitext
import re                                  #for sub, match

InFile = open( "Nodes.txt", "r" )
OutFile = open( "Nodes.json", "w" )

Line = InFile.readline()
( NGamesPlayed, Size, NNodes ) = Line.split()

OutFile.write( "{" )
OutFile.write( f'"NGamesPlayed":%s,"Size":%s,"NNodes":%s' % \
                  (NGamesPlayed, Size, NNodes) )
OutFile.write( ',"Nodes":[' )

Lines = InFile.readlines()
Count = 0
for L in Lines:
  if Count > 0:
    OutFile.write( "," )
  Count += 1

  m = re.match(
           "^\s*(\d+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\S+)\s+(\w)\s+(.*)\s*$", L )
  id = m.group(1)
  grid = m.group(2)
  status = m.group(3)
  nwins = m.group(4)
  nties = m.group(5)
  complete = m.group(6)
  links = m.group(7).split()

  #A '-4' in the links is a delimiter between child nodes and non-child nodes.
  #Since this distinction only matters when its necessary to recursively
  #delete the node tree/map/graph, it's not needed in JavaScript (since it
  #uses garbage collection).
  #Also don't need to know if this is a '-1' (loss), '-2' (win), or '-3'
  #(tie).
  links = [e for e in links if int(e) >= 0]

  OutFile.write( '[%s,"%s","%s",%s,%s,"%s",[' % \
                 (id, grid, status, nwins, nties, complete) )

  LinkCount = 0
  for l in links:
    if LinkCount > 0:
      OutFile.write( "," )
    LinkCount += 1

    OutFile.write( l )
  
  OutFile.write( "]]" )

#   print( f"[%s] [%s] [%s] [%s] [%s] [%s] " %
#          (id, grid, status, nwins, nties, complete), end='' )
#   print( f"<%s>" % links )

OutFile.write( "]}" )

InFile.close()
OutFile.close()
