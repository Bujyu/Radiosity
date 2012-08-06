///To calculate all pairs of mesh's Form Factor

#define SIDE 4096
enum { top = 0, fornt, right, back, left };

//n_cell is 64 cells or 1024 cells per side
void hemiCubeGenrater( int n_cells ){

    // Points in cell is 64 points or 4 points per cell
    double cellSize = SIDE / n_cells;
    double halfSize = n_cells / 2;
    int i, j, k;

    //Top
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = -halfSize ; j < halfSize ; j++ ){

            addPoint(         i * cellSize, halfSize * cellSize, j * cellSize );
            addPoint(         i * cellSize, halfSize * cellSize, ( j + 1 ) * cellSize );
            addPoint( ( i + 1 ) * cellSize, halfSize * cellSize, ( j + 1 ) * cellSize);
            addPoint( ( i + 1 ) * cellSize, halfSize * cellSize, j * cellSize );
            addPatch();

        }
    }

    //Front and Back
    for( i = -halfSize ; i < halfSize ; i++ ){
        for( j = 0; j < halfSize ; j++ ){

            addPoint(         i * cellSize, j * cellSize        , halfSize * cellSize );
            addPoint(         i * cellSize, ( j + 1 ) * cellSize, halfSize * cellSize );
            addPoint( ( i + 1 ) * cellSize, ( j + 1 ) * cellSize, halfSize * cellSize );
            addPoint( ( i + 1 ) * cellSize, j * cellSize        , halfSize * cellSize );
            addPatch();

            addPoint(         i * cellSize, j * cellSize        , -halfSize * cellSize );
            addPoint(         i * cellSize, ( j + 1 ) * cellSize, -halfSize * cellSize );
            addPoint( ( i + 1 ) * cellSize, ( j + 1 ) * cellSize, -halfSize * cellSize );
            addPoint( ( i + 1 ) * cellSize, j * cellSize        , -halfSize * cellSize );
            addPatch();

        }
    }

    // Right and Left
    for( i = 0; i < halfSize ; i++ ){
        for( j = -halfSize; j < halfSize ; j++ ){

            addPoint( halfSize * cellSize,         i * cellSize, j * cellSize         );
            addPoint( halfSize * cellSize,         i * cellSize, ( j + 1 ) * cellSize );
            addPoint( halfSize * cellSize, ( i + 1 ) * cellSize, ( j + 1 ) * cellSize );
            addPoint( halfSize * cellSize, ( i + 1 ) * cellSize, j * cellSize         );
            addPatch();

            addPoint( -halfSize * cellSize,         i * cellSize, j * cellSize         );
            addPoint( -halfSize * cellSize,         i * cellSize, ( j + 1 ) * cellSize );
            addPoint( -halfSize * cellSize, ( i + 1 ) * cellSize, ( j + 1 ) * cellSize );
            addPoint( -halfSize * cellSize, ( i + 1 ) * cellSize, j * cellSize         );
            addPatch();

        }
    }

}
