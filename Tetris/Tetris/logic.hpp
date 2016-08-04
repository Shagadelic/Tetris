#include <stdlib.h>
#include <time.h>

#include <vector>

#define GRID_Y 20
#define GRID_X 10

#define SPAWN_X (GRID_X / 2)
#define SPAWN_Y 0

#define CUR_COMP_Y(x) cur_block_geometry[x + 1]

bool grid[GRID_X][GRID_Y];
static enum block_type {I, J, L, O, S, T, Z, blocks_len = 7}; 
static int prev_block;

//the bassline - the current "bottom" of the playing field - none of the coordinates of the current block can go 
//beyond the bassline - needless to say, all 10 are Y coordinates, blocks fall from top to bottom

//move clockwise, leftmost block provided as (0, 0) -> all blocks specified RELATIVE to leftmost block
static std::vector<short> cur_block_geometry(8), geo_center(2);

//spawns a new block if the current one has been 'placed'
static int new_block(short prev_block)
{
	short new_random = rand() % blocks_len;
	::prev_block = new_random;
	if (new_random == prev_block)
		return ((new_random + 1) % blocks_len);
	return new_random;
}

//redefines the block coordinates of the block in play, defaults rotation to 0
//rotation axis is defined AS the first point 0
static void redefined_block_geometry(int new_block)
{
	switch (new_block) //gemetric center varies figure to figure (for rotation)
	{
	case I: 
		cur_block_geometry = { 0, 0, 1, 0, 2, 0, 3, 0 };
		geo_center = { 1, 0 };
		break;
	case J:
		cur_block_geometry = { 0, 0, 1, 0, 2, 0, 2, 1 };
		geo_center = { 1, 1 };
		break;
	case L:
		cur_block_geometry = { 0, 0, 0, 1, 1, 0, 2, 0 };
		geo_center = { 1, 1 };
		break;
	case O:
		cur_block_geometry = { 0, 0, 0, 1, 1, 0, 1, 1 };
		geo_center = { 0, GRID_Y * 2 }; //NO ROTATION FOR SQUARE, make center Y insanely large
		break;
	case S:
		cur_block_geometry = { 0, 1, 1, 1, 1, 0, 2, 0 };
		geo_center = { 1, 1 };
		break;
	case T:
		cur_block_geometry = { 1, 1, 0, 0, 1, 0, 2, 0 };
		geo_center = { 1, 0 };
		break;
	case Z:
		cur_block_geometry = { 1, 1, 0, 0, 1, 0, 2, 1 };
		geo_center = { 1, 0 };
		break; 
	default:
		break;
	}

	//now, the block need be normalized. by default, spawn blocks @ middle
	for (int i = 0; i <= 7; i += 2)
	{
		cur_block_geometry[i] += SPAWN_X;
		CUR_COMP_Y(i) += SPAWN_Y;
	}
	geo_center[0] += SPAWN_X; geo_center[1] += SPAWN_Y;
}

