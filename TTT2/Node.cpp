/*Node
*/
#ifndef NODE_CPP
#define NODE_CPP

#include<iostream>
#include<iomanip>
#include<fstream>
#include<sstream>
#include"ShowGrid.h"
#include"Node.h"

using namespace std;

const int GRID_SIZE = 9;
const char BLANK = '.';
const char SIDE1 = 'X';
const char SIDE2 = 'O';

static int NextNodeId = 0;

//---------------------------------------------------------------------------
Node::Node( int Size, bool FillWithBlanks=false ) {
  Id = NextNodeId++;
  this->Size = Size;
  Grid = new char [(unsigned)Size];
  Next1 = Next2 = nullptr;
  Parent = nullptr;
  Next1NodeIds = nullptr;
  Next2NodeIds = nullptr;
  NNext1 = NNext2 = NWins = NTies = 0;
  NUntried = Size;
  PathComplete = false;
  Status = NONE;

  if( FillWithBlanks )
    for( int i = 0; i < Size; i++ )
      Grid[i] = BLANK;
cout << "Node::Node 1: Node #" << Id << " created: ";             //AAA
ShowGrid( Grid, Size, true );                                     //AAA
cout << endl;                                                     //AAA
} //Node::Node

//---------------------------------------------------------------------------
Node::Node( char Grid[], int Size, Node *Parent ) {
  Id = NextNodeId++;
  this->Size = Size;
  this->Grid = new char [(unsigned)Size];
  for( int i = 0; i < Size; i++ )
    this->Grid[i] = Grid[i];
  Next1 = Next2 = nullptr;
  this->Parent = Parent;
  Next1NodeIds = nullptr;
  Next2NodeIds = nullptr;
  NNext1 = NNext2 = NWins = NTies = 0;
  PathComplete = false;
  Status = NONE;

  for( int i = 0; i < Size; i++ )
    if( Grid[i] == BLANK )
      NUntried++;

cout << "Node::Node 1: Node #" << Id << " created: ";             //AAA
ShowGrid( Grid, Size, true );                                     //AAA
cout << endl;                                                     //AAA
} //Node::Node

//---------------------------------------------------------------------------
//Node::~Node
//Deletes this node and all nodes it is a parent of.
Node::~Node() {
cout << "Node::~Node 1: Deleting Node #" << Id << endl;  //AAA
  delete [] Grid;

  if( Next1 ) {
    for( int i = 0; i < NNext1; i++ ) {
cout << "Node::~Node 4:   i=" << i << endl;  //AAA
      if( Next1[i] != nullptr ) {
cout << "Node::~Node 5:   Deleting SubNode #" << Next1[i]->GetId() << endl;  //AAA
        delete Next1[i];
      }
    }
    delete [] Next1;
  }

  if( Next1NodeIds )
    delete [] Next1NodeIds;
  if( Next2NodeIds )
    delete [] Next2NodeIds;
} //Node::~Node

//---------------------------------------------------------------------------
bool  Node::ReadTree( string FileName, int &NGamesPlayed ) {
  ifstream InFile( FileName );
  if( InFile.good() ) {
    int nnodes;

    //Note: It's assumed Size doesn't change among all nodes.
    InFile >> NGamesPlayed >> Size >> nnodes;
    this->ReadNode( InFile );  //Read root node

    //Read child nodes.
    Node **N = new Node* [(unsigned)nnodes];
    N[0] = this; //Points back to this (root) node
    for( int i = 0; i < nnodes-1; i++ ) {
      N[i+1] = new Node( Size );
      N[i+1]->ReadNode( InFile );
    }

    //Arrange child nodes into tree.
    for( int i = 0; i < nnodes; i++ )
      N[i]->ConnectNextNodes( N, nnodes );

    InFile.close();
    return true;
  }

  return false;
} //Node::ReadTree

//---------------------------------------------------------------------------
//Node::WriteTree
//Note default for Diag is false.
bool  Node::WriteTree( string FileName, int NGamesPlayed, bool Diag ) {
  ofstream OutFile( FileName );
  if( OutFile.good() ) {
    int nnodes = GetNumNodes();

    //Note: It's assumed Size doesn't change among all nodes.
    OutFile << NGamesPlayed << " " << Size << " " << nnodes << endl;
    WriteNode( OutFile );
    OutFile.close();
    return true;
  }

  return false;
} //Node::WriteTree

