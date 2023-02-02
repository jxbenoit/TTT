/*Node
*/
#ifndef NODE_H
#define NODE_H

#include<string>

using namespace std;

class NodeList;

//---------------------------------------------------------------------------
class Node {
  Node  **Next1, **Next2, *Parent;
  char  *Grid;
  int   NNext1, NNext2, NWins, NTies, Size, NUntried;
  int   *Next1NodeIds;  //Used when reading in child nodes from file
  int   *Next2NodeIds;  //Used when reading in non-child nodes from file
  int   Id;
  bool  PathComplete;
  char  Status;
  bool  Compare( char TestGrid[], int Size );
  void  ConnectNextNodes( Node* Nodes[], int NNodes );
  int   GetNumNodes();

  public:
    enum StateStatus { NONE, WIN, LOSS, TIE, MIXED };
    Node( int Size, bool FillWithBlanks );
    Node( char Grid[], int Size, Node *Parent );
    ~Node();
    void  CopyGrid( char OutGrid[] );
    void  CopyGrid( char InGrid[], char OutGrid[], int Size );
    //Node* Find( char TestGrid[], int Size );
    Node* Find( char TestGrid[], int Size, bool Recurse=false );
    void  ResetSeenNodes();
    Node* AddAsChildNode( char Grid[], int Size );
    void  AddAsNonChildNode( Node *p );
    Node* ChooseMove( int Marker );
    int   GetNumWins();
    int   GetNumTies();
    bool  ReadTree( string FileName, int &NGamesPlayed );
    bool  WriteTree( string FileName, int NGamesPlayed, bool Diag=false );
    int   GetId();
    void  WriteNode( ofstream& OutFile );
    void  ReadNode( ifstream& InFile );
    int   GetSize();
    bool  IsPathComplete();
    int   GetNumUntriedPaths();
    char  GetCellContent( int index );
    void  UpdateStatus();
    void  MarkParentAsLoss();
    void  SetParent( Node *Parent );
    Node* GetParent();
    bool  CheckForLossNextMove();
    void  Set( char Grid[], int Size );
    char  GetStatus();
    void  SetStatus( char Status );
};//Node

#endif