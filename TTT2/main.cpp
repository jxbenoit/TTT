/*AI Tic-Tac-Toe - V1.0 - Dec27, 2022
  In this version:
    * No advanced transitions between nodes -- i.e. Flipping, Rotating, and
      Swapping sides. Just concentrating on Node class and straight links
      to other nodes.
    * ChooseMove first searches for Wins and untried paths. If none are found,
      it will look at Ties and choose among them if available.
    * Cleaned up code; removed unneeded functions.
    * Added better comments.
    * Added MIXED status for nodes (in addition to NONE, WIN, TIE, and LOSS).
      This would be for non-leaf nodes.
    * Added capability to link nodes in different branches of node tree
      (creating a node 'web' or graph).

  To do:
    * Build 'trainer' version of code to build a more complete Nodes.txt file.
    * Add (back) Swap Sides, Flip (horizontal, vertical, diagonal),
      Rotate by maybe creating a 'Link' class. Nodes would have arrays of
      links rather than to other nodes directly.
    * Allow player to switch sides (O's instead of X's).
    * Allow player to decide who starts.

  Note: Mobile-C app's C++ compiler gives an error if memory is dynamically
  allocated given an int for size, e.g. 'Grid = new char [Size];'. This size
  variable seems to need to be cast as unsigned, so that is what is done here.
*/
#include<iostream>
#include<iomanip>
#include<cmath>
#include"Node.h"
#include"ShowGrid.h"

using namespace std;

const int GRID_SIZE = 9;
const char BLANK = '.';
const char SIDE1 = 'X';
const char SIDE2 = 'O';

void GetPlayerMove( char Grid[], int Size, char Marker, Node **CurrState,
                    Node *Root );
void GetAIMove( char Grid[], int Size, char Marker, Node **CurrState,
                Node *Root );
bool CheckWin( char Grid[], int Size, char Marker );

//===========================================================================
int main( int argc, char** argv ) {
  //Set up node tree or load tree.
  string NodeFileName = "Nodes.txt";
  int NGamesPlayed = 0;
  Node *Root = new Node( GRID_SIZE, true );
  if( ! Root->ReadTree(NodeFileName, NGamesPlayed) )
    cout << "*** Can't open " << NodeFileName << " for reading. ***" << endl;

  Node *CurrState;

  //Main loop through multiple games.
  bool KeepPlaying = false;
  do {
    NGamesPlayed++;
    cout << "--------------------------------------------------------------\n"
         << "Game #" << (NGamesPlayed) << "\n" << endl;

    CurrState = Root;  //Reset current state to root

    //Init grid.
    char Grid[GRID_SIZE];
    for( int i = 0; i < GRID_SIZE; i++ )
      Grid[i] = BLANK;

    int Turn = 0;
    bool Side1Won = false, Side2Won = false;
    do {
      Turn++;
      GetPlayerMove( Grid, GRID_SIZE, SIDE1, &CurrState, Root );
      if( CheckWin(Grid, GRID_SIZE, SIDE1) )
        Side1Won = true;
      else if( Turn < GRID_SIZE ) {
        Turn++;
        GetAIMove( Grid, GRID_SIZE, SIDE2, &CurrState, Root );

        if( CheckWin(Grid, GRID_SIZE, SIDE2) )
          Side2Won = true;
      }
    } while( Turn < GRID_SIZE && ! Side1Won && ! Side2Won );

    ShowGrid( Grid, GRID_SIZE );  //Show grid one final time

    if( Side1Won ) {
      cout << "Player won!" << endl;
      CurrState->SetStatus( Node::LOSS );
      CurrState->MarkParentAsLoss();
    }
    else if( Side2Won ) {
      cout << "Computer won!" << endl;
      CurrState->SetStatus( Node::WIN );
    }
    else {
      cout << "It's a tie!" << endl;
      CurrState->SetStatus( Node::TIE );
    }

    //Play again?
    char yorn;
    do {
      cout << "Keep playing? (y/n): ";
      cin >> yorn;
      cin.clear();
      cin.ignore(1000, '\n');      // Need the '\n' 

      if( yorn == 'y' || yorn == 'Y' )
        KeepPlaying = true;
      else if( yorn == 'n' || yorn == 'N' )
        KeepPlaying = false;
      else
        cout << "Please enter 'y' or 'n'." << endl;
    } while( yorn != 'y' && yorn != 'Y' && yorn != 'n' && yorn != 'N' );
  } while( KeepPlaying );

  //Save tree.
  if( ! Root->WriteTree(NodeFileName, NGamesPlayed) )
    cout << "*** Can't open " << NodeFileName << " for writing! ***" << endl;
 //Save diagnostic version.
  if( ! Root->WriteTree("Nodes.txt", NGamesPlayed, true) )
    cout << "*** Can't open " << "Nodes.txt" << " for writing! ***" << endl;

  delete Root;

  return 0;
} //main

