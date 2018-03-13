#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <queue>
#include <time.h>
//#include <assert.h>
#include <vector>
#include <string>
#include "Query_interface.h"
#define DebugMode false
using namespace std;
struct tree_node
{
	int value; // this should be size_t
	
	int X1;
	int Y1;
	int Z1;

	int X2;
	int Y2;
	int Z2;

	// int high;
	tree_node* first = NULL;
	tree_node* second = NULL;
	tree_node* third = NULL;
	tree_node* forth = NULL;
	tree_node* fifth = NULL;
	tree_node* sixth = NULL;
	tree_node* seventh = NULL;
	tree_node* eighth = NULL;

};
class DataGenerator
{
	private:
		vector<vector< vector<int> >> matrix;
		int DimX;
		int DimY;
		int DimZ;
		int lowerBound;
		int higherBound;
		int count;// number of elements in the matrix DimX*DimY*DimZ
		//================ aggregate generation
		int Devision_factor = 2; // each dimension devided to 2 as we go deep in the tree
		vector<int> aggregates;
		int step = 0; //for debugging
		tree_node* root;
	public:
		/*
			This class is to generate a 3d dimX by dimY by dimZ matrix where the values
			are uniformly random integers ranging between lowBound and highBound.
		*/
		DataGenerator(int dimX, int dimY,int dimZ, int lowBound, int highBound)
		{
			// TODO: Check lowVal<highVAl
			DimX = dimX;
			DimY = dimY;
			DimZ = dimZ;
			lowerBound = lowBound;
			higherBound = highBound;
			count = dimX*dimY*dimZ;
			//=======================
		}
		/*
			generates random matrix
		*/
		void generate()
		{
			srand(time(NULL));
			for(int i=0;i<DimX;i++)
			{
				vector<vector<int>> plain ;
				for(int j=0 ;j<DimY;j++)
				{
					vector<int> row ;
					for( int k=0;k<DimZ;k++)
					{
						int range = higherBound - lowerBound;
						int rnd = rand()%(range+1)+lowerBound;
						row.push_back(rnd);
					}			
					plain.push_back(row);
				}
				matrix.push_back(plain);				
			}
		}

		/*
		prints the matrix
		*/
		// void print()
		// {
		// 	int MaxX = matrix.size();
		// 	for(int i=0;i<MaxX;i++)
		// 	{
		// 		int MaxY = matrix[i].size();
		// 		for(int j=0 ;j<MaxY;j++)
		// 		{
		// 			cout<< matrix[i][j]<<"	";
		// 		}
		// 		cout<<endl;
		// 	}
		// }
		/*
		saves the matrix in a output file with the address dir
		*/
		void saveAs(string dir)
		{
			ofstream myfile;
			myfile.open (dir);
			for(int i=0;i<DimX;i++)
				for(int j=0 ;j<DimY;j++)
					for(int k=0 ;k<DimZ;k++)
						myfile<< matrix[i][j][k]<<" ";
			myfile.close();
		}
		/*
		loads the matrix from a input file residing in the address dir
		*/
		void load(string dir)
		{
			ifstream myfile;
			myfile.open(dir);
			if(myfile.is_open())
			{
				cout<<"reading the file ..."<<endl;
				for(int i=0;i<DimX;i++)
				{
					vector<vector<int>> plain ;
					for(int j=0 ;j<DimY;j++)
					{
						vector<int> row ;
						for( int k=0;k<DimZ;k++)
						{
							row.push_back(0);
						}			
						plain.push_back(row);
					}
					matrix.push_back(plain);				
				}
				for(int i=0;i<DimX;i++)
					for(int j=0 ;j<DimY;j++)
						for(int k=0 ;k<DimZ;k++)
							myfile>> matrix[i][j][k];

				cout<<"The file read successfully"<<endl;
			}
			else
				cout<<"couldn't open the file"<<endl;
			myfile.close();

		}
		/*
		returns the actual data
		*/
		vector<vector <vector<int>>> get_matrix()
		{
			return matrix;
		}
		int* get_array()
		{
			int* array = new int[DimX*DimY*DimZ];
			for(int i=0;i<DimX;i++)
			{
				for(int j=0; j< DimY;j++)
				{
					for(int k=0;k<DimZ;k++)
					{
						int key = i*DimY*DimZ+j*DimZ+k;
						array[key]= matrix[i][j][k];
						//cout<<"array["<<key<<"]:"<<array[key]<<endl;
					}
				}
			}
			return array;
		}
		int get_count()
		{return count;}
		int get_DimY()
		{return DimY;}
		int get_DimX()
		{return DimX;}
		int get_DimZ()
		{return DimZ;}
		// /*generates the top levels of the aggregate tree*/
		void generate_sum_tree_upto_level(int level)
		{
			int granularity = 1<<level;
			int Xstride = DimX / granularity;
			int Ystride = DimY / granularity;
			int Zstride = DimZ / granularity;
			// cout<<Xstride<<" "<<Ystride<<" "<<Zstride<<endl;
			if(Xstride<=0 || Ystride <=0 || Zstride <= 0)
			{
				cout<<"Error: Please choose a lower tree level\n";
				assert(false);
			}
			vector<vector< vector<int>> > aggregate_matrix;
			for(int i = 0; i<granularity; i++) // This will skip the boarders !!
			{
				vector<vector<int>> plain;
				for(int j = 0;j<granularity;j++)// This will skip the boarders !!
				{
					vector<int> row;
					for(int k = 0;k<granularity;k++)// This will skip the boarders !!
					{
						//cout<<i<<" "<<j<<endl;
						row.push_back (query_base(i*Xstride,j*Ystride,k*Zstride,(i+1)*Xstride-1,(j+1)*Ystride-1,(k+1)*Zstride-1));
					}
					plain.push_back(row);
				}
				aggregate_matrix.push_back(plain);
			}
			root = sum_helper(0,0,0,granularity-1,granularity-1,granularity-1,aggregate_matrix);
		}

