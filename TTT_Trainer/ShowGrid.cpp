#ifndef SHOWGRID_CPP
#define SHOWGRID_CPP

#include<iostream>
#include<cmath>

using namespace std;

//---------------------------------------------------------------------------
void ShowGrid( char Grid[], int Size, bool AsSingleLine=false ) {
  if( AsSingleLine ) {
    for( int i = 0; i < Size; i++ )
      cout << Grid[i];
    
    return;    
  }

  int side_len = sqrt( Size );
  for( int i = 0; i < side_len; i++ ) {
    if( i == 0 ) {  //Write column headers
      cout << "  ";
      for( int j = 0; j < side_len; j++ ) {
        if( j > 0 ) cout << " ";
        cout << j;
      }
      cout << endl;
      cout << "  ";
      for( int j = 0; j < (side_len*2)-1; j++ )
        cout << "-";
      cout << endl;
    }
    else {         //Write row dividers
      cout << "  ";
      for( int j = 0; j < (side_len*2)-1; j++ )
        cout << "-";
      cout << endl;
    }

    cout << i << "|";
    for( int j = 0; j < side_len; j++ ) {
      if( j > 0 ) cout << "|";
      cout << Grid[i*side_len+j];
    }
    cout << endl;
  }
} //ShowGrid

#endif /* SHOWGRID_H */

