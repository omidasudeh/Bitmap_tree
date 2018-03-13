#include "Query_interface.h"
Query_interface::Query_interface(int* array,unsigned long items, int DX,int DY, int DZ,int* aggregates, unsigned long aggregate_items, boost::dynamic_bitset<> BR){
		cout<<"Creating Actual bitmap ...\n";
		bitmap = new Bitmap<int>(array,items);		
		bitmap->calcPreAgg();
		//bitmap->print_stat();
		DimX = DX;
		DimY = DY;
		DimZ = DZ;
		cout<<"Creating bitmap tree ...\n";
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

int Query_interface:: Query(pair<point,point> query_region){
	// pair<point, point>* root_region = new pair<point,point>;
	pair<point, point> root_region;
	root_region.first.x   = 0;
	root_region.first.y   = 0;
	root_region.first.z   = 0;	

	root_region.second.x  = DimX-1;
	root_region.second.y  = DimY-1;
	root_region.second.z  = DimZ-1;	
	
	//k0=0;
	//gtt = 0;
	//clock_t tq = clock(); 
	int result = TreeQuery(query_region,0,root_region);
	//gtt += clock()-tq;
	//cout<<"\ngtt:"<<gtt<<endl;
	//cout<<"# of getvals:"<<k0<<endl;
	return result;

}

int  Query_interface::TreeQuery(pair<point,point> query_region,int node_number, pair<point, point> node_region){
	// cout<<"tree\n";
	total_acccess++;
	tree_access++;
	// cout<<"tree query\n";
	int X1 = node_region.first.x;
	int Y1 = node_region.first.y;
	int Z1 = node_region.first.z;

	int X2 = node_region.second.x;
	int Y2 = node_region.second.y;
	int Z2 = node_region.second.z;
	
	
	//// if(match(query,region))
	if(query_region == node_region) // base case: if the query region match the node area
	{
		//clock_t tq = clock(); /////bottle neck!!!!!!!!!!!!!!!!!!!!
		pair<int,int>* node_min_max = bitmapTree->get_value(node_number);// get the bin statistic e.g. : [min:2,max: 10]
		//gtt += clock()-tq;					
		int node_value =  (node_min_max->first+node_min_max->second)/2;// assumption: represent the bin with the mid value of the bin range e.g. (2+10)/2=6
		return node_value;
	}	
	/*calculate the node number of the possible children*/
	int first_child_tree_index = 8*node_number+1;
	int first_child = bitmap_tree_index(first_child_tree_index);

	int second_child_tree_index= 8*node_number+2;
	int second_child = bitmap_tree_index(second_child_tree_index);
	
	int third_child_tree_index = 8*node_number+3;
	int third_child = bitmap_tree_index(third_child_tree_index);
	
	int forth_child_tree_index = 8*node_number+4;
	int forth_child = bitmap_tree_index(forth_child_tree_index);

	int fifth_child_tree_index = 8*node_number+5;
	int fifth_child = bitmap_tree_index(first_child_tree_index);

	int sixth_child_tree_index= 8*node_number+6;
	int sixth_child = bitmap_tree_index(second_child_tree_index);
	
	int seventh_child_tree_index = 8*node_number+7;
	int seventh_child = bitmap_tree_index(third_child_tree_index);
	
	int eighth_child_tree_index = 8*node_number+8;
	int eighth_child = bitmap_tree_index(forth_child_tree_index);
	
	//===========================================
	/*calculate the node region of the possible children*/
	int XMid = (X1+X2)/2;
	int YMid = (Y1+Y2)/2;
	int ZMid = (Y1+Y2)/2;
	pair<point,point>  first_child_region; ;//(x1,y1,xMid,yMid)
	first_child_region.first.x  = X1;
	first_child_region.first.y  = Y1;
	first_child_region.first.z  = Z1;	
	first_child_region.second.x = XMid;
	first_child_region.second.y = YMid;
	first_child_region.second.z = ZMid;
	
	pair<point,point>  second_child_region;//x1,yMid+1,xMid,y2
	second_child_region.first.x  = X1;
	second_child_region.first.y  = Y1;
	second_child_region.first.z  = ZMid+1;	
	second_child_region.second.x = XMid;
	second_child_region.second.y = YMid;
	second_child_region.second.z = Z2;

	pair<point,point> third_child_region;//(xMid+1,y1, x2,yMid)
	third_child_region.first.x  = X1;
	third_child_region.first.y  = YMid+1;
	third_child_region.first.z  = Z1;	
	third_child_region.second.x = XMid;
	third_child_region.second.y = Y2;
	third_child_region.second.z = ZMid;
	
	pair<point,point>  forth_child_region;//(xMid+1,yMid+1,x2,y2);
	forth_child_region.first.x  = X1;
	forth_child_region.first.y  = YMid+1;
	forth_child_region.first.z  = ZMid+1;	
	forth_child_region.second.x = XMid;
	forth_child_region.second.y = Y2;
	forth_child_region.second.z = Z2;
		
	pair<point,point>  fifth_child_region; ;//(x1,y1,xMid,yMid)
	fifth_child_region.first.x  = XMid+1;
	fifth_child_region.first.y  = Y1;
	fifth_child_region.first.z  = Z1;	
	fifth_child_region.second.x = X2;
	fifth_child_region.second.y = YMid;
	fifth_child_region.second.z = ZMid;
	
	pair<point,point>  sixth_child_region;//x1,yMid+1,xMid,y2
	sixth_child_region.first.x  = XMid+1;
	sixth_child_region.first.y  = Y1;
	sixth_child_region.first.z  = ZMid+1;	
	sixth_child_region.second.x = X2;
	sixth_child_region.second.y = YMid;
	sixth_child_region.second.z = Z2;

	pair<point,point> seventh_child_region;//(xMid+1,y1, x2,yMid)
	seventh_child_region.first.x  = XMid+1;
	seventh_child_region.first.y  = YMid+1;
	seventh_child_region.first.z  = Z1;	
	seventh_child_region.second.x = X2;
	seventh_child_region.second.y = Y2;
	seventh_child_region.second.z = ZMid;
	
	pair<point,point>  eighth_child_region;//(xMid+1,yMid+1,x2,y2);
	eighth_child_region.first.x  = XMid+1;
	eighth_child_region.first.y  = YMid+1;
	eighth_child_region.first.z  = ZMid+1;	
	eighth_child_region.second.x = X2;
	eighth_child_region.second.y = Y2;
	eighth_child_region.second.z = Z2;
	
	//// subqueries results
	int r1 = 0;
	int r2 = 0;
	int r3 = 0;
	int r4 = 0;
	int r5 = 0;
	int r6 = 0;
	int r7 = 0;
	int r8 = 0;
	///// sub regions. set them null as default
	pair<point,point> a1,a2,a3,a4,a5,a6,a7,a8;
	
	//// calculate the subqury regions for the children
	//cout<<"first_child:" <<first_child<<endl;
	
	a1 = TreeOverlap(query_region, first_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<first_child_region->first.first<<","<<first_child_region->first.second<<","<<
		//first_child_region->second.first<<","<<first_child_region->second.second<<endl;
	if(a1!=NULL)
	{
		if(first_child>0) // if the first_child exists
		{
			r1 = TreeQuery(a1, first_child,first_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
			
		}
	}
	
	a2 = TreeOverlap(query_region, second_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<second_child_region->first.first<<","<<second_child_region->first.second<<","<<
		//second_child_region->second.first<<","<<second_child_region->second.second<<endl;
	if(a2!=NULL)
	{
		if(second_child>0) // if the second_child exists
		{
			r2 = TreeQuery(a2, second_child,second_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
			
		}
	}
	
	a3 = TreeOverlap(query_region, third_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<third_child_region->first.first<<","<<third_child_region->first.second<<","<<
		//third_child_region->second.first<<","<<third_child_region->second.second<<endl;
	if(a3!=NULL)
	{
		if(third_child>0) // if the third_child exists
		{		
			r3 = TreeQuery(a3, third_child,third_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
		}
	}
	a4 = TreeOverlap(query_region, forth_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<forth_child_region->first.first<<","<<forth_child_region->first.second<<","<<
		//forth_child_region->second.first<<","<<forth_child_region->second.second<<endl;
	if(a4!=NULL)
	{
		if(forth_child>0) // if the fourth_child exists
		{	
			r4 = TreeQuery(a4, forth_child,forth_child_region);
		}
		else
		{
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			return  BitmapQuery(Pv,Pdx,Pdy);
		}	
	}
	//cout<<r1<<" "<<r2<<" "<<r3<<" "<<r4<<" "<<r1+r2+r3+r4<<endl;
	//assert(false);
	return r1+r2+r3+r4+r5+r6+r7+r8;
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
pair<point, point> Query_interface::TreeOverlap(pair<point, point> query_region,pair<point, point> node_region)
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
	// cout<<"bitmap\n";
	total_acccess++;
	bitmap_access++;
	//============== value based filtering; lines 1-7 in 2015 paper algo.
	vector<int> value_based_filtered_bins;
	int i = 0;
	for(auto bin:*(bitmap->get_firstlevelvalue()))
	{
		if(rangeOverlap(pair<int, int>(bin.min_val/bitmap->get_numpres(), bin.max_val/bitmap->get_numpres()),Pv))
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
	//cout<<endl;
	unordered_map<int,int>* count_array = new unordered_map<int,int>;
	for(int bin_number:value_based_filtered_bins)
	{
		vector<size_t> result = Bitops.logic_and(bitmap->get_firstlevelvector(bin_number),transalted_pd); // make sure to do it by reference
		boost::dynamic_bitset<> bit_result = Bitops.uncompressIndex(result, bitmap->get_count());// unefficient!!
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
		
		int binNumber = elem.first;
		float count = (float)elem.second;
		
		//cout<<"bin#:"<<binNumber<<" count:"<<count<<endl;
		
		
		float stat_sum = bitmap->get_first_level_sum(binNumber);
		
		float stat_count = bitmap->get_first_level_count(binNumber);
		
		approx_sum+=stat_sum*(count/stat_count);
		//cout<<stat_sum<<"*("<<count<<"/"<<stat_count<<") = "<<stat_sum*(count/stat_count)<<"	curAppSum:"<<approx_sum<<endl;
		
	}
	//cout<<approx_sum<<endl;
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
	boost::dynamic_bitset<> pd(bitmap->get_count());// make an output bitvector of the size of the input data
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
void Query_interface::print_access_log()
{
	cout<<"================ access report ===================\n";
	cout<<"number of total access to both bitmaps: "<<total_acccess<<endl;
	cout<<"Actual bitmap access ratio: "<<float(bitmap_access)/float(total_acccess)<<endl;
	cout<<"Bitmap tree access ratio: "<<float(tree_access)/float(total_acccess)<<endl;	
}