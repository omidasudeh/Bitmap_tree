#include "Query_interface.h"
Query_interface::Query_interface(int* array,unsigned long items, int DX,int DY,int* aggregates, unsigned long aggregate_items, boost::dynamic_bitset<> BR){
		bitmap = new Bitmap<int>(array,items);
		bitmap->calcPreAgg();
		//bitmap->print_stat();
		DimX = DX;
		DimY = DY;
		bitmapTree = new Bitmap<int>(aggregates,aggregate_items);
		bitmapTree->calcPreAgg();
		//bitmapTree->print_stat();
		this->Bit_representator = BR;
		//this->compressed_Bit_representator = compressBitset(BR);
	}
Query_interface::~Query_interface(){
	delete bitmapTree;
	delete bitmap;
	}

int Query_interface:: Query(int x1, int y1, int x2, int y2){
	pair<pair<int,int>, pair<int,int>>* root_region = new pair<pair<int,int>, pair<int,int>>;
	root_region->first.first   = 0;
	root_region->first.second  = 0;	
	root_region->second.first  = DimX-1;
	root_region->second.second = DimY-1;
	
	//k0=0;
	//gtt = 0;
	//clock_t tq = clock(); 
	int result = TreeQuery(x1, y1, x2, y2,0,root_region);
	//gtt += clock()-tq;
	//cout<<"\ngtt:"<<gtt<<endl;
	//cout<<"# of getvals:"<<k0<<endl;
	return result;

}