//---------------------------------------------------------------------------
//Node::WriteNode
//Recursive function to write a node tree.
void  Node::WriteNode( ofstream& OutFile ) {
  OutFile << setw(5) << Id << " ";

  for( int i = 0; i < Size; i++ )
    OutFile << Grid[i];

  //Output number of wins & if this path is complete.
  char s;
  if( Status == NONE ) s = '?';
  else if( Status == WIN ) s = 'W';
  else if( Status == LOSS ) s = 'L';
  else if( Status == TIE ) s = 'T';
  else if( Status == MIXED ) s = 'M';
  else
    cout << "Node:WriteNode: ** Odd Status value for Node #" << Id
         << ": " << Status << " **" << endl;

  OutFile << " " << s;
  OutFile << " " << NWins << " " << NTies << " " << ( PathComplete?'t':'f' );

  if( Next1 != nullptr || Next2 != nullptr ) {
    if( Next1 ) {
      for( int i = 0; i < NNext1; i++ )
        OutFile << " " << Next1[i]->GetId();
    }

    if( Next2 ) {
      OutFile << " " << -4;
      for( int i = 0; i < NNext2; i++ )
        OutFile << " " << Next2[i]->GetId();      
    }

    OutFile << endl;

    //Write child nodes.
    for( int i = 0; i < NNext1; i++ )
      Next1[i]->WriteNode( OutFile );
  }
  else {
    if( PathComplete ) {
      if( NTies > 0 )
        OutFile << " " << -3;  //Tie
      else if( NWins == 0 )
        OutFile << " " << -1;  //Loss
      else if( NWins > 0 )
        OutFile << " " << -2;  //Win
    }
    else  //Path is incomplete
      OutFile << " " << -3;  //Tie
    
    OutFile << endl;
  }
} //Node::WriteNode

//---------------------------------------------------------------------------
//Node::ReadNode
void  Node::ReadNode( ifstream& InFile ) {
  InFile >> Id;

  char s1[Size+1];
  InFile.read( s1, sizeof(s1) );
  for( int i = 0; i < Size; i++ )
    Grid[i] = s1[i+1];

  char s;
  InFile >> s;
  if( s == '?' ) Status = NONE;
  else if( s == 'W' ) Status = WIN;
  else if( s == 'L' ) Status = LOSS;
  else if( s == 'T' ) Status = TIE;
  else if( s == 'M' ) Status = MIXED;
  else
    cout << "Node:ReadNode: ** Odd Status value for Node #" << Id
         << ": " << Status << endl;

  //Count spaces left.
  int nblanks = 0;
  for( int i = 0; i < Size; i++ )
    if( Grid[i] == BLANK )
      nblanks++;

  char complete;
  InFile >> NWins >> NTies >> complete;
  if( complete == 't' )
    PathComplete = true;
  else if( complete == 'f' )
    PathComplete = false;
  else
    cout << "Node::ReadNode: ** Odd 'PathComplete' value: " << complete
         << " **" << endl;    

  string s2;
  getline( InFile, s2 );
  istringstream is( s2 );
  int n;
  bool nonparent_nodes = false;
  NNext1 = NNext2 = 0;
  while( is >> n ) {
    if( n < 0 ) {
      PathComplete = true;
      if( n == -1 ) {  //All losses or ties down this path
                       //Should be no more numbers in line after this
        NWins = 0;
      }
      else if( n == -3 ) {  //Tie
        NWins = 0;
        NTies = 1;
      }
      else if( n == -4 )  //Delimiter separating child & non-child nodes
        nonparent_nodes = true;
    }
    else {
      if( ! nonparent_nodes ) {
        if( Next1NodeIds == nullptr )
          Next1NodeIds = new int [(unsigned)nblanks];
        Next1NodeIds[NNext1++] = n;
      }
      else {
        if( Next2NodeIds == nullptr )
          Next2NodeIds = new int [(unsigned)nblanks];
        Next2NodeIds[NNext2++] = n;        
      }
    }
  }

  if( PathComplete )
    NUntried = 0;     //By definition, there must be no untried paths left

  if( NWins == 0 && NTies == 0 && PathComplete && NNext1 == 0 && NNext2 == 0 )
    Status = LOSS; //This node is a loss 'leaf'
} //Node::ReadNode

