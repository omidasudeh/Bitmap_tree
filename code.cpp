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
	size_t value; // this should be size_t
	
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
		vector<vector< vector<size_t> >> matrix;
		int DimX;
		int DimY;
		int DimZ;
		int lowerBound;
		int higherBound;
		int count;// number of elements in the matrix DimX*DimY*DimZ
		//================ aggregate generation
		int Devision_factor = 2; // each dimension devided to 2 as we go deep in the tree
		vector<size_t> aggregates;
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
				vector<vector<size_t>> plain ;
				for(int j=0 ;j<DimY;j++)
				{
					vector<size_t> row ;
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
					vector<vector<size_t>> plain ;
					for(int j=0 ;j<DimY;j++)
					{
						vector<size_t> row ;
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
		vector<vector <vector<size_t>>> get_matrix()
		{
			return matrix;
		}
		/*
		flatten the matrix
		*/
		size_t* get_array()
		{
			size_t* array = new size_t[DimX*DimY*DimZ];
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
			vector<vector< vector<size_t>> > aggregate_matrix;
			for(int i = 0; i<granularity; i++) // This will skip the boarders !!
			{
				vector<vector<size_t>> plain;
				for(int j = 0;j<granularity;j++)// This will skip the boarders !!
				{
					vector<size_t> row;
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
		tree_node* sum_helper(int x1, int y1,int z1, int x2, int y2,int z2,vector<vector< vector<size_t> >> matrix )
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
		vector<size_t>* BFS_max_depth(int maxDepth,boost::dynamic_bitset<>& Bit_Representor)
		{
			boost::dynamic_bitset<> bit_representor;
			//int last_full_level = log(3*count)/log(4)-1;
			if(maxDepth<0)
				return NULL;
			vector<size_t>* result = new vector<size_t>; // this should be size_t
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
		size_t query_base(int x1, int y1,int z1, int x2, int y2,int z2)
		{
			size_t sum = 0;	
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
	cout<<"--error: bitmapTree acceptance error; default error = 0"<<endl;
	
	bool generate_mode = false;// false mean load; true means generate
	int cardinality = 100;
	int dimSize = 512;
	int error = 0;
	string directory = "";
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
		if(arg == "--error")
		{
			i++;
			error = stoi(argv[i]);
		}
	}
	cout<<"\n#################################################"<<endl;	
	cout<< "Parameters set to:\n";
	cout<< "--generate:"<<generate_mode<<" cardinality: "<<cardinality<<endl;
	cout<<"--dimSize:"<<dimSize<<endl;
	if(directory!="")
		cout<< "--directory:"<<directory<<endl;
	else
		cout<<"Loads Bitmaps"<<endl;
	cout<<"--TreeLevel:"<<Tree_level<<endl;
	cout<<"--Query: ("<<query_region.first.x<<","<<query_region.first.y<<","<<query_region.first.z<<") ("
					  <<query_region.second.x<<","<<query_region.second.y<<","<<query_region.second.z<<")"<<endl;
	cout<<"--error:"<<error<<endl;
					  

	cout<<"======================\n";


DataGenerator dg (dimSize, dimSize,dimSize, 0,cardinality);
Query_interface* query_handler;
if(directory!="")
{
////########################## 1. Data generation ###############################
cout<<"\n############## Data Generation/load #############"<<endl;
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
	// cout<<x1<<","<<y1<<","<<z1<<","<<x2<<","<<y2<<","<<z2<<endl;
clock_t qt = clock(); 
size_t R1 = dg.query_base(query_region.first.x,query_region.first.y,query_region.
				first.z,query_region.second.x,query_region.second.y,query_region.second.z);
qt = clock()-qt;
cout<<"\nquery time:"<<qt<<endl;

cout<<"\n#################################################"<<endl;	
cout<<"exact query result:"<<R1<<endl;

////################### 3. Convert aggregate tree to array #####################
cout<<"\n#################################################"<<endl;	
cout<<"preparing Aggregate-Tree for Bitmap Generation"<<endl;
	t3 = clock();
	//dg.BFS();
	boost::dynamic_bitset<> Bit_representator;
	vector<size_t>* aggregates = dg.BFS_max_depth(Tree_level,Bit_representator);
cout<<"\n#################################################"<<endl;	
////######################## Query Here! VVVVVVVV ############################
	query_handler = new Query_interface("../data/bitmap/",dg.get_array(),dg.get_count(),dg.get_DimX(),dg.get_DimY(),dg.get_DimZ(),
								&(aggregates->at(0)),aggregates->size(), Bit_representator, error);//generate the bitmaps ready to query

}
else 
    query_handler = new Query_interface(dimSize,dimSize,dimSize, error);

// cout<<"here!!!\n";
int result = query_handler->Query(query_region);
cout<<"combined result:"<<result<<endl;
query_handler->print_access_log();


return 0;
}
