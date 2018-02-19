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
#define Tree_level 1
using namespace std;
//not
struct tree_node
{
	int value;
	int X1;
	int Y1;
	int X2;
	int Y2;
	int high;
	tree_node* first = NULL;
	tree_node* second = NULL;
	tree_node* third = NULL;
	tree_node* forth = NULL;
};
class DataGenerator
{
	private:
		vector< vector<int> > matrix;
		int DimX;
		int DimY;
		int lowerBound;
		int higherBound;
		int count;// number of elements in the matrix DimX*DimY
		//================ aggregate generation 
		int Devision_factor = 2;
		vector<int> aggregates;		
		int step = 0; //for debugging
		tree_node* root;
	public: 
		/*
			This class is to generate a 2d dimX by dimY matrix where the values
			are uniformly random integers ranging between lowBound and highBound.
		*/
		DataGenerator(int dimX, int dimY, int lowBound, int highBound)
		{
			// TODO: Check lowVal<highVAl
			DimX = dimX;
			DimY = dimY;
			lowerBound = lowBound;
			higherBound = highBound;
			count = dimX*dimY;
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
			vector<int> row ;
				for(int j=0 ;j<DimY;j++)
				{
					int range = higherBound - lowerBound;
    				int rnd = rand()%(range+1)+lowerBound;
					row.push_back(rnd);
				}
				matrix.push_back(row);
			}
		}
		
		/*
		prints the matrix
		*/
		void print()
		{
			int MaxX = matrix.size();
			for(int i=0;i<MaxX;i++)
			{
				int MaxY = matrix[i].size();
				for(int j=0 ;j<MaxY;j++)
				{
					cout<< matrix[i][j]<<"	";
				}
				cout<<endl;
			}
		}
		/*
		saves the matrix in a output file with the address dir
		*/
		void saveAs(string dir)
		{
			ofstream myfile;
			myfile.open (dir);
			int MaxX = matrix.size();
			for(int i=0;i<MaxX;i++)
			{
				int MaxY = matrix[i].size();
				for(int j=0 ;j<MaxY;j++)
				{
					myfile<< matrix[i][j];
					if(j!= MaxY-1)
						myfile<< ",";
				}
				myfile<<endl;
			}
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
				string next_line;
				cout<<"reading the file ..."<<endl;
				while(myfile>>next_line)
				{
					istringstream iss(next_line);
					vector<int> line_values;
					int next_value;
					while(iss>>next_value)
					{						
						line_values.push_back(next_value);
						if (iss.peek() == ',')
							iss.ignore();
						//cout<<next_value<<"|";
					}
					matrix.push_back(line_values);
					//cout<<endl;				
				}
			cout<<"The file read successfully"<<endl;	
			}
			else
				cout<<"couldn't open the file"<<endl;
			myfile.close();
			
		}
		/*
		returns the actual data
		*/
		vector <vector<int>> get_matrix()
		{
			return matrix;
		}
		int* get_array()
		{
			int* array = new int[DimX*DimY];	
			for(int i=0;i<DimX;i++)
			{
				for(int j=0; j< DimY;j++)
				{
					int key = i*DimY+j;
					array[key]= matrix[i][j];
					//cout<<"array["<<key<<"]:"<<array[key]<<endl;
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
		/*generates a quad-tree of the sum aggregate*/
		void generate_sum_tree()
		{
			root = sum_helper(0,0,DimX-1,DimY-1);
		}
		tree_node* sum_helper(int x1, int y1, int x2, int y2)
		{
			/*step++;
			assert(step != 50);*/
			//cout<<x1<<","<<y1<<"	"<<x2<<","<<y2<<endl;
			int xMid = (x1+x2)/2;
			int yMid = (y1+y2)/2;
			if(x1 == x2)
			{
				if (y1==y2)
				{
					/* . */
					//cout<<"case1 "<<endl;
					
					tree_node* node = new tree_node;
					node->value = matrix[x1][y1];
					node->X1 = x1;
					node->Y1 = y1;
					node->X2 = x1;
					node->Y2 = y1;
					return node;
				}
				else
				{
					/* __ */ 
					//cout<<"case2"<<endl;
					
					tree_node* first = sum_helper(x1,y1,x1,yMid);
					tree_node* second= sum_helper(x1,yMid+1,x1,y2);
					tree_node* node = new tree_node;
					node->value = (first->value+second->value);///2;
					node->first = first;
					node->second = second;
					//cout<< result<<endl;
					node->X1 = x1;
					node->Y1 = y1;
					node->X2 = x1;
					node->Y2 = y2;
					return node;
				}
			}
			else
			{
				if (y1==y2)
				{
					/* | */					
					//cout<<"case3"<<endl;
					
					tree_node* first = sum_helper(x1,y1,xMid,y1);
					tree_node* third = sum_helper(xMid+1,y1,x2,y1);
					tree_node* node = new tree_node;
					node->value = (first->value+third->value);///2;
					node->first = first;
					node->third = third;
					//cout<< result<<endl;
					node->X1 = x1;
					node->Y1 = y1;
					node->X2 = x2;
					node->Y2 = y1;
					return node;
				}
				else
				{
					/* + */
					//cout<<"case4"<<endl;
					
					tree_node* first = sum_helper(x1,y1,xMid,yMid);
					tree_node* second = sum_helper(x1,yMid+1,xMid,y2);
					tree_node* third = sum_helper(xMid+1,y1, x2,yMid);
					tree_node* forth = sum_helper(xMid+1,yMid+1,x2,y2);

					tree_node* node = new tree_node;
					node->value = (first->value+second->value+third->value+forth->value);///4;
					
					node->first = first;
					node->second = second;
					node->third = third;
					node->forth = forth;
					node->X1 = x1;
					node->Y1 = y1;
					node->X2 = x2;
					node->Y2 = y2;
					//cout<< result<<endl;
					return node;
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
				cout<<head->value<<"	("<<head->X1<<","<<head->Y1<<") ("<<head->X2<<","<<head->Y2<<")"<<endl;
				if((head->first)!= NULL)
					Q.push(head->first);
				if(head->second!= NULL)
					Q.push(head->second);
				if(head->third!= NULL)
					Q.push(head->third);
				if(head->forth!= NULL)
					Q.push(head->forth);
			}
			cout<<endl;
		}
		vector<int>* BFS_max_depth(int maxDepth,boost::dynamic_bitset<>& Bit_Representor)
		{			
			boost::dynamic_bitset<> bit_representor;
			//int last_full_level = log(3*count)/log(4)-1;
			if(maxDepth<0)
				return NULL;
			vector<int>* result = new vector<int>;
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
				cout<<head->value<<"	("<<head->X1<<","<<head->Y1<<") ("<<head->X2<<","<<head->Y2<<")"<<endl;
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

			}
			Bit_Representor = bit_representor;
			//cout<<"bit representor: "<<bit_representor<<" size:"<<bit_representor.size()<<endl;
			//cout<<result->at(0)<<"	"<<result->at(result->size()-1)<<endl;
			return result;
		}
		int query(int x1, int y1, int x2, int y2)
		{
			
			step = 0;
			int result = query_helper(x1, y1, x2, y2,root);
			cout<<"# of getvals:"<<step<<"\n";
			return result;
			
		}
		int query_helper(int x1, int y1, int x2, int y2, tree_node* node)
		{
			if(DebugMode)
				cout<<"Query("<<x1<<","<<y1<<" "<<x2<<","<<y2<<") over ("<<node->X1<<","<<node->Y1<<" "<<node->X2<<","<<node->Y2<<")"<<endl;
			//cout<<"=============================="<<endl;
			//step++;
			//assert(step!=10);
			if(x1==node->X1 && y1 == node->Y1 && x2==node->X2 && y2 == node->Y2)
			{
				//cout<<"exact value:"<<node->value<<endl;
				step++;
				return node->value;
			}
			float r1 = 0;
			float r2 = 0;
			float r3 = 0;
			float r4 = 0;
			
			pair<pair<int,int>, pair<int,int>>* a1 = NULL;
			pair<pair<int,int>, pair<int,int>>* a2 = NULL;
			pair<pair<int,int>, pair<int,int>>* a3 = NULL;
			pair<pair<int,int>, pair<int,int>>* a4 = NULL;
			if(node->first!=NULL)
				a1 = overlap(x1, y1, x2, y2, node->first);
			//cout<<"here0"<<endl;
			if(node->second!=NULL)
				a2 = overlap(x1, y1, x2, y2, node->second);
			//cout<<"here1"<<endl;
			if(node->third!=NULL)
				a3 = overlap(x1, y1, x2, y2, node->third);
			//cout<<"here2"<<endl;
			if(node->forth!=NULL)
				a4 = overlap(x1, y1, x2, y2, node->forth);
			//cout<<a1<<" "<<a2<<" "<<a3<<" "<<a4<<endl;
			if(a1!=NULL)
			{
				//cout<<"here0"<<endl;
				//cout<<(a1->first).first<<","<<(a1->first).second<<" "<<(a1->second).first<<","<<(a1->second).second<<endl;
				r1 = query_helper((a1->first).first,(a1->first).second,(a1->second).first,(a1->second).second, node->first);
			}
			/*else
				cout<<"a1 is null"<<endl;*/
			if(a2!=NULL)
			{
				//cout<<"here1"<<endl;
				//cout<<(a2->first).first<<","<<(a2->first).second<<" "<<(a2->second).first<<","<<(a2->second).second<<endl;
				r2 = query_helper((a2->first).first,(a2->first).second,(a2->second).first,(a2->second).second, node->second);
			}/*
			else
				cout<<"a2 is null"<<endl;*/
			if(a3!=NULL)
			{
				//cout<<"here2"<<endl;
				//cout<<(a3->first).first<<","<<(a3->first).second<<" "<<(a3->second).first<<","<<(a3->second).second<<endl;
				r3 = query_helper((a3->first).first,(a3->first).second,(a3->second).first,(a3->second).second, node->third);
			}
			/*else
				cout<<"a3 is null"<<endl;*/
			if(a4!=NULL)
			{
				//cout<<"here3"<<endl;
				//cout<<(a4->first).first<<","<<(a4->first).second<<" "<<(a4->second).first<<","<<(a4->second).second<<endl<<endl;
				r4 = query_helper((a4->first).first,(a4->first).second,(a4->second).first,(a4->second).second, node->forth);
			}
			/*else
				cout<<"a4 is null"<<endl;*/
			//cout<<"------------------------------------"<<endl;
			//cout<<"r1:"<<r1<<" r2:"<<r2<<" r3:"<<r3<<" r4:"<<r4<<" res:"<<(r1+r2+r3+r4)/4<<endl;
			return (r1+r2+r3+r4);
		}
		int max(int a, int b)
		{
			return (a>=b)?a:b;
		}
		int min(int a, int b)
		{
			return (a<=b)?a:b;
		}
		pair<pair<int,int>, pair<int,int>>* overlap(int x1, int y1, int x2, int y2, tree_node* node)
		{
			//cout<<"overlap("<<x1<<","<<y1<<" "<<x2<<","<<y2<<")	("<<node->X1<<","<<node->Y1<<" "<<node->X2<<","<<node->Y2<<")"<<endl;
			pair<pair<int,int>, pair<int,int>>* result = NULL;
			pair<int,int> top_left;
			pair<int,int> bottom_right;
			top_left.first = max(x1, node->X1);
			top_left.second = max(y1, node->Y1);
			bottom_right.first = min(x2, node->X2);
			bottom_right.second = min(y2, node->Y2);
			if(bottom_right.first<top_left.first || bottom_right.second<top_left.second ) // no overlap between query region and node area
			{
				return NULL;
			}
			result = new pair<pair<int,int>, pair<int,int>>;
			result->first.first = top_left.first;
			result->first.second = top_left.second;
			result->second.first = bottom_right.first;
			result->second.second = bottom_right.second;
			return result;
			
		}
		int query_base(int x1, int y1, int x2, int y2)
		{
			int sum = 0;
			for(int i = x1;i<=x2;i++)
				for(int j = y1;j<=y2;j++)
					sum+=matrix[i][j];
			//return (float) sum;//((x2-x1)*(y2-y1));
			return sum;
				
		}
		
};
int main (int argc, char** argv)
{ 	
////########################## 1. Data generation ###############################
cout<<"################ Data Generation/load ###############"<<endl;
	//cout<<argv[1]<<"	"<<argv[2]<<endl;
	
	// select mode here ______________________________________
	bool generate_mode = false;// 0 mean load; 1 means generate
	//________________________________________________________
	
	DataGenerator dg (stoi(argv[1]), stoi(argv[2]), 1,10);
	if(generate_mode){
		dg.generate();	
		dg.saveAs("./"+(string)argv[3]);
	}
	else{
		dg.load(argv[3]);
	}
	//dg.print();
	cout<<"===============>"<<endl;

clock_t t1,t2=0,t3=0,t4=0,sumtime=0,sumtime1=0,sumtime2=0;	

////######################## 2. Generate aggregate tree #########################
cout<<"################ Tree Generation #################"<<endl;
	t1 = clock();
	dg.generate_sum_tree();
	t1 = clock()-t1;
	cout<<"tree generation time:"<<((float)t1)/CLOCKS_PER_SEC<<endl;
int R1 = dg.query_base(0,0,1,1);
cout<<"exact query result:"<<R1<<endl;
/*int rep = 1;

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
////################### 4. Convert aggregate tree to Bitmap #####################	
cout<<"######### Aggregate-Tree Bitmap Generation #################"<<endl;
	t3 = clock();
	//dg.BFS();	
	boost::dynamic_bitset<> Bit_representator;
	vector<int>* aggregates = dg.BFS_max_depth(Tree_level,Bit_representator);
////######################## VVVVVVVV Here! VVVVVVVV ############################

Query_interface query_handler(dg.get_array(),dg.get_count(),dg.get_DimX(),dg.get_DimY(),&(aggregates->at(0)),aggregates->size(), Bit_representator);//generate the bitmaps ready to query
int result = query_handler.Query(0,0,0,0);
cout<<"combined result:"<<result<<endl;
/*
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
