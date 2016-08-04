#include <stdlib.h>
#include <vector>
#include <array>

#define GRID_Y 20
#define GRID_X 10
#define SPAWN_X GRID_X / 2
#define SPAWN_Y 0

#define CUR_COMP_ROW(x) cur_block_geometry[x + 1]

class Field{
private:
	enum block_type { I = 1, J, L, O, S, T, Z, blocks_len = 8 };
	short prev_block;

	//spawns a new block if the current one has been 'placed'
	int new_block(short prev_block)
	{
		short new_random = rand() % blocks_len + 1;
		this->prev_block = new_random;
		if (new_random == prev_block)
			return ((new_random + 1) % blocks_len);
		return new_random;
	}

public:
	int** grid; //row major - which makes each row a Y-AXIS major value (array of x-values)
	std::vector<short> cur_block_geometry, geo_center;
	bool GAME_OVER;

	Field()	{
		grid = new int*[GRID_Y];
		for (int i = 0; i < GRID_Y; i++)
		{
			grid[i] = new int[GRID_X];
			memset(grid[i], 0, GRID_X * sizeof(int)); //need to set here - array of pointers NOT contiguous memory
		}
		prev_block = NULL;
		geo_center.resize(2);
		cur_block_geometry.resize(16);
		GAME_OVER = false;
	}