		// /*generates a full sum aggregate*/
		void generate_sum_tree()
		{
			root = sum_helper(0,0,0,DimX-1,DimY-1,DimZ-1,matrix);
		}
		tree_node* sum_helper(int x1, int y1,int z1, int x2, int y2,int z2,vector<vector< vector<int> >> matrix )
		{
			/*step++;
			assert(step != 50);*/
			//cout<<x1<<","<<y1<<"	"<<x2<<","<<y2<<endl;
			int xMid = (x1+x2)/2;
			int yMid = (y1+y2)/2;
			int zMid = (z1+z2)/2;
			if(x1 == x2)
			{
				if (y1==y2)
				{
					if(z1==z2)
					{
						/* . */
						//cout<<"case1 "<<endl;

						tree_node* node = new tree_node;
						node->value = matrix[x1][y1][z1];
						
						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x1;
						node->Y2 = y1;
						node->Z2 = z1;
						return node;
					}
					else//(z1!=z2)
					{
						/* / */
						//cout<<"case2"<<endl;

						tree_node* first = sum_helper(x1,y1,z1,x1,y1,zMid,matrix);
						tree_node* second= sum_helper(x1,y1,zMid+1,x1,y1,z2,matrix);
						tree_node* node = new tree_node;
						node->value = (first->value+second->value);///2;
						node->first = first;
						node->second = second;
						//cout<< result<<endl;
						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x1;
						node->Y2 = y1;
						node->Z2 = z2;
						return node;
					}
				}
				else//(y1!=y2)
				{
					if(z1 == z2)
					{
						/* -- */
						//cout<<"case3"<<endl;

						tree_node* first = sum_helper(x1,y1,z1,x1,yMid,z1,matrix);
						tree_node* third= sum_helper(x1,yMid+1,z1,x1,y2,z1,matrix);
						tree_node* node = new tree_node;
						node->value = (first->value+third->value);///2;
						node->first = first;
						node->third = third;
						//cout<< result<<endl;
						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x1;
						node->Y2 = y2;
						node->Z2 = z1;
						return node;
					}
					else//(z1!=z2)
					{
						/* /+/ */
						//cout<<"case4"<<endl;

						tree_node* first  = sum_helper(x1,y1  ,z1    ,x1,yMid,zMid,matrix);
						tree_node* second = sum_helper(x1,y1  ,zMid+1,x1,yMid,z2,matrix);
						tree_node* third  = sum_helper(x1,yMid,z1    ,x1,y2,zMid,matrix);
						tree_node* forth  = sum_helper(x1,yMid,zMid+1,x1,y2,z2,matrix);

						tree_node* node = new tree_node;
						node->value = (first->value+second->value+third->value+forth->value);///4;

						node->first = first;
						node->second = second;
						node->third = third;
						node->forth = forth;

						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x1;
						node->Y2 = y2;
						node->Z2 = z2;
						//cout<< result<<endl;
						return node;
					}

				}
			}
			else//(x1!=x2)
			{
				if (y1==y2)
				{
					if(z1==z2)
					{
						/* | */
						//cout<<"case5"<<endl;

						tree_node* first = sum_helper(x1    ,y1,z1,xMid,y1,z1,matrix);
						tree_node* fifth = sum_helper(xMid+1,y1,z1,x2  ,y1,z1,matrix);
						tree_node* node = new tree_node;
						node->value = (first->value+fifth->value);///2;
						node->first = first;
						node->fifth = fifth;
						//cout<< result<<endl;
						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x2;
						node->Y2 = y1;
						node->Z2 = z1;
						return node;
					}
					else//(z1!=z2)
					{
						/* /+/ */
						//cout<<"case6"<<endl;

						tree_node* first  = sum_helper(x1    ,y1,z1    ,xMid,y1,zMid,matrix);
						tree_node* second = sum_helper(x1    ,y1,zMid+1,xMid,y1,z2,matrix);
						tree_node* fifth  = sum_helper(xMid+1,y1,z1    ,x2  ,y1,zMid,matrix);
						tree_node* sixth  = sum_helper(xMid+1,y1,zMid+1,x2  ,y1,z2,matrix);

						tree_node* node = new tree_node;
						node->value = (first->value+second->value+fifth->value+sixth->value);///4;

						node->first = first;
						node->second = second;
						node->fifth = fifth;
						node->sixth = sixth;
						
						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x2;
						node->Y2 = y1;
						node->Z2 = z2;						
						//cout<< result<<endl;
						return node;
					}
				}
				else//(y1!=y2)
				{
					if(z1 == z2)
					{
						/* |+| */
						//cout<<"case7"<<endl;

						tree_node* first   = sum_helper(x1    ,y1,    z1,xMid,yMid,z1,matrix);
						tree_node* third   = sum_helper(x1    ,yMid+1,z1,xMid,y2  ,z1,matrix);
						tree_node* fifth   = sum_helper(xMid+1,y1    ,z1,x2  ,yMid,z1,matrix);
						tree_node* seventh = sum_helper(xMid+1,yMid+1,z1,x2  ,y2  ,z1,matrix);

						tree_node* node = new tree_node;
						node->value = (first->value+third->value+fifth->value+seventh->value);///4;

						node->first = first;
						node->third= third;
						node->fifth = fifth;
						node->seventh = seventh;
						
						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x2;
						node->Y2 = y2;
						node->Z2 = z1;						
						//cout<< result<<endl;
						return node;
					}
					else//(z1!=z2)
					{
						/* ++ */
						//cout<<"case8"<<endl;

						tree_node* first   = sum_helper(x1    ,y1    ,z1    ,xMid,yMid,zMid,matrix);
						tree_node* second  = sum_helper(x1    ,y1    ,zMid+1,xMid,yMid,z2  ,matrix);
						tree_node* third   = sum_helper(x1    ,yMid+1,z1    ,xMid,y2  ,zMid,matrix);
						tree_node* forth   = sum_helper(x1    ,yMid+1,zMid+1,xMid,y2  ,z2  ,matrix);
						tree_node* fifth   = sum_helper(xMid+1,y1    ,z1    ,x2  ,yMid,zMid,matrix);
						tree_node* sixth   = sum_helper(xMid+1,y1    ,zMid+1,x2  ,yMid,z2  ,matrix);
						tree_node* seventh = sum_helper(xMid+1,yMid+1,z1    ,x2  ,y2  ,zMid,matrix);
						tree_node* eighth  = sum_helper(xMid+1,yMid+1,zMid+1,x2  ,y2  ,z2  ,matrix);
						

						tree_node* node = new tree_node;
						node->value   = (first->value+second->value+third->value+forth->value
									+fifth->value+sixth->value+seventh->value+eighth->value);///8;

						node->first   = first;
						node->second  = second;
						node->third   = third;
						node->forth   = forth;
						node->fifth   = fifth;
						node->sixth   = sixth;
						node->seventh = seventh;
						node->eighth  = eighth;

						node->X1 = x1;
						node->Y1 = y1;
						node->Z1 = z1;

						node->X2 = x2;
						node->Y2 = y2;
						node->Z2 = z2;
						//cout<< result<<endl;
						return node;
					}

				}
			}

		}
		void BFS()
		{
			queue<tree_node*> Q;
			Q.push(root);
			while(!Q.empty())
			{
				tree_node* head = Q.front();
				Q.pop();
				cout<<head->value<<"	("<<head->X1<<","<<head->Y1<<","<<head->Z1<<") ("<<head->X2<<","<<head->Y2<<","<<head->Z2<<")"<<endl;
				if((head->first)!= NULL)
					Q.push(head->first);
				if(head->second!= NULL)
					Q.push(head->second);
				if(head->third!= NULL)
					Q.push(head->third);
				if(head->forth!= NULL)
					Q.push(head->forth);
				if((head->fifth)!= NULL)
					Q.push(head->fifth);
				if(head->sixth!= NULL)
					Q.push(head->sixth);
				if(head->seventh!= NULL)
					Q.push(head->seventh);
				if(head->eighth!= NULL)
					Q.push(head->eighth);
			}
			cout<<endl;
		}
		vector<int>* BFS_max_depth(int maxDepth,boost::dynamic_bitset<>& Bit_Representor)
		{
			boost::dynamic_bitset<> bit_representor;
			//int last_full_level = log(3*count)/log(4)-1;
			if(maxDepth<0)
				return NULL;
			vector<int>* result = new vector<int>; // this should be size_t
			queue<tree_node*> Q;
			Q.push(root);
			int currentDepth = 0,
				elementsToDepthIncrease = 1,
				nextElementsToDepthIncrease = 0;
			while(!Q.empty())
			{
				tree_node* head = Q.front();
				Q.pop();
				result->push_back(head->value);
				// cout<<head->value<<"	("<<head->X1<<","<<head->Y1<<") ("<<head->X2<<","<<head->Y2<<")"<<endl;
				int numberOfChildren = 0;
				if((head->first)!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);
				}
				if(head->second!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);
				}
				if(head->third!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);

				}
				if(head->forth!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);
				}
				if((head->fifth)!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);
				}
				if(head->sixth!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);
				}
				if(head->seventh!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);

				}
				if(head->eighth!= NULL)
				{
					numberOfChildren++;
					//if(last_full_level == currentDepth)
					bit_representor.push_back(1);
				}
				else{
					//if(last_full_level == currentDepth)
					bit_representor.push_back(0);

				}
				nextElementsToDepthIncrease += numberOfChildren;
				if (--elementsToDepthIncrease == 0) {
				  if (++currentDepth > maxDepth) return result;
				  elementsToDepthIncrease = nextElementsToDepthIncrease;
				  nextElementsToDepthIncrease = 0;
				}
				if((head->first)!= NULL)
					Q.push(head->first);
				if(head->second!= NULL)
					Q.push(head->second);
				if(head->third!= NULL)
					Q.push(head->third);
				if(head->forth!= NULL)
					Q.push(head->forth);
				if((head->fifth)!= NULL)
					Q.push(head->fifth);
				if(head->sixth!= NULL)
					Q.push(head->sixth);
				if(head->seventh!= NULL)
					Q.push(head->seventh);
				if(head->eighth!= NULL)
					Q.push(head->eighth);
			}
			Bit_Representor = bit_representor;
			//cout<<"bit representor: "<<bit_representor<<" size:"<<bit_representor.size()<<endl;
			//cout<<result->at(0)<<"	"<<result->at(result->size()-1)<<endl;
			return result;
		}
		// int query(int x1, int y1, int x2, int y2)
		// {

		// 	step = 0;
		// 	int result = query_helper(x1, y1, x2, y2,root);
		// 	cout<<"# of getvals:"<<step<<"\n";
		// 	return result;

		// }
		// int query_helper(int x1, int y1, int x2, int y2, tree_node* node)
		// {
		// 	if(DebugMode)
		// 		cout<<"Query("<<x1<<","<<y1<<" "<<x2<<","<<y2<<") over ("<<node->X1<<","<<node->Y1<<" "<<node->X2<<","<<node->Y2<<")"<<endl;
		// 	//cout<<"=============================="<<endl;
		// 	//step++;
		// 	//assert(step!=10);
		// 	if(x1==node->X1 && y1 == node->Y1 && x2==node->X2 && y2 == node->Y2)
		// 	{
		// 		//cout<<"exact value:"<<node->value<<endl;
		// 		step++;
		// 		return node->value;
		// 	}
		// 	float r1 = 0;
		// 	float r2 = 0;
		// 	float r3 = 0;
		// 	float r4 = 0;

		// 	pair<pair<int,int>, pair<int,int>>* a1 = NULL;
		// 	pair<pair<int,int>, pair<int,int>>* a2 = NULL;
		// 	pair<pair<int,int>, pair<int,int>>* a3 = NULL;
		// 	pair<pair<int,int>, pair<int,int>>* a4 = NULL;
		// 	if(node->first!=NULL)
		// 		a1 = overlap(x1, y1, x2, y2, node->first);
		// 	//cout<<"here0"<<endl;
		// 	if(node->second!=NULL)
		// 		a2 = overlap(x1, y1, x2, y2, node->second);
		// 	//cout<<"here1"<<endl;
		// 	if(node->third!=NULL)
		// 		a3 = overlap(x1, y1, x2, y2, node->third);
		// 	//cout<<"here2"<<endl;
		// 	if(node->forth!=NULL)
		// 		a4 = overlap(x1, y1, x2, y2, node->forth);
		// 	//cout<<a1<<" "<<a2<<" "<<a3<<" "<<a4<<endl;
		// 	if(a1!=NULL)
		// 	{
		// 		//cout<<"here0"<<endl;
		// 		//cout<<(a1->first).first<<","<<(a1->first).second<<" "<<(a1->second).first<<","<<(a1->second).second<<endl;
		// 		r1 = query_helper((a1->first).first,(a1->first).second,(a1->second).first,(a1->second).second, node->first);
		// 	}
		// 	/*else
		// 		cout<<"a1 is null"<<endl;*/
		// 	if(a2!=NULL)
		// 	{
		// 		//cout<<"here1"<<endl;
		// 		//cout<<(a2->first).first<<","<<(a2->first).second<<" "<<(a2->second).first<<","<<(a2->second).second<<endl;
		// 		r2 = query_helper((a2->first).first,(a2->first).second,(a2->second).first,(a2->second).second, node->second);
		// 	}/*
		// 	else
		// 		cout<<"a2 is null"<<endl;*/
		// 	if(a3!=NULL)
		// 	{
		// 		//cout<<"here2"<<endl;
		// 		//cout<<(a3->first).first<<","<<(a3->first).second<<" "<<(a3->second).first<<","<<(a3->second).second<<endl;
		// 		r3 = query_helper((a3->first).first,(a3->first).second,(a3->second).first,(a3->second).second, node->third);
		// 	}
		// 	/*else
		// 		cout<<"a3 is null"<<endl;*/
		// 	if(a4!=NULL)
		// 	{
		// 		//cout<<"here3"<<endl;
		// 		//cout<<(a4->first).first<<","<<(a4->first).second<<" "<<(a4->second).first<<","<<(a4->second).second<<endl<<endl;
		// 		r4 = query_helper((a4->first).first,(a4->first).second,(a4->second).first,(a4->second).second, node->forth);
		// 	}
		// 	/*else
		// 		cout<<"a4 is null"<<endl;*/
		// 	//cout<<"------------------------------------"<<endl;
		// 	//cout<<"r1:"<<r1<<" r2:"<<r2<<" r3:"<<r3<<" r4:"<<r4<<" res:"<<(r1+r2+r3+r4)/4<<endl;
		// 	return (r1+r2+r3+r4);
		// }
		// int max(int a, int b)
		// {
		// 	return (a>=b)?a:b;
		// }
		// int min(int a, int b)
		// {
		// 	return (a<=b)?a:b;
		// }
		// pair<pair<int,int>, pair<int,int>>* overlap(int x1, int y1, int x2, int y2, tree_node* node)
		// {
		// 	//cout<<"overlap("<<x1<<","<<y1<<" "<<x2<<","<<y2<<")	("<<node->X1<<","<<node->Y1<<" "<<node->X2<<","<<node->Y2<<")"<<endl;
		// 	pair<pair<int,int>, pair<int,int>>* result = NULL;
		// 	pair<int,int> top_left;
		// 	pair<int,int> bottom_right;
		// 	top_left.first = max(x1, node->X1);
		// 	top_left.second = max(y1, node->Y1);
		// 	bottom_right.first = min(x2, node->X2);
		// 	bottom_right.second = min(y2, node->Y2);
		// 	if(bottom_right.first<top_left.first || bottom_right.second<top_left.second ) // no overlap between query region and node area
		// 	{
		// 		return NULL;
		// 	}
		// 	result = new pair<pair<int,int>, pair<int,int>>;
		// 	result->first.first = top_left.first;
		// 	result->first.second = top_left.second;
		// 	result->second.first = bottom_right.first;
		// 	result->second.second = bottom_right.second;
		// 	return result;

		// }
		int query_base(int x1, int y1,int z1, int x2, int y2,int z2)
		{
			int sum = 0;
			for(int i = x1;i<=x2;i++)
				for(int j = y1;j<=y2;j++)
					for(int k = z1;k<=z2;k++)
						sum+=matrix[i][j][k];
			return sum;

		}

};
int main (int argc, char** argv)
{
////########################## 0. User interations ##############################
cout<<"#################################################"<<endl;		
	cout<<"Usage:"<<endl;
	cout<<"./proj [--generate 100]<--dimSize 512><--directory ./>[--TreeLevel 1][--Query 0 0 0 128 128 128] \n";
	cout<<"--generate: generates a random matrix; default is false; generate needs cardinality next after it; default cardinality is 100"<<endl;
	cout<<"--dimSize:  number of elements in each dimension of the matrix; default is 512"<<endl;
	cout<<"--directory: address of the input file"<<endl;
	cout<<"--TreeLevel: number of levels in the aggregate tree; default is 1"<<endl;
	cout<<"--Query: coordinations of the two points to be queried; default is (x1 = 0,y1 = 0,z1 = 0)(x2 = 128,y2 = 128,z2 = 128)"<<endl;
	bool generate_mode = false;// false mean load; true means generate
	int cardinality = 100;
	int dimSize = 512;
	string directory = "./";
	int Tree_level = 0;
	int x1 = 0;
	int y1 = 0;
	int z1 = 0;
	int x2 = 128;
	int y2 = 128;
	int z2 =128;
	pair<point,point> query_region;
	for (int i = 1; i < argc; i++) 
	{
		string arg = argv[i];
		// cout<<i<<":"<< arg<<endl;
		if(arg == "--generate")
		{
			generate_mode = true;
			i++;
			cardinality = stoi(argv[i]);
		}
		if(arg == "--dimSize")
		{
			i++;
			dimSize = stoi(argv[i]);
		}
		if(arg == "--directory")
		{
			i++;
			directory = argv[i];
		}
		if(arg == "--TreeLevel")
		{
			i++;
			Tree_level = stoi(argv[i]);
		}
		if(arg == "--Query")
		{
			i++; query_region.first.x = stoi(argv[i]);
			i++; query_region.first.y = stoi(argv[i]);
			i++; query_region.first.z = stoi(argv[i]);
			i++; query_region.second.x = stoi(argv[i]);
			i++; query_region.second.y = stoi(argv[i]);
			i++; query_region.second.z = stoi(argv[i]);
		}
	}
	cout<<"\n#################################################"<<endl;	
	cout<< "Parameters set to:\n";
	cout<< "--generate:"<<generate_mode<<" cardinality: "<<cardinality<<endl;
	cout<<"--dimSize:"<<dimSize<<endl;
	cout<< "--directory:"<<directory<<endl;
	cout<<"--TreeLevel:"<<Tree_level<<endl;
	cout<<"--Query: ("<<query_region.first.x<<","<<query_region.first.y<<","<<query_region.first.z<<") ("
					  <<query_region.second.x<<","<<query_region.second.y<<","<<query_region.second.z<<")"<<endl;
////########################## 1. Data generation ###############################
cout<<"\n############## Data Generation/load #############"<<endl;
	DataGenerator dg (dimSize, dimSize,dimSize, 0,cardinality);
	if(generate_mode){
		dg.generate();
		dg.saveAs(directory);
	}
	else{
		dg.load(directory);
	}
////######################## 2. Generate aggregate tree #########################
clock_t t1,t2=0,t3=0,t4=0,sumtime=0,sumtime1=0,sumtime2=0;
cout<<"############### Tree Generation #################"<<endl;
	t1 = clock();
	dg.generate_sum_tree_upto_level(Tree_level);
	t1 = clock()-t1;
	cout<<"tree generation time:"<<((float)t1)/CLOCKS_PER_SEC<<endl;
	// dg.BFS();

int R1 = dg.query_base(x1,y1,z1,x2,y2,z2);
cout<<"\n#################################################"<<endl;	
cout<<"exact query result:"<<R1<<endl;
int rep = 1;
/*
////########################## 3. Exact queries #################################
cout<<"################ Exact Query #####################"<<endl;
cout<<"\n=================\nbaseline:\n";
for(int i = 0 ; i<11;i++)// increase the query size
//for(int i = 0 ; i<4;i++)// increase the query size
{
	//base line
	for(int j = 0;j<rep;j++)// repeat the query 10 times
	{
		//Query the tree
		int R1 = 0;
		t2=0;
		t2 = clock();
		//R1 = dg.query_base(0,0,499*i,499*i);
		R1 = dg.query_base(0,0,49*i,49*i);
		/////baseline query
		//R1 = dg.query_base(0,0,i,i);

		//cout<<R1<<"\t";
		t2 = clock()-t2;
		sumtime1+=t2;
	}
	cout<<sumtime1/rep<<endl;
}
cout<<"\n==================\ntree:\n";
for(int i = 0 ; i<11;i++)// increase the query size
//for(int i = 0 ; i<4;i++)// increase the query size
{
	//tree based aggregation
	//cout<<"(0,0,"<<499*i<<","<<499*i<<") treeBased: ";
	////cout<<"(0,0,"<<50*i<<","<<50*i<<") treeBased: ";
	//cout<<"(0,0,"<<i<<","<<i<<") treeBased: ";
	//for(int j = 0;j<rep;j++)// repeat the query 10 times
	//{
		//Query the tree
		int R = 0;
		t2 = clock();
		//R = dg.query(0,0,499*i,499*i);
		R = dg.query(0,0,49*i,49*i);
		t2 = clock()-t2;
		///tree query
		//R = dg.query(0,0,i,i);
		//if(j==0 )
		cout<<"exact sum="<<R<<endl;

		//sumtime+=t2;
	//}
	//cout<<sumtime/rep<<endl;
	cout<<"time:"<<t2<<endl<<"========\n";
}
*/
////################### 3. Convert aggregate tree to array #####################
cout<<"\n#################################################"<<endl;	
cout<<"preparing Aggregate-Tree for Bitmap Generation"<<endl;
	t3 = clock();
	//dg.BFS();
	boost::dynamic_bitset<> Bit_representator;
	vector<int>* aggregates = dg.BFS_max_depth(Tree_level,Bit_representator);
////################### 4. Convert aggregate tree to Bitmap #####################
cout<<"\n#################################################"<<endl;	
cout<<"Aggregate-Tree Bitmap Generation"<<endl;
////######################## VVVVVVVV Here! VVVVVVVV ############################

Query_interface query_handler(dg.get_array(),dg.get_count(),dg.get_DimX(),dg.get_DimY(),dg.get_DimZ(),
								&(aggregates->at(0)),aggregates->size(), Bit_representator);//generate the bitmaps ready to query

/*
int result = query_handler.Query(query_region);
cout<<"combined result:"<<result<<endl;

query_handler.print_access_log();
float error = abs(R1-result)/float(R1);
cout<<"accuracy: "<<1-error<<endl;


cout<<"################ Approximate Query ###############"<<endl;
for(int i = 0 ; i<11;i++)// increase the query size
//for(int i = 0 ; i<4;i++)// increase the query size
{
	//tree based aggregation
	////cout<<"(0,0,"<<50*i<<","<<50*i<<") bitmap  based: ";
	//cout<<"(0,0,"<<i<<","<<i<<") bitmap  based: ";
	//for(int j = 0;j<rep;j++)// repeat the query 10 times
	//{
	//	//Query the tree
		int R1 = 0;
		t2 = clock();
		/////approximate Query
		//query_handler.query();
		//R1 = query_handler.query(0,0,i,i,dg.get_DimX(),dg.get_DimY());
		//R1 = query_handler.query(0,0,499*i,499*i,dg.get_DimX(),dg.get_DimY());
		R1 = query_handler.query(0,0,49*i,49*i,dg.get_DimX(),dg.get_DimY());
		//if(j==0 )
			cout<<" approx sum="<<R1<<endl;
		t2 = clock()-t2;
		//sumtime2+=t2;
	//}
	//cout<<sumtime2/rep<<endl;
		cout<<"time:"<<t2<<endl<<"========\n";
}
*/
	return 0;
}