//---------------------------------------------------------------------------
void GetAIMove( char Grid[], int Size, char Marker, Node **CurrState,
                Node *Root ) {
  Node *new_p = (*CurrState)->Find( Grid, Size );

  //If no matching state found; try searching from root of node tree.
  if( new_p == nullptr ) {

    new_p = Root->Find( Grid, Size, true );

    if( new_p != nullptr )
      (*CurrState)->AddAsNonChildNode( new_p );  //Add link to this node
  }

  //If still no matching state found; create one.
  if( new_p == nullptr ) {
    (*CurrState) = (*CurrState)->AddAsChildNode( Grid, Size );
  }
  else {
    *CurrState = new_p;
  }

  (*CurrState) = (*CurrState)->ChooseMove( Marker );
cout << "GetAIMove 1: Moving CurrState to Node #" << (*CurrState)->GetId() << endl; //AAA
  (*CurrState)->CopyGrid( Grid );
} //GetAIMove

//---------------------------------------------------------------------------
bool CheckWin( char Grid[], int Size, char Marker ) {
  int side_len = sqrt( Size );

  //Check rows.
  for( int i = 0; i < side_len; i++ ) {
    int j = 0;
    for( ; j < side_len && Grid[i*side_len+j] == Marker; j++ );
    if( j == side_len ) return true;
  }

  //Check columns.
  for( int j = 0; j < side_len; j++ ) {
    int i = 0;
    for( ; i < side_len && Grid[i*side_len+j] == Marker; i++ );
    if( i == side_len ) return true;
  }

  //Check diagonals.
  int x = 0;
  for( ; x < side_len && Grid[x*side_len+x] == Marker; x++ ) {}
  if( x == side_len ) return true;

  x = side_len - 1;
  for( ; x >= 0 && Grid[x*side_len+(side_len-1-x)] == Marker; x-- ) {}
  if( x < 0 ) return true;

  return false;
} //CheckWin

//---------------------------------------------------------------------------
//GetPlayerMove
void GetPlayerMove( char Grid[], int Size, char Marker, Node **CurrState,
                    Node *Root ) {
  int side_len = sqrt( Size );
  bool valid_move = false;
  int r, c;
  do {
    ShowGrid( Grid, Size );
    cout << "Enter move (row, column): ";
    cin >> r >> c;
    cin.clear();
    cin.ignore(1000, '\n');      // Need the '\n' 

    if( r < 0 || r >= side_len || c < 0 || c >= side_len )
      cout << "Invalid move. Try again.\n" << endl;
    else if( Grid[r*side_len + c] != BLANK )
      cout << "That square is taken. Try again.\n" << endl;
    else
      valid_move = true;
  } while( ! valid_move );

  Grid[r*side_len + c] = Marker;

  (*CurrState)->UpdateStatus();

  Node *new_p = (*CurrState)->Find( Grid, Size );

  //If no matching state found; try searching from root of node tree.
  if( new_p == nullptr ) {

    new_p = Root->Find( Grid, Size, true );

    if( new_p != nullptr ) {
      (*CurrState)->AddAsNonChildNode( new_p );  //Add link to this node
    }
    else {
    }
  }

  //If still no matching state found; create one.
  if( new_p == nullptr ) {
    (*CurrState) = (*CurrState)->AddAsChildNode( Grid, Size );
  }
  else {
    *CurrState = new_p;
  }

  (*CurrState)->UpdateStatus();
} //GetPlayerMove