	void redefined_block_geometry()
	{
		switch (new_block(prev_block)) //gemetric center varies figure to figure (for rotation)
		{ //bear in mind the coordinates are arranged in (x, y) fashion whilst the grid is row-major
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
		for (int i = 0; i < 7; i += 2)
		{
			cur_block_geometry[i] += SPAWN_X;
			CUR_COMP_ROW(i) += SPAWN_Y;
		}
		geo_center[0] += SPAWN_X; geo_center[1] += SPAWN_Y;
	}

	void block_rotate(bool direction) //0 = clockwise, 1 = counter
	{
		//check if square
		std::vector<short> temp_rotation_vec(8);
		if (geo_center[1] < GRID_Y)
		{
			if (!direction)
			{
				//same as move - check if rotation POSSIBLE, then commit
				for (int i = 0; i < 7; i += 2)
				{
					if ((cur_block_geometry[i] == geo_center[0]) &&
						(CUR_COMP_ROW(i) == geo_center[1]))
					{
						temp_rotation_vec[i] = cur_block_geometry[i];
						temp_rotation_vec[i + 1] = CUR_COMP_ROW(i);
					} //don't rotate the coordinate if it's the geo center...
					else
					{
						int rowSelector = CUR_COMP_ROW(i) - geo_center[1]; //-1, 0, 1 (top, center, bottom)
						switch (rowSelector * 5 + (cur_block_geometry[i] - geo_center[0]))
						{
						case -6: //top left
							temp_rotation_vec[i] = cur_block_geometry[i] + 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i);
							break;
						case -5: //top center
							temp_rotation_vec[i] = cur_block_geometry[i] + 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 1;
							break;
						case -4: //top right
							temp_rotation_vec[i] = cur_block_geometry[i];
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 2;
							break;
						case -1: //center left
							temp_rotation_vec[i] = cur_block_geometry[i] + 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 1;
							break;
						case 1: //center right
							temp_rotation_vec[i] = cur_block_geometry[i] - 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 1;
							break;
						case 4: //bottom left
							temp_rotation_vec[i] = cur_block_geometry[i];
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 2;
							break;
						case 5: //bottom center
							temp_rotation_vec[i] = cur_block_geometry[i] - 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 1;
							break;
						case 6: //bottom right
							temp_rotation_vec[i] = cur_block_geometry[i] - 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i);
							break;
							//the I block has the special case of one coordinate removed 2 blocks from geo center...
						case 2: //right
							temp_rotation_vec[i] = cur_block_geometry[i] - 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 2;
							break;
						case 10: //bot
							temp_rotation_vec[i] = cur_block_geometry[i] - 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 2;
							break;
						case -2: //left
							temp_rotation_vec[i] = cur_block_geometry[i] + 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 2;
							break;
						case -10: //top
							temp_rotation_vec[i] = cur_block_geometry[i] + 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 2;
							break;
						}
					}
					if (grid[temp_rotation_vec[i + 1]][temp_rotation_vec[i]]) //GRID X/Y SWAPPED!!!!
						return;
				}
				cur_block_geometry.swap(temp_rotation_vec); //set to the rotated vector
			}
			else
			{
				for (int i = 0; i < 7; i += 2)
				{
					if ((cur_block_geometry[i] == geo_center[0]) &&
						(CUR_COMP_ROW(i) == geo_center[1]))
					{
						temp_rotation_vec[i] = cur_block_geometry[i];
						temp_rotation_vec[i + 1] = CUR_COMP_ROW(i);
					} 
					else
					{
						int rowSelector = CUR_COMP_ROW(i) - geo_center[1]; //-1, 0, 1 (top, center, bottom)
						switch (rowSelector * 5 + (cur_block_geometry[i] - geo_center[0]))
						{
						case -6: //top left
							temp_rotation_vec[i] = cur_block_geometry[i];
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 2;
							break;
						case -5: //top center
							temp_rotation_vec[i] = cur_block_geometry[i] - 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 1;
							break;
						case -4: //top right
							temp_rotation_vec[i] = cur_block_geometry[i] - 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i);
							break;
						case -1: //center left
							temp_rotation_vec[i] = cur_block_geometry[i] + 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 1;
							break;
						case 1: //center right
							temp_rotation_vec[i] = cur_block_geometry[i] - 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 1;
							break;
						case 4: //bottom left
							temp_rotation_vec[i] = cur_block_geometry[i] + 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i);
							break;
						case 5: //bottom center
							temp_rotation_vec[i] = cur_block_geometry[i] + 1;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 1;
							break;
						case 6: //bottom right
							temp_rotation_vec[i] = cur_block_geometry[i];
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 2;
							break;
							//the I block has the special case of one coordinate removed 2 blocks from geo center...
						case 2: //right
							temp_rotation_vec[i] = cur_block_geometry[i] - 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 2;
							break;
						case 10: //bot
							temp_rotation_vec[i] = cur_block_geometry[i] + 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) - 2;
							break;
						case -2: //left
							temp_rotation_vec[i] = cur_block_geometry[i] + 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 2;
							break;
						case -10: //top
							temp_rotation_vec[i] = cur_block_geometry[i] - 2;
							temp_rotation_vec[i + 1] = CUR_COMP_ROW(i) + 2;
							break;
						}
					}
					if (grid[temp_rotation_vec[i]][temp_rotation_vec[i + 1]])
						return;
				}
				cur_block_geometry.swap(temp_rotation_vec); //set to the rotated vector
			}
		}
	}

	void block_horizontal(bool direction) //0 = left, 1 = right
	{
		switch (direction)
		{
		case 0:
			for (int i = 0; i < 7; i += 2)
			if (grid[CUR_COMP_ROW(i)][cur_block_geometry[i] - 1] || cur_block_geometry[i] == 0)
				return;
			for (int i = 0; i < 7; i += 2)
				cur_block_geometry[i] -= 1;
			geo_center[0] -= 1;
			break;
		case 1:
			for (int i = 0; i < 7; i += 2)
			if (grid[CUR_COMP_ROW(i)][cur_block_geometry[i] + 1] || cur_block_geometry[i] == GRID_X - 1)
				return;
			for (int i = 0; i < 7; i += 2)
				cur_block_geometry[i] += 1;
			geo_center[0] += 1;
		}
	}

	//lowers the y coordinates of the current block's 4 components
	void block_down()
	{
		for (int i = 1; i <= 7; i += 2)
			cur_block_geometry[i] += 1;
		geo_center[1] += 1;
	}

	//if the block is placed, need to append the block to the grid...
	void regen_grid()
	{
		for (int i = 0; i < 7; i += 2)
			grid[CUR_COMP_ROW(i)][cur_block_geometry[i]] = prev_block; //set it to the color of the previous block
	}

	bool check_bassline()
	{
		for (int i = 0; i < 7; i += 2) //go through the x-coordinates, for the correlating y-slot
			if (CUR_COMP_ROW(i) == (GRID_Y - 1) || grid[CUR_COMP_ROW(i) + 1][cur_block_geometry[i]]) //if the y coordinate of the component collides next move...or if it hits 0
				return false; //new bloc need be generated, grid and bassline values adjusted
		return true; //otherwise, no collision, continue w/ block_down()
	}

	void check_lines()
	{

		for (int i = 0; i < 7; i += 2)
		{
			if (!CUR_COMP_ROW(i))
			{
				//block reached the top of the screen: GG
				GAME_OVER = true;
				return;
			}
			for (int j = 0; j < GRID_X; j++)
			{
				if (!grid[CUR_COMP_ROW(i)][j])
					break;
				if (j == GRID_X - 1)
				{
					//delete grid[CUR_COMP_ROW(i)];
					for (int k = CUR_COMP_ROW(i); k > 0; k--)
						grid[k] = grid[k - 1]; //shift all the rows down 1
					//std::copy(std::begin(grid[k - 1]), std::end(grid(k)), std::begin(grid[k]));
					memset(grid[0], 0, GRID_X * sizeof(int)); //top row reset to 0
				}
			}
		}
	}
}; 