//---------------------------------------------------------------------------
void  Node::ConnectNextNodes( Node* Nodes[], int NNodes ) {
  for( int i = 0; i < NNext1; i++ ) {
    bool node_found = false;
    for( int j = 0; j < NNodes && ! node_found; j++ ) {
      if( Nodes[j]->GetId() == Next1NodeIds[i] ) {
        if( Next1 == nullptr )
          Next1 = new Node* [(unsigned)Size];

        Next1[i] = Nodes[j];
        if( Next1[i]->GetParent() == nullptr )
          Next1[i]->SetParent( this );

        node_found = true;
      }
    }
  }

  for( int i = 0; i < NNext2; i++ ) {
    bool node_found = false;
    for( int j = 0; j < NNodes && ! node_found; j++ ) {
      if( Nodes[j]->GetId() == Next2NodeIds[i] ) {
        if( Next2 == nullptr )
          Next2 = new Node* [(unsigned)Size];

        Next2[i] = Nodes[j];

        node_found = true;
      }
    }
  }
} //Node::ConnectNextNodes

//---------------------------------------------------------------------------
int   Node::GetSize() { return Size; }

//---------------------------------------------------------------------------
int   Node::GetId() { return Id; }

//---------------------------------------------------------------------------
int   Node::GetNumWins() { return NWins; }

//---------------------------------------------------------------------------
int   Node::GetNumTies() { return NTies; }

//---------------------------------------------------------------------------
int   Node::GetNumUntriedPaths() { return NUntried; }

//---------------------------------------------------------------------------
bool  Node::IsPathComplete() { return PathComplete; }

//---------------------------------------------------------------------------
char  Node::GetStatus() { return Status; }

//---------------------------------------------------------------------------
char  Node::GetCellContent( int index ) { return Grid[index]; }

//---------------------------------------------------------------------------
void  Node::SetParent( Node *Parent ) {
  this->Parent = Parent;
} //Node::SetParent

//---------------------------------------------------------------------------
Node* Node::GetParent() { return Parent; }

//---------------------------------------------------------------------------
//Node::GetNumNodes
//Counts the number of nodes including and under this node.
int   Node::GetNumNodes() {
  int nnodes = 1;
  for( int i = 0; i < NNext1; i++ )
    nnodes += Next1[i]->GetNumNodes();

  return nnodes;
} //Node::GetNumNodes

//---------------------------------------------------------------------------
Node* Node::AddAsChildNode( char Grid[], int Size ) {
  if( NNext1 == 0 )
    Next1 = new Node* [(unsigned)Size];

  Next1[NNext1++] = new Node( Grid, Size, this );

  return Next1[NNext1-1];
} //Node::AddAsChildNode

//---------------------------------------------------------------------------
void  Node::AddAsNonChildNode( Node *p ) {
  if( NNext2 == 0 )
    Next2 = new Node* [(unsigned)Size];

  Next2[NNext2++] = p;  
} //Node::AddAsNonChildNode

//---------------------------------------------------------------------------
void  Node::CopyGrid( char OutGrid[] ) {
  for( int i = 0; i < Size; i++ )
    OutGrid[i] = Grid[i];
} //Node::CopyGrid

//---------------------------------------------------------------------------
void  Node::CopyGrid( char InGrid[], char OutGrid[], int Size ) {
  for( int i = 0; i < Size; i++ )
    OutGrid[i] = InGrid[i];
} //Node::CopyGrid

//---------------------------------------------------------------------------
void  Node::SetStatus( char Status ) {
  this->Status = Status;
  if( Status == WIN || Status == LOSS || Status == TIE || Status == MIXED ) {
    PathComplete = true;
    if( Status == WIN || Status == LOSS || Status == MIXED ) {
      NUntried = 0;
      if( Status == WIN )
        NWins++;
      else
        NWins = 0;
    }
    else if( Status == TIE )
      NTies++;
  }
} //Node::SetStatus

//---------------------------------------------------------------------------
void  Node::MarkParentAsLoss() {
  if( Parent != nullptr )
    Parent->SetStatus( LOSS );
  else
    cout << "Node::MarkParentAsLoss: ** Parent is null! **" << endl;
} //Node:MarkParentAsLoss

