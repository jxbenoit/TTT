#ConvertNodesToJS.py
#Similar to ConvertNodesToJSON.py, but writes as a full JavaScript file.
#Since loading a JSON file into a web page requires running a web server,
#loading a ".js" file instead (using '<script src="myscripts.js"></script>')
#may be simpler.
import re                                  #for sub, match

InFile = open( "Nodes.txt", "r" )
OutFile = open( "Nodes.js", "w" )

Line = InFile.readline()
( NGamesPlayed, Size, NNodes ) = Line.split()

OutFile.write( "NodesJSON = {" )
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

OutFile.write( "]};" )

InFile.close()
OutFile.close()