static void rotate_block(bool direction) //0 = clockwise, 1 = counter
{
	//check if square
	std::vector<short> temp_rotation_vec(8);
	if (!(geo_center[1] > GRID_Y))
	{
		if (!direction)
		{
			//same as move - check if rotation POSSIBLE, then commit
			for (int i = 0; i < 7; i += 2)
			{
				if ((cur_block_geometry[i] == geo_center[0]) &&
					(CUR_COMP_Y(i) == geo_center[1]))
				{
					temp_rotation_vec[i] = cur_block_geometry[i];
					temp_rotation_vec[i + 1] = CUR_COMP_Y(i);
				} //don't rotate the coordinate if it's the geo center...
				else
				{
					int rowSelector = CUR_COMP_Y(i) - geo_center[1]; //-1, 0, 1 (top, center, bottom)
					switch (rowSelector * 5 + (cur_block_geometry[i] - geo_center[0]))
					{
					case -6: //top left
						temp_rotation_vec[i] = cur_block_geometry[i] + 2;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i);
						break;
					case -5: //top center
						temp_rotation_vec[i] = cur_block_geometry[i] + 1;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) + 1;
						break;
					case -4: //top right
						temp_rotation_vec[i] = cur_block_geometry[i];
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) + 2;
						break;
					case -1: //center left
						temp_rotation_vec[i] = cur_block_geometry[i] + 1;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) - 1;
						break;
					case 1: //center right
						temp_rotation_vec[i] = cur_block_geometry[i] - 1;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) + 1;
						break;
					case 4: //bottom left
						temp_rotation_vec[i] = cur_block_geometry[i];
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) - 2;
						break;
					case 5: //bottom center
						temp_rotation_vec[i] = cur_block_geometry[i] - 1;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) - 1;
						break;
					case 6: //bottom right
						temp_rotation_vec[i] = cur_block_geometry[i] - 2;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i);
						break;
					//the I block has the special case of one coordinate removed 2 blocks from geo center...
					case 2: //right
						temp_rotation_vec[i] = cur_block_geometry[i] - 2;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) + 2;
						break;
					case 10: //bot
						temp_rotation_vec[i] = cur_block_geometry[i] - 2;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) - 2;
						break;
					case -2: //left
						temp_rotation_vec[i] = cur_block_geometry[i] + 2;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) - 2;
						break;
					case -10: //top
						temp_rotation_vec[i] = cur_block_geometry[i] + 2;
						temp_rotation_vec[i + 1] = CUR_COMP_Y(i) + 2;
						break;
					}
				}
				if (grid[temp_rotation_vec[i]][temp_rotation_vec[i + 1]])
					return;
			}
			cur_block_geometry.swap(temp_rotation_vec); //set to the rotated vector
		}
		else
		{
			//for (int i = 0; i < 7; i++)
			//{
			//	short xdiff = geo_center[0] - cur_block_geometry[i],
			//		ydiff = geo_center[1] - CUR_COMP_Y(i);
			//	//y and x switch, y gets negated///////
			//	temp_rotation_vec[i] = ydiff;
			//	temp_rotation_vec[i + 1] = -xdiff;
			//	///////////////////////////////
			//	if (grid[temp_rotation_vec[i]][temp_rotation_vec[i + 1]])
			//		return;
			//}
			//cur_block_geometry.swap(temp_rotation_vec); //set to the rotated vector
		}
	}
}

static void block_horizontal(bool direction) //0 = left, 1 = right
{
	switch (direction)
	{
	case 0:
		for (int i = 0; i < 7; i += 2)
			if ((cur_block_geometry[i] == 0) || (grid[i - 1][CUR_COMP_Y(i)]))
				return; //first, check if OK to move block
		for (int i = 0; i < 7; i += 2)
			cur_block_geometry[i] -= 1; //commit move
		geo_center[0] -= 1;
		break;
	case 1:
		for (int i = 0; i < 7; i += 2)
			if ((cur_block_geometry[i] == GRID_X - 1) || (grid[i + 1][CUR_COMP_Y(i)]))
				return;
		for (int i = 0; i < 7; i += 2)
			cur_block_geometry[i] += 1; 
		geo_center[0] += 1;
	}
}

//checks if any of the block's y coordinates are about to hit the bassline. If so, new block
static bool check_bassline()
{
	for (int i = 0; i < 7; i += 2) //go through the x-coordinates, for the correlating y-slot
		if (grid[cur_block_geometry[i]][CUR_COMP_Y(i) + 1] || (GRID_Y - 1) == CUR_COMP_Y(i)) //if the y coordinate of the component collides next move...or if it hits 0
			return false; //new bloc need be generated, grid and bassline values adjusted
	return true; //otherwise, no collision, continue w/ block_down()
}

//lowers the y coordinates of the current block's 4 components
static void block_down()
{
	for (int i = 1; i <= 7; i += 2)
		cur_block_geometry[i] += 1;
	geo_center[1] += 1;
}

//if the block is placed, need to append the block to the grid...
static void regen_grid()
{
	for (int i = 0; i < 7; i += 2)
		grid[cur_block_geometry[i]][CUR_COMP_Y(i)] = true;
}

//after a block is placed, need to check if it filled any lines. If it did, erase those lines.
static void check_lines()
{
	//this would be incredibly easy if the grid could be easily transposed...
	/*bool** transposedGrid = new bool*[GRID_Y];
	for (int i = 0; i < GRID_Y; i++)
	{
		for (int j = 0)........
	}*/

	bool tempPush[GRID_X][GRID_Y];
	for (int i = 0; i < 7; i += 2)
	{
		short row = CUR_COMP_Y(i);
		for (int j = 0; j < GRID_X; j++)
		{
			if (grid[j][row])	break;
			tempPush[j][0] = 0;
			for (int k = 1; k < row; k++)
				tempPush[j][k + 1] = grid[j][k];
			for (int k = row; k < GRID_Y; k++)
				tempPush[j][k] = grid[j][k];

			if (j == GRID_X - 1)	//if all the cells within the row are filled...
			for (int l = 0; l < GRID_X; l++)
				*grid[l] = true;// tempPush[l];
		}
	}
}