//---------------------------------------------------------------------------
//Node::CheckForLossNextMove
//If there is a loss in the next move return true, else false.
bool  Node::CheckForLossNextMove() {
  for( int i = 0; i < NNext1; i++ ) {
    if( Next1[i]->GetStatus() == LOSS )
      return true;
  }
  for( int i = 0; i < NNext2; i++ ) {
    if( Next2[i]->GetStatus() == LOSS )
      return true;
  }

  return false;
} //Node::CheckForLossNextMove

//---------------------------------------------------------------------------
//Node::UpdateStatus
//Update Status, NUntried and PathComplete.
void  Node::UpdateStatus() {
  //If status is already a loss, keep it that way. A loss is a loss.
  //Otherwise, the algorithm may pursue incomplete paths down this path,
  //hoping for a win.
  if( Status == LOSS ) {
    PathComplete = true;
    return;
  }

  //Count spaces left.
  int nblanks = 0;
  for( int i = 0; i < Size; i++ )
    if( Grid[i] == BLANK )
      nblanks++;
  
  //Any untried moves left?
  NUntried = nblanks - NNext1 - NNext2;

  bool complete = true, all_subnodes_losses = true, all_subnodes_wins = true,
       all_subnodes_ties = true;
  for( int i = 0; i < NNext1 && complete; i++ ) {
    if( ! Next1[i]->IsPathComplete() )
      complete = false;  //A subpath is incomplete, so this node's path is
    else {
      char stat = Next1[i]->GetStatus();
      if( stat == WIN ) {
        all_subnodes_losses = false;
        all_subnodes_ties = false;
      }
      else if( stat == TIE ) {
        all_subnodes_losses = false;
        all_subnodes_wins = false;
      }
      else if( stat == LOSS ) {
        all_subnodes_ties = false;
        all_subnodes_wins = false;
      }
      else {
        all_subnodes_ties = false;
        all_subnodes_wins = false;
        all_subnodes_losses = false;
      }
    }
  }

  for( int i = 0; i < NNext2 && complete; i++ ) {
    if( ! Next2[i]->IsPathComplete() )
      complete = false;  //A subpath is incomplete, so this node's path is
    else {
      char stat = Next2[i]->GetStatus();
      if( stat == WIN ) {
        all_subnodes_losses = false;
        all_subnodes_ties = false;
      }
      else if( stat == TIE ) {
        all_subnodes_losses = false;
        all_subnodes_wins = false;
      }
      else if( stat == LOSS ) {
        all_subnodes_ties = false;
        all_subnodes_wins = false;
      }
      else {
        all_subnodes_ties = false;
        all_subnodes_wins = false;
        all_subnodes_losses = false;
      }
    }
  }

  PathComplete = (NUntried == 0) && complete;

  if( PathComplete ) {
    if( all_subnodes_losses )
      SetStatus( LOSS );
    else if( all_subnodes_wins )
      SetStatus( WIN );
    else if( all_subnodes_ties )
      SetStatus( TIE );
    else
      SetStatus( MIXED );
  }
} //Node::UpdateStatus

