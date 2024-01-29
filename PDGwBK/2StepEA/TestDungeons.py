import io
import os

import numpy as np
from ctypes import CDLL
from ctypes import c_size_t, c_double, c_int

import glob
import sys

############################# FUNCTIONS TO CREATE MAZE IMAGE  #############################

import plotly.graph_objects as go

def separate_connection_barriers(X,Y, barrier):
  # barrier = {"1":[3,11]}  # {barrierID: [edge1, edge2, ...])
  # Since 2 position then None, we need to check the edge id
  NX = []
  NY = []
  BX = []
  BY = []
  # Now pass for all the posible Barriers
  label = list(range(len(X)//3))
  barrier_label = []
  barriers = []
  barriersID = []
  for key in barrier:
    for edge in barrier[key]:
      barriers.append(edge)
      barriersID.append(key)
  for x in range(len(label)):
    if(x in barriers):
      BX += [X[x*3],X[x*3+1], X[x*3+2]]
      BY += [Y[x*3],Y[x*3+1], Y[x*3+2]]
      barrier_label.append(barriersID[barriers.index(x)])
    else:
      NX += [X[x*3],X[x*3+1], X[x*3+2]]
      NY += [Y[x*3],Y[x*3+1], Y[x*3+2]]

  return [NX,NY],[BX,BY],barrier_label

def separate_nodes_by_type(Xn,Yn, keyRooms, labels):
  SX,SY = [],[]
  EX,EY = [],[]
  NX,NY = [],[]
  KX,KY = [],[]

  count = 0
  for x in labels:
    if x == "S":
      SX.append(Xn[count])
      SY.append(Yn[count])
    if x == "E":
      EX.append(Xn[count])
      EY.append(Yn[count])
    else:
      # {6:"1.1"}
      if x in keyRooms:
        KX.append(Xn[count])
        KY.append(Yn[count])
      else:
        NX.append(Xn[count])
        NY.append(Yn[count])
    count += 1
  return [[NX,NY],[KX,KY],[SX,SY],[EX,EY]]

# Helper function to create annotations for plotly
def make_annotations(pos, text, barriers, barrier_text, keyRooms, keyoffset=[0.01,0],  font_size=10, font_color='rgb(250,250,250)'):

    if len(text) != len(pos):
        raise ValueError('The lists pos and text must have the same len')
    annotations = []

    # Room Identifier annotations
    counter = 0
    for id in pos:
        annotations.append(
            dict(
                text=text[counter],
                x=pos[id][0],
                y=pos[id][1],
                xref='x1',
                yref='y1',
                font=dict(color=font_color, size=font_size),
                showarrow=False
            )
        )
        counter += 1

    # Connection Barrier Annotations
    offset = 0.05
    counter = 0
    for id in range(len(barrier_text)):
        annotations.append(
            dict(
                text=barrier_text[id],
                x= (barriers[0][id*3] + barriers[0][id*3+1])/2 + offset,
                y= (barriers[1][id*3] + barriers[1][id*3+1])/2 ,
                xref='x1',
                yref='y1',
                font=dict(color='rgb(0 ,0,0)', size=font_size),
                showarrow=False
            )
        )
        counter += 1

    # Room Keys Annotations
    offset = 0.05
    counter = 0
    for id in pos:
        currentRoom = text[counter]
        if(currentRoom in keyRooms):
          annotations.append(
              dict(
                  text=keyRooms[currentRoom],
                  x= pos[id][0] + keyoffset[0],
                  y= pos[id][1] + keyoffset[1],
                  xref='x1',
                  yref='y1',
                  font=dict(color='rgb(0 ,0,0)', size=font_size*1.2),
                  showarrow=False
              )
          )
        counter += 1
    return annotations

def separate_nodes_by_type_Map(roomPositions, keyRooms, initialRoom, finalRoom):
  # initialRoom = [RoomID] | finalRoom = [RoomID] | keyRooms = {roomID:keyID}
  SX,SY = [],[]
  EX,EY = [],[]
  NX,NY = [],[]
  KX,KY = [],[]

  dim = [0,0,0,0]
  labels = []
  count = 0
  for pos in roomPositions:
    if count in keyRooms:
      KX.append(roomPositions[count][0])
      KY.append(roomPositions[count][1])
      labels.append(str(count))
    elif count in initialRoom:
      SX.append(roomPositions[count][0])
      SY.append(roomPositions[count][1])
      labels.append("S")
    elif count in finalRoom:
      EX.append(roomPositions[count][0])
      EY.append(roomPositions[count][1])
      labels.append("E")
    else :
      NX.append(roomPositions[count][0])
      NY.append(roomPositions[count][1])
      labels.append(str(count))
    # dimensions
    if(dim[0] > roomPositions[count][0]):
      dim[0] = roomPositions[count][0]
    if(dim[1] > roomPositions[count][1]):
      dim[1] = roomPositions[count][1]
    if(dim[2] < roomPositions[count][0]):
      dim[2] = roomPositions[count][0]
    if(dim[3] < roomPositions[count][1]):
      dim[3] = roomPositions[count][1]
    count += 1
  return [[NX,NY],[KX,KY],[SX,SY],[EX,EY]], labels, dim

def get_Map_connections(roomPositions, edges, barriers):
  # [StartX, EndX, None, StartX, EndX, None ...]
  # [StartY, EndY, None, StartY, EndY, None ...]
  # barrier = {"1.TEST":[(0,2)]}
  NCX, NCY, BCX, BCY = [], [], [], []
  barrierTuple = []
  barrierTupleID = []
  BLabel = []
  for key in barriers:
    for edge in barriers[key]:
      barrierTuple.append(edge)
      barrierTupleID.append(key)
  for edge in edges:
    if edge in barrierTuple:
      BCX += [roomPositions[edge[0]][0],roomPositions[edge[1]][0] , None]
      BCY += [roomPositions[edge[0]][1],roomPositions[edge[1]][1] , None]
      BLabel.append(barrierTupleID[barrierTuple.index(edge)])
    else:
      NCX += [roomPositions[edge[0]][0],roomPositions[edge[1]][0] , None]
      NCY += [roomPositions[edge[0]][1],roomPositions[edge[1]][1] , None]
  return [NCX,NCY],[BCX, BCY], BLabel

def plot_map(roomPositions, edges, initialRoom, finalRoom, keyRooms, barriers):
    fig = go.Figure()

    # Colors and lines
    nodeType = [
                {"Nombre":"Normal","color":'#6175c1',"lcolor":"rgb(50,50,50)", "lwidth":2},
                {"Nombre":"Key","color":'#6175c1',"lcolor":"rgb(50,50,50)", "lwidth":4},
                {"Nombre":"Start","color":'#adb7db',"lcolor":"rgb(50,50,50)","lwidth":2},
                {"Nombre":"End","color":'#828fbf',"lcolor":"rgb(50,50,50)", "lwidth":0.5}
              ]

    # Reorganice the information to crete the map
    separated_nodes, labels, dim = separate_nodes_by_type_Map(roomPositions, keyRooms, initialRoom, finalRoom)
    N_connection, B_connection, B_labels = get_Map_connections(roomPositions, edges, barriers)
    # print(N_connection)
    # print(B_connection)

    # Add connections
    fig.add_trace(go.Scatter(
        x=N_connection[0],
        y=N_connection[1],
        name='Connection',
        mode='lines',
        line=dict(color='rgb(210,210,210)', width=2),
        hoverinfo='none'
    ))
    # Add Connections with Barriers
    fig.add_trace(go.Scatter(
        x=B_connection[0],
        y=B_connection[1],
        name='Barrier',
        mode='lines',
        line=dict(color='rgb(210,210,210)', width=6),
        hoverinfo='none'
    ))

    for i in range(len(separated_nodes)):
      # Add markers for room node type
      fig.add_trace(go.Scatter(
          x=separated_nodes[i][0],
          y=separated_nodes[i][1],
          mode='markers',
          name=nodeType[i]["Nombre"],
          marker=dict(
              symbol='square',
              size=25,
              color= nodeType[i]["color"],
              line=dict(color=nodeType[i]["lcolor"], width=nodeType[i]["lwidth"]),
          ),
          hoverinfo='none',
          opacity=0.8
      ))

    # Prepare the layout for the plot and annotations !!!
    axis = dict(showline=False, # hide axis line, grid, ticklabels and  title
            zeroline=False,
            showgrid=False,
            showticklabels=False,
            )
    axis = dict()

    fig.update_layout(
        title='Dungeon Tree',
        annotations=make_annotations(roomPositions, labels, B_connection, B_labels, keyRooms, [0.2,0.2]),
        font_size=12,
        xaxis=axis,
        yaxis=axis,
        margin=dict(l=40, r=40, b=85, t=100),
        hovermode='closest',
        plot_bgcolor='rgb(248,248,248)'
    )
    # print(dim)
    fig.update_xaxes(range=[dim[0] - 1, dim[2]+ 1])
    fig.update_yaxes(range=[dim[1] - 1, dim[3]+ 1])

    fig.show()

############################# MAIN  #############################
libfile = ''
# Get the current directory of the Python script
script_dir = os.path.dirname(os.path.abspath(__file__))

# Specify the relative path to the shared library
libfile = os.path.join(script_dir, 'pdgwbSO')
mylib = CDLL(libfile)

# First variable extracted manually to create the initial Array to obtain the dungeons !
# Size will be the number of rooms * 2(double the size in case of bad resutls) * 6 (id,x,y,sonL,sonD,sonR, KeyID) + 2 (last room Position)
nV = int(sys.argv[2]) # Number of rooms
maxRange = nV * 10 # Considered for testing and generation of extra rooms in the first generations
size = (maxRange * 7) + 2

if size < 20: # to pass all the variables !
  size = 20
R = np.full(size, -1.0)

# print(sys.argv[7] )
# Pass the values by args
if len(sys.argv) > 17:
  for i in range(1, 7):
      R[i - 1] = float(sys.argv[i]) 
  if sys.argv[7] == "true":
    R[6] = 1
  else:
    R[6] = 0
  R[7] = 0
  if sys.argv[8] == "Analisis":
    R[7] = 1
  for i in range(9,18):
    R[i - 1] = float(sys.argv[i]) 
    
R[17] = 0
R[18] = 1
R[19] = 0
displayFlag = sys.argv[-1] == "true"
# Analisis format
analisisSize = (3 * 2 * int(sys.argv[9])) + 5 
AR = np.full(analisisSize, -1.0)
# C-type corresponding to numpy array
mylib.CreateDungeons.argtypes = [np.ctypeslib.ndpointer(dtype=np.float64, shape=(size,)), np.ctypeslib.ndpointer(dtype=np.float64, shape=(analisisSize,)), c_int]
mylib.CreateDungeons(R, AR, size)

if (displayFlag):
  ### Create the way to be interpreted !
  # Helper
  roomIDNewID = {}
  keyRooms = {}
  barriersHelper = {}
  # Data for the representation
  roomPositions = {}
  edges = []          # [(0,1),(0,2),(1,4),(4,3), (3,6),(6,7), (7,8),(8,5) ]
  initialRoom = [0]   # [RoomID]
  finalRoom = []      # [roomID]
  keyRooms = {}       # {"2":"1.key"} 
  barriers = {}       # {"1.Key":[(4,3)]}  {barrierID: [(from, to))])

  finalRoomPos = [int(R[-2]), int(R[-1])]
  # 7 elements in the aray for each element of data per room
  # [roomID, X, Y, SonL, SonD, SonR, key]
  # Add Rooms
  for i in range(maxRange):
    base = i * 7
    # print("Data {}: [{},{}] Key?: {}".format(int(R[base]),int(R[base+ 1]),int(R[base+ 2]), int(R[base+6])))
    if(int(R[base]) == -1):
      continue
    roomPositions[i] = [int(R[base+ 1]),int(R[base+2])]
  # Add Edges, key positions and locked room
  for i in range(maxRange):
    base = i * 7
    if(int(R[base]) == -1):
      continue
    if(R[base + 3]>= 0):
      edges.append((int(R[base]), int(R[base + 3])))
    if(R[base + 4]>= 0):
      edges.append((int(R[base]), int(R[base + 4])))
    if(R[base + 5]>= 0):
      edges.append((int(R[base]), int(R[base + 5])))
    # roomIDNewID[int(R[base])] = i
    roomPositions[i] = [int(R[base+ 1]),int(R[base+2])]
    
    if(R[base + 6] > 0):
      # If its the same id, it is a locked room
      if(R[base] != R[base+6]):
        barriersHelper[int(R[base])] =  int(R[base+6])# Barrier
      else:
        keyRooms[str(int(R[base]))] = int(R[base+6])

    # Check final Room
    if (int(R[base+1]) == finalRoomPos[0] and int(R[base+2]) == finalRoomPos[1] and (finalRoomPos != 0 and finalRoomPos != 0)):
      finalRoom.append(i)

  # locked edges
  for i in range(maxRange):
    base = i * 7
    if(R[base + 3]>= 0 and int(R[base+3] in barriersHelper)):
      barriers[barriersHelper[(int(R[base+3]))]] = [(int(R[base]),int(R[base+3]))]
    if(R[base + 4]>= 0 and int(R[base+4] in barriersHelper)):
      barriers[barriersHelper[(int(R[base+4]))]] = [(int(R[base]),int(R[base+4]))]
    if(R[base + 5]>= 0 and int(R[base+5] in barriersHelper)):
      barriers[barriersHelper[(int(R[base+5]))]] = [(int(R[base]),int(R[base+5]))]

  # Create Maps !
  plot_map(roomPositions, edges, initialRoom, finalRoom, keyRooms, barriers)
  