int  Query_interface::TreeQuery(int x1, int y1, int x2, int y2,int node_number,pair<pair<int,int>, pair<int,int>>* node_region){
	
	int X1 = node_region->first.first;
	int Y1 = node_region->first.second;
	int X2 = node_region->second.first;
	int Y2 = node_region->second.second;
	
	//// if(match(query,region))
	if(x1 == X1 && y1 == Y1 && x2 == X2 && y2 == Y2) // base case: if the query region match the node area
	{
		//clock_t tq = clock(); /////bottle neck!!!!!!!!!!!!!!!!!!!!
		pair<int,int>* node_min_max = bitmapTree->get_value(node_number);// get the bin statistic e.g. : [min:2,max: 10]
		//gtt += clock()-tq;					
		int node_value =  (node_min_max->first+node_min_max->second)/2;// assumption: represent the bin with the mid value of the bin range e.g. (2+10)/2=6
		return node_value;
	}	
	/*calculate the node number of the possible children*/
	int first_child_tree_index = 4*node_number+1;
	int first_child = bitmap_tree_index(first_child_tree_index);

	int second_child_tree_index= 4*node_number+2;
	int second_child = bitmap_tree_index(second_child_tree_index);
	
	int third_child_tree_index = 4*node_number+3;
	int third_child = bitmap_tree_index(third_child_tree_index);
	
	int forth_child_tree_index = 4*node_number+4;
	int forth_child = bitmap_tree_index(forth_child_tree_index);
	
	//===========================================
	/*calculate the node region of the possible children*/
	int XMid = (X1+X2)/2;
	int YMid = (Y1+Y2)/2;
	pair<pair<int,int>, pair<int,int>>* first_child_region = new pair<pair<int,int>, pair<int,int>> ;//(x1,y1,xMid,yMid)
	first_child_region->first.first   = X1;
	first_child_region->first.second  = Y1;	
	first_child_region->second.first  = XMid;
	first_child_region->second.second = YMid;
	pair<pair<int,int>, pair<int,int>>* second_child_region = new pair<pair<int,int>, pair<int,int>>;//x1,yMid+1,xMid,y2
	second_child_region->first.first   = X1;
	second_child_region->first.second  = YMid+1;	
	second_child_region->second.first  = XMid;
	second_child_region->second.second = Y2;
	pair<pair<int,int>, pair<int,int>>* third_child_region = new pair<pair<int,int>, pair<int,int>>;//(xMid+1,y1, x2,yMid)
	third_child_region->first.first   = XMid+1;
	third_child_region->first.second  = Y1;	
	third_child_region->second.first  = X2;
	third_child_region->second.second = YMid;
	pair<pair<int,int>, pair<int,int>>* forth_child_region = new pair<pair<int,int>, pair<int,int>>;//(xMid+1,yMid+1,x2,y2);
	forth_child_region->first.first   = XMid+1;
	forth_child_region->first.second  = YMid+1;	
	forth_child_region->second.first  = X2;
	forth_child_region->second.second = Y2;
	
	//// subqueries results
	int r1 = 0;
	int r2 = 0;
	int r3 = 0;
	int r4 = 0;
	///// sub regions. set them null as default
	pair<pair<int,int>, pair<int,int>>* a1 = NULL;
	pair<pair<int,int>, pair<int,int>>* a2 = NULL;
	pair<pair<int,int>, pair<int,int>>* a3 = NULL;
	pair<pair<int,int>, pair<int,int>>* a4 = NULL;
	//// calculate the subqury regions for the children
	//cout<<"first_child:" <<first_child<<endl;
	
	a1 = TreeOverlap(x1, y1, x2, y2, first_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<first_child_region->first.first<<","<<first_child_region->first.second<<","<<
		//first_child_region->second.first<<","<<first_child_region->second.second<<endl;
	if(a1!=NULL)
	{
		if(first_child>0) // if the first_child exists
		{
			r1 = TreeQuery((a1->first).first,(a1->first).second,(a1->second).first,(a1->second).second, first_child,first_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			//cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
			
		}
	}
	
	a2 = TreeOverlap(x1, y1, x2, y2, second_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<second_child_region->first.first<<","<<second_child_region->first.second<<","<<
		//second_child_region->second.first<<","<<second_child_region->second.second<<endl;
	if(a2!=NULL)
	{
		if(second_child>0) // if the second_child exists
		{
			r2 = TreeQuery((a2->first).first,(a2->first).second,(a2->second).first,(a2->second).second, second_child,second_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			//cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
			
		}
	}
	
	a3 = TreeOverlap(x1, y1, x2, y2, third_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<third_child_region->first.first<<","<<third_child_region->first.second<<","<<
		//third_child_region->second.first<<","<<third_child_region->second.second<<endl;
	if(a3!=NULL)
	{
		if(third_child>0) // if the third_child exists
		{		
			r3 = TreeQuery((a3->first).first,(a3->first).second,(a3->second).first,(a3->second).second, third_child,third_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			//cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
		}
	}
	a4 = TreeOverlap(x1, y1, x2, y2, forth_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<forth_child_region->first.first<<","<<forth_child_region->first.second<<","<<
		//forth_child_region->second.first<<","<<forth_child_region->second.second<<endl;
	if(a4!=NULL)
	{
		if(forth_child>0) // if the fourth_child exists
		{	
			r4 = TreeQuery((a4->first).first,(a4->first).second,(a4->second).first,(a4->second).second, forth_child,forth_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			//cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
		}	
	}
	//cout<<r1<<" "<<r2<<" "<<r3<<" "<<r4<<" "<<r1+r2+r3+r4<<endl;
	//assert(false);
	return r1+r2+r3+r4;
}

int Query_interface::bitmap_tree_index(int node_number){
	if(node_number>=bitmapTree->get_count()) // no such child exists
		return -1;
	
	int index=0;
	//test_count();
	//assert(false);
	
	/*
	////naive based count on compressed bit_representor
	boost::dynamic_bitset<> shifted_bit_representator;
	int shifts = Bit_representator.size()-node_number-1;
	shifted_bit_representator = Bit_representator<<(shifts);
	int ones = shifted_bit_representator.count()-1;
	return ones;
	*/
	
	/*
	//fast count on compressed bit_representor
	//cout<<"compressed_Bit_representator.size:"<<compressed_Bit_representator.size()<<endl;
	//clock_t tq = clock(); 	
		
	int ones = CountOnesUpto(compressed_Bit_representator,node_number);
	//gtt+=clock()-tq;

	return ones;
	*/
	index = node_number;

	return  index;
}
pair<pair<int,int>, pair<int,int>>* Query_interface::TreeOverlap(int x1, int y1, int x2, int y2,pair<pair<int,int>, pair<int,int>>* node_region)
{
	//// calculate the subqury regions for the children

	pair<pair<int,int>, pair<int,int>>* result = NULL; //programming problem local object is being returned!
	pair<int,int> top_left;
	pair<int,int> bottom_right;
	//cout<<"overlap get region---| "<<endl;
	//cout<<"---------------------|\n"<<endl;
	int X1 = node_region->first.first;
	int Y1 = node_region->first.second;
	int X2 = node_region->second.first;
	int Y2 = node_region->second.second;
	//cout<<"overlap("<<x1<<","<<y1<<" "<<x2<<","<<y2<<") over ("<<X1<<","<<Y1<<" "<<X2<<","<<Y2<<")"<<endl;
	top_left.first = (x1>X1)?x1:X1;//max(x1,X1)
	top_left.second = (y1>Y1)?y1:Y1;//max(y1,Y1)
	bottom_right.first = (x2<=X2)?x2:X2;//min(x2,X2)
	bottom_right.second = (y2<=Y2)?y2:Y2;//min(y2,Y2)
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
float Query_interface::BitmapQuery(vector<pair<int, int>> Pv, vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy)
{
	//============== value based filtering; lines 1-7 in 2015 paper algo.
	vector<int> value_based_filtered_bins;
	int i = 0;
	for(auto elem:*(bitmap->get_firstlevelvalue()))
	{
		if(rangeOverlap(pair<int, int>(elem.min_val, elem.max_val),Pv))
		{
			value_based_filtered_bins.push_back(i);
		}
		i++;
	}
	//assert(false);
	//============== dimension based filtering ; lines 9-15 in 2015 paper algo.
	
	//cout<<"here3"<<Pdx[0].first<<","<<Pdx[0].second<<Pdy[0].first<<","<<Pdy[0].second<<endl;
	vector<size_t> transalted_pd = translate(Pdx, Pdy);
	//for(auto w:transalted_pd)
		//cout<<w;
	cout<<endl;
	unordered_map<int,int>* count_array = new unordered_map<int,int>;
	for(int bin_number:value_based_filtered_bins)
	{
		vector<size_t> result = Bitops.logic_and(bitmap->get_firstlevelvector(bin_number),transalted_pd); // make sure to do it by reference
		boost::dynamic_bitset<> bit_result(bitmap->get_count(),result[0]); // copy problem efficiency issue
		count_array->insert(pair<int, int> (bin_number,bit_result.count()));  // uses the boost's count function
	}
	//assert(false);
	//for (auto elem:*count_array)
		//cout<<"bin"<<elem.first<<" count:"<<elem.second<<"	";
	float approx_sum = approximate_sum(count_array);
	//cout<<"approx sum:"<<approx_sum<<endl;
	return approx_sum;
}

float Query_interface::approximate_sum(unordered_map<int,int> * count_array)
{
	float approx_sum = 0.0;
	for(auto elem:*count_array)
	{
		
		int key = elem.first;
		//cout<<"bin#:"<<key<<" count:"<<elem.second<<endl;
		float count = (float)elem.second;

		float stat_sum = bitmap->get_first_level_sum(key);
		
		float stat_count = bitmap->get_first_level_count(key);
		
		approx_sum+=stat_sum*(count/stat_count);
		//cout<<stat_sum<<"*("<<count<<"/"<<stat_count<<") = "<<stat_sum*(count/stat_count)<<endl;
		
	}
	cout<<approx_sum<<endl;
	return approx_sum;
}
bool Query_interface::rangeOverlap(pair<int, int> b, vector<pair<int,int>> Pv)
{
	for(auto R: Pv)
	{

		if(((b.second/bitmap->get_numpres())>=R.first) && ((b.first/bitmap->get_numpres())<=R.second))
			return true;
	}
	return false;
}

/*translates the dimension based query into a bitvector predicate*/
vector<size_t> Query_interface::translate (vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy)
{
	//cout<<"here2"<<endl;
	boost::dynamic_bitset<> pd(bitmap->get_count(),0);// make an output bitvector of the size of the input data
	for(auto x_pair:Pdx )// for each partial range of x-dimension in the query
	{
		int x1 = x_pair.first;
		int x2 = x_pair.second;
		//cout<<"here1"<<endl;
		for(auto y_pair: Pdy)//for each partial range of ranges of y-dimension in the query
		{
			int y1 = y_pair.first;
			int y2 = y_pair.second;
			//cout<<"here"<<x1<<","<<y1<<","<<x2<<","<<y2<<endl;
			for(int i = x1;i<=x2;i++)// sweep the x-axis
			{
				for(int j= y1;j<=y2;j++)// sweep the y-axis
				{
					//cout<<"i:"<<i<<" j:"<<j<<endl;
					pd[i*DimY+j]=1;
				}
				
			}
		}
	}
	//cout<<pd<<endl;
	return Bitops.compressBitset(pd);
}