//---------------------------------------------------------------------------
Node* Node::ChooseMove( int Marker ) {
  Node *New_pNode = nullptr;

  UpdateStatus();

  //Find move with maximum wins.
  //Use Win bools from nodes and NWins arrays.
  //'Bubble up' NWins from next nodes.
  NWins = NTies = 0;
  int max_next_nwins = 0, num_max_next_nwins = 0;
  for( int i = 0; i < NNext1; i++ ) {
    NTies += Next1[i]->GetNumTies();
    int next_nwins = Next1[i]->GetNumWins();
    NWins += next_nwins;
    if( next_nwins > max_next_nwins ) {
      max_next_nwins = next_nwins;
      num_max_next_nwins = 0;
    }
    else if( next_nwins == max_next_nwins )
      num_max_next_nwins++;
  }
  for( int i = 0; i < NNext2; i++ ) {
    NTies += Next2[i]->GetNumTies();
    int next_nwins = Next2[i]->GetNumWins();
    NWins += next_nwins;
    if( next_nwins > max_next_nwins ) {
      max_next_nwins = next_nwins;
      num_max_next_nwins = 0;
    }
    else if( next_nwins == max_next_nwins )
      num_max_next_nwins++;
  }

  //Total 'points' for this node's subpaths. If subpath:
  //  is complete and has 0 wins  - reward 0 pts (loss or ties down this path)
  //  is complete and has >0 wins - reward 2 points for each win
  //  is incomplete               - reward 1 pt plus
  //                                reward 2 points for each win and 1 point
  //                                for each untried subsubpath
  int npoints = NUntried;
cout << "ChooseMove 1: In Node #" << Id << endl;;
  for( int i = 0; i < NNext1; i++ ) {
    /*Check subsubnodes of this subnode, which are paths player decides
      after this AI move. If any are marked as losses (inescapable), mark
      this subnode as a loss.
      E.g. if the grid is XX0
                          ...
                          ...
      all next nodes EXCEPT the one for choosing the center cell should be
      marked as losses.

      If the move AFTER the next move is player's, AND it's marked as a loss,
      mark this next node as a loss so it isn't chosen by the AI.
    */
    if( Next1[i]->CheckForLossNextMove() ) {
cout << "ChooseMove 2:   Subnode #" << Next1[i]->GetId() << " has Loss next move" << endl; //AAA
      Next1[i]->SetStatus( LOSS );
    }
    else {
      npoints += Next1[i]->GetNumWins() * 2;
      Next1[i]->UpdateStatus();  //Update PathComplete
char stat = Next1[i]->GetStatus();                                          //AAA
cout << "ChooseMove 3:   Subnode #" << Next1[i]->GetId() << " status is " //AAA
     << ((stat==WIN)?'W':((stat==LOSS)?'L':(stat==TIE)?'T':(stat==MIXED)?'M':'?')) << endl; //AAA
      if( ! Next1[i]->IsPathComplete() || Next1[i]->GetStatus() == MIXED ) {
        npoints++;   //Add 1 pt for just having incomplete path
        npoints += Next1[i]->GetNumUntriedPaths();
      }
    }
  }

  for( int i = 0; i < NNext2; i++ ) {
    /*Check subsubnodes of this subnode, which are paths player decides
      after this AI move. If any are marked as losses (inescapable), mark
      this subnode as a loss.
      E.g. if the grid is XX0
                          ...
                          ...
      all next nodes EXCEPT the one for choosing the center cell should be
      marked as losses.

      If the move AFTER the next move is player's, AND it's marked as a loss,
      mark this next node as a loss so it isn't chosen by the AI.
    */
    if( Next2[i]->CheckForLossNextMove() ) {
cout << "ChooseMove 4:   Subnode #" << Next2[i]->GetId() << " has Loss next move" << endl; //AAA
      Next2[i]->SetStatus( LOSS );
    }
    else {
      npoints += Next2[i]->GetNumWins() * 2;
      Next2[i]->UpdateStatus();  //Update PathComplete
char stat = Next1[i]->GetStatus();                                          //AAA
cout << "ChooseMove 5:   Subnode #" << Next1[i]->GetId() << " status is " //AAA
     << ((stat==WIN)?'W':((stat==LOSS)?'L':(stat==TIE)?'T':(stat==MIXED)?'M':'?')) << endl; //AAA
      if( ! Next2[i]->IsPathComplete() || Next2[i]->GetStatus() == MIXED ) {
        npoints++;   //Add 1 pt for just having incomplete path
        npoints += Next2[i]->GetNumUntriedPaths();
      }
    }
  }
  
  //Randomly choose a 'best' move (one with max wins, or untried).
  int c;
  if( npoints > 0 ) {
    c = rand() % npoints + 1; 
    for( int i = 0; i < NNext1 && c > 0; i++ ) {
      c -= Next1[i]->GetNumWins() * 2;
      //Note that a node path is marked complete if it is a loss, even if
      //there are untried spaces.
      if( ! Next1[i]->IsPathComplete() || Next1[i]->GetStatus() == MIXED )
        c -= Next1[i]->GetNumUntriedPaths() + 1;
      if( c <= 0 )
        New_pNode = Next1[i];
    }

    for( int i = 0; i < NNext2 && c > 0; i++ ) {
      c -= Next2[i]->GetNumWins() * 2;
      //Note that a node path is marked complete if it is a loss, even if
      //there are untried spaces.
      if( ! Next2[i]->IsPathComplete() || Next2[i]->GetStatus() == MIXED )
        c -= Next2[i]->GetNumUntriedPaths() + 1;
      if( c <= 0 )
        New_pNode = Next2[i];
    }
cout << "ChooseMove 6:   Chose Subnode #" << New_pNode->GetId() << endl; //AAA
  }
  else { //no points -- that is, no winning paths; look for ties
    for( int i = 0; i < NNext1; i++ )
      npoints += Next1[i]->GetNumTies();
    for( int i = 0; i < NNext2; i++ )
      npoints += Next2[i]->GetNumTies();

    if( npoints > 0 ) {
      c = rand() % npoints + 1;
      for( int i = 0; i < NNext1 && c > 0; i++ ) {
        c -= Next1[i]->GetNumTies();
        if( c <= 0 )
          New_pNode = Next1[i];
      }

      for( int i = 0; i < NNext2 && c > 0; i++ ) {
        c -= Next2[i]->GetNumTies();
        if( c <= 0 )
          New_pNode = Next2[i];
      }
    }
    else {  //No ties either; choose move at random
      c = rand() % (NNext1 + NNext2);
      if( c < NNext1 )
        New_pNode = Next1[ c ];
      else
        New_pNode = Next2[ c ];
    }
cout << "ChooseMove 7:   No winning paths. Chose Subnode #" << New_pNode->GetId() << endl; //AAA
  }

  if( New_pNode == nullptr ) {  //Choose an untried path
    char new_grid[Size];
    for( int i = 0; i < Size; i++ ) {
      if( Grid[i] == BLANK ) {
        //Check against subpaths seen so far to see if this one has been done
        //before.
        bool already_tried = false;
        for( int j = 0; j < NNext1 && ! already_tried; j++ )
          if( Next1[j]->GetCellContent(i) != BLANK )
            already_tried = true;
        for( int j = 0; j < NNext2 && ! already_tried; j++ )
          if( Next2[j]->GetCellContent(i) != BLANK )
            already_tried = true;

        if( ! already_tried ) {
          if( --c == 0 )
            new_grid[i] = Marker;
          else new_grid[i] = Grid[i];
        }
        else new_grid[i] = Grid[i];
      }
      else new_grid[i] = Grid[i];
    }

    New_pNode = AddAsChildNode( new_grid, Size );
cout << "ChooseMove 8:   Chose untried path: ";                    //AAA
ShowGrid(new_grid, Size);                                          //AAA
cout << endl;                                                      //AAA
  }

  return New_pNode;
} //Node::ChooseMove

//---------------------------------------------------------------------------
//Node::Find
//Note default for Recurse is false.
//
Node* Node::Find( char TestGrid[], int Size, bool Recurse ) {

  //Check if current node matches.
  if( Compare(TestGrid, Size) ) {
cout << "Node::Find 1: Found match with Node #" << Id << "!" << endl;   //AAA
    return this;
  }

  //Look for a match (just the next level of nodes).
  //If no match found, return nullptr.
  if( Next1 || Next2 ) {
    for( int i = 0; i < NNext1; i++ ) {
      if( Next1[i]->Compare(TestGrid, Size) ) {
cout << "Node::Find 2: Found match with Node #" << Next1[i]->GetId() << "!" << endl;   //AAA
        return Next1[i];
      }
    }
    for( int i = 0; i < NNext2; i++ ) {
      if( Next2[i]->Compare(TestGrid, Size) ) {
cout << "Node::Find 3: Found match with Node #" << Next2[i]->GetId() << "!" << endl;   //AAA
        return Next2[i];
      }
    }

    //Note recursion only goes through child subnodes, not non-child subnodes.
    if( Recurse ) {
      for( int i = 0; i < NNext1; i++ ) {
        Node *p = Next1[i]->Find( TestGrid, Size, true );
        if( p != nullptr ) {
          cout << "Node::Find 4: Node #" << Id << " Returning subnode #" << p->GetId() << endl; //AAA
          return p;
        }
//return nullptr; //AAA
      }
    }
  }
  
  return nullptr;
} //Node::Find

//---------------------------------------------------------------------------
//Node::Compare
//Main compare function. Test node's grid against a test grid.
bool  Node::Compare( char TestGrid[], int Size ) {
  for( int i = 0; i < Size; i++ )
    if( TestGrid[i] != this->Grid[i] )
      return false;

  return true;
} //Node::Compare

#endif