#include "Query_interface.h"
#include <omp.h>
Query_interface::Query_interface(string dir,size_t* array,size_t items, int DX,int DY, int DZ,size_t* aggregates, size_t aggregate_items, boost::dynamic_bitset<> BR, int error){
		
		cout<<"Creating bitmap tree ...\n";
		cout<<"items:"<<aggregate_items<<endl;		
		bitmapTree = new Bitmap<size_t>(dir+"bitmapTree/",aggregates,aggregate_items);
		//bitmapTree->print_stat();
		this->Bit_representator = BR;
		//this->compressed_Bit_representator = compressBitset(BR);

		cout<<"Creating Actual bitmap ...\n";
		cout<<"items:"<<items<<endl;
		bitmap = new Bitmap<size_t>(dir+"bitmap/",array,items);	
		//bitmap->print_stat();
		DimX = DX;
		DimY = DY;
		DimZ = DZ;
        this->error = error;
        
	}
Query_interface::Query_interface(int DX,int DY, int DZ, int error)
{
	bitmapTree = new Bitmap<size_t>("../data/bitmap/bitmapTree/");
	bitmap = new Bitmap<size_t>("../data/bitmap/bitmap/");
	DimX = DX;
	DimY = DY;
	DimZ = DZ;
    this->error = error;
	// cout<<bitmap->get_count()<<"  "<<bitmapTree->get_count()<<endl;
}
Query_interface::~Query_interface(){
	delete bitmapTree;
	delete bitmap;
	}

int Query_interface:: Query(pair<point,point> query_region){
	// pair<point, point>* root_region = new pair<point,point>;
	total_acccess = 0;
	tree_access = 0;
	bitmap_access = 0;

	pair<point, point> root_region;
	root_region.first.x   = 0;
	root_region.first.y   = 0;
	root_region.first.z   = 0;	

	root_region.second.x  = DimX-1;
	root_region.second.y  = DimY-1;
	root_region.second.z  = DimZ-1;	

	clock_t qt = clock(); 
	int result = 0;
	if(isThinQuery(query_region, thinness_ratio))
	{
		vector<pair<int, int>> Pv; 
		Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
		vector<pair<int, int>> Pdx; 
		Pdx.push_back(pair<int, int>(query_region.first.x ,query_region.second.x)); 
		vector<pair<int, int>> Pdy; 
		Pdy.push_back(pair<int, int>(query_region.first.y ,query_region.second.y)); 
		vector<pair<int, int>> Pdz; 
		Pdz.push_back(pair<int, int>(query_region.first.z ,query_region.second.z)); 
		result = BitmapQuery(Pv,Pdx,Pdy,Pdz);
	}
	else
		result = TreeQuery(query_region,0,root_region);
	qt = clock()-qt;
	cout<<"\nall query time:"<<qt<<endl;
	cout<<"bitmap time:"<<tt<<endl;
	
	return result;

}
bool Query_interface::isThinQuery(pair<point,point> query_region,float ratio)
{
	// if the width/length ratio is less ratio it returns false. O.w true.
	float x_len = abs( query_region.first.x - query_region.second.x );
	float y_len = abs( query_region.first.y - query_region.second.y );
	float z_len = abs( query_region.first.z - query_region.second.z );


	if(x_len/y_len < ratio || x_len/z_len < ratio || y_len/z_len < ratio ||
	   y_len/x_len < ratio || z_len/x_len < ratio || z_len/y_len < ratio)
	{
	//    query_region.first.print();
	//    query_region.second.print();	   
	//    cout<<"thin region:("<<x_len<<","<<y_len<<","<<z_len<<")\n";
	   return true;
	}
	else
		return false;	
}

int  Query_interface::TreeQuery(pair<point,point> query_region,int node_number, pair<point, point> node_region){

	int X1 = node_region.first.x;
	int Y1 = node_region.first.y;
	int Z1 = node_region.first.z;

	int X2 = node_region.second.x;
	int Y2 = node_region.second.y;
	int Z2 = node_region.second.z;
	
	int x1 = query_region.first.x;
	int y1 = query_region.first.y;
	int z1 = query_region.first.z;

	int x2 = query_region.second.x;
	int y2 = query_region.second.y;
	int z2 = query_region.second.z;
	// cout<<"tree query:("<<x1<<","<<y1<<","<<z1<<")("<<x2<<","<<y2<<","<<z2<<")\n";	

	// query_region.first.print();
	// query_region.second.print();
	
	// node_region.first.print();
	// node_region.second.print();
	
	// if(query_region-node_region<=10)
	// if(query_region == node_region) // base case: if the query region match the node area
	if(match(query_region, node_region,error))
    {
		
		//clock_t tq = clock(); /////bottle neck!!!!!!!!!!!!!!!!!!!!
		pair<int,int>* node_min_max = bitmapTree->get_value(node_number);// get the bin statistic e.g. : [min:2,max: 10]
		
		//gtt += clock()-tq;					
		int node_value =  (node_min_max->first+node_min_max->second)/2;// assumption: represent the bin with the mid value of the bin range e.g. (2+10)/2=6
		// cout<<"tree\n";	
		total_acccess++;
		tree_access++;
		// cout<<"tree subquery result:"<<node_value<<endl;
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
	///// subquery regions. set them null initially
	pair<point,point>* a1 = NULL;
	pair<point,point>* a2 = NULL;
	pair<point,point>* a3 = NULL;
	pair<point,point>* a4 = NULL;
	pair<point,point>* a5 = NULL;
	pair<point,point>* a6 = NULL;
	pair<point,point>* a7 = NULL;
	pair<point,point>* a8 = NULL;
	
	
	//// calculate the subqury regions for the children
	//cout<<"first_child:" <<first_child<<endl;
	a1 = TreeOverlap(query_region, first_child_region);

	// cout<<"come on0\n";
	// cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<first_child_region->first.first<<","<<first_child_region->first.second<<","<<
		//first_child_region->second.first<<","<<first_child_region->second.second<<endl;
	if(a1!=NULL)
	{		
		if(first_child>0 && !isThinQuery(*a1, thinness_ratio)) // if the first_child exists
		{
			r1 = TreeQuery(*a1, first_child,first_child_region);
		}
		else
		{
			int x1 = a1->first.x;
			int y1 = a1->first.y;
			int z1 = a1->first.z;

			int x2 = a1->second.x;
			int y2 = a1->second.y;
			int z2 = a1->second.z;
			
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r1 =  BitmapQuery(Pv,Pdx,Pdy,Pdz);
		}
	}
	// cout<<"come on1\n";
	a2 = TreeOverlap(query_region, second_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<second_child_region->first.first<<","<<second_child_region->first.second<<","<<
		//second_child_region->second.first<<","<<second_child_region->second.second<<endl;
	
	if(a2!=NULL)
	{
		if(second_child>0&& !isThinQuery(*a2, thinness_ratio)) // if the second_child exists
		{
			r2 = TreeQuery(*a2, second_child,second_child_region);
		}
		else
		{
			int x1 = a2->first.x;
			int y1 = a2->first.y;
			int z1 = a2->first.z;

			int x2 = a2->second.x;
			int y2 = a2->second.y;
			int z2 = a2->second.z;
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r2 =  BitmapQuery(Pv,Pdx,Pdy,Pdz);
			
		}
	}
	// cout<<"come on2\n";
	a3 = TreeOverlap(query_region, third_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<third_child_region->first.first<<","<<third_child_region->first.second<<","<<
		//third_child_region->second.first<<","<<third_child_region->second.second<<endl;
	if(a3!=NULL)
	{
		if(third_child>0&& !isThinQuery(*a3, thinness_ratio)) // if the third_child exists
		{		
			r3 = TreeQuery(*a3, third_child,third_child_region);
		}
		else
		{
			int x1 = a3->first.x;
			int y1 = a3->first.y;
			int z1 = a3->first.z;

			int x2 = a3->second.x;
			int y2 = a3->second.y;
			int z2 = a3->second.z;
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r3 =  BitmapQuery(Pv,Pdx,Pdy,Pdz);
		}
	}
	// cout<<"come on3\n";
	a4 = TreeOverlap(query_region, forth_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<forth_child_region->first.first<<","<<forth_child_region->first.second<<","<<
		//forth_child_region->second.first<<","<<forth_child_region->second.second<<endl;
	if(a4!=NULL)
	{
		if(forth_child>0&& !isThinQuery(*a4, thinness_ratio)) // if the fourth_child exists
		{	
			r4= TreeQuery(*a4, forth_child,forth_child_region);
		}
		else
		{
			int x1 = a4->first.x;
			int y1 = a4->first.y;
			int z1 = a4->first.z;

			int x2 = a4->second.x;
			int y2 = a4->second.y;
			int z2 = a4->second.z;
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r4 = BitmapQuery(Pv,Pdx,Pdy,Pdz);
		}	
	}
	// cout<<"come on4\n";
	a5 = TreeOverlap(query_region, fifth_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<forth_child_region->first.first<<","<<forth_child_region->first.second<<","<<
		//forth_child_region->second.first<<","<<forth_child_region->second.second<<endl;
	if(a5!=NULL)
	{
		if(fifth_child>0&& !isThinQuery(*a5, thinness_ratio)) // if the fourth_child exists
		{	
			r5= TreeQuery(*a5, fifth_child,fifth_child_region);
		}
		else
		{
			int x1 = a5->first.x;
			int y1 = a5->first.y;
			int z1 = a5->first.z;

			int x2 = a5->second.x;
			int y2 = a5->second.y;
			int z2 = a5->second.z;
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r5 =  BitmapQuery(Pv,Pdx,Pdy,Pdz);
		}	
	}
	// cout<<"come on5\n";
	a6 = TreeOverlap(query_region, sixth_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<second_child_region->first.first<<","<<second_child_region->first.second<<","<<
		//second_child_region->second.first<<","<<second_child_region->second.second<<endl;
	if(a6!=NULL)
	{
		if(sixth_child>0&& !isThinQuery(*a6, thinness_ratio)) // if the second_child exists
		{
			r6 = TreeQuery(*a6, sixth_child,sixth_child_region);
		}
		else
		{
			int x1 = a6->first.x;
			int y1 = a6->first.y;
			int z1 = a6->first.z;

			int x2 = a6->second.x;
			int y2 = a6->second.y;
			int z2 = a6->second.z;
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r6 = BitmapQuery(Pv,Pdx,Pdy,Pdz);
			
		}
	}
	// cout<<"come on6\n";
	a7 = TreeOverlap(query_region, seventh_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<third_child_region->first.first<<","<<third_child_region->first.second<<","<<
		//third_child_region->second.first<<","<<third_child_region->second.second<<endl;
	if(a7!=NULL)
	{
		if(seventh_child>0&& !isThinQuery(*a7, thinness_ratio)) // if the third_child exists
		{		
			r7 = TreeQuery(*a7, seventh_child,seventh_child_region);
		}
		else
		{
			int x1 = a7->first.x;
			int y1 = a7->first.y;
			int z1 = a7->first.z;

			int x2 = a7->second.x;
			int y2 = a7->second.y;
			int z2 = a7->second.z;
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r7 = BitmapQuery(Pv,Pdx,Pdy,Pdz);
		}
	}
	// cout<<"come on7\n";
	a8 = TreeOverlap(query_region, eighth_child_region);
	//cout<<x1<<","<< y1<<","<< x2<<"," <<y2<<","<<forth_child_region->first.first<<","<<forth_child_region->first.second<<","<<
		//forth_child_region->second.first<<","<<forth_child_region->second.second<<endl;
	if(a8!=NULL)
	{
		if(eighth_child>0&& !isThinQuery(*a8, thinness_ratio)) // if the fourth_child exists
		{	
			r8 = TreeQuery(*a8, eighth_child,eighth_child_region);
		}
		else
		{
			int x1 = a8->first.x;
			int y1 = a8->first.y;
			int z1 = a8->first.z;

			int x2 = a8->second.x;
			int y2 = a8->second.y;
			int z2 = a8->second.z;
			vector<pair<int, int>> Pv; 
			Pv.push_back(pair<int, int>(-100000,100000)); // skip the value_based filtering
			vector<pair<int, int>> Pdx; 
			Pdx.push_back(pair<int, int>(x1 ,x2)); 
			vector<pair<int, int>> Pdy; 
			Pdy.push_back(pair<int, int>(y1,y2)); 
			vector<pair<int, int>> Pdz; 
			Pdz.push_back(pair<int, int>(z1,z2)); 
			// cout<<"bitmap query:"<<a1->first.first<<","<<a1->first.second<<","<<a1->second.first<<","<<a1->second.second<<","<<endl;
			r8 = BitmapQuery(Pv,Pdx,Pdy,Pdz);
		}	
	}
	// cout<<"come on8\n";
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
pair<point, point>* Query_interface::TreeOverlap(pair<point, point> query_region,pair<point, point> node_region)
{	
	//// calculate the subqury regions for the children

	pair<point, point>* result = NULL; 
	point top_left;
	point bottom_right;
	//cout<<"overlap get region---| "<<endl;
	//cout<<"---------------------|\n"<<endl;

	int X1 = node_region.first.x;
	int Y1 = node_region.first.y;
	int Z1 = node_region.first.z;
	
	int X2 = node_region.second.x;
	int Y2 = node_region.second.y;
	int Z2 = node_region.second.z;

	int x1 = query_region.first.x;
	int y1 = query_region.first.y;
	int z1 = query_region.first.z;

	int x2 = query_region.second.x;
	int y2 = query_region.second.y;
	int z2 = query_region.second.z;
	
	//cout<<"overlap("<<x1<<","<<y1<<" "<<x2<<","<<y2<<") over ("<<X1<<","<<Y1<<" "<<X2<<","<<Y2<<")"<<endl;
	top_left.x = (x1>X1)?x1:X1;//max(x1,X1)
	top_left.y = (y1>Y1)?y1:Y1;//max(y1,Y1)
	top_left.z = (z1>Z1)?z1:Z1;//max(z1,Z1)
	bottom_right.x = (x2<=X2)?x2:X2;//min(x2,X2)
	bottom_right.y = (y2<=Y2)?y2:Y2;//min(y2,Y2)
	bottom_right.z = (z2<=Z2)?z2:Z2;//min(z2,Z2)
	if(bottom_right.x<top_left.x || bottom_right.y<top_left.y ||  bottom_right.z<top_left.z ) // no overlap between query region and node area
	{
		return NULL;
	}
	result = new pair<point,point>;
	result->first.x = top_left.x;
	result->first.y = top_left.y;
	result->first.z = top_left.z;
	
	result->second.x = bottom_right.x;
	result->second.y = bottom_right.y;
	result->second.z  = bottom_right.z;
	
	return result;
	
}
bool Query_interface::match(pair<point, point> query_region,pair<point, point> node_region, int error)
{	
	int X1 = node_region.first.x;
	int Y1 = node_region.first.y;
	int Z1 = node_region.first.z;
	
	int X2 = node_region.second.x;
	int Y2 = node_region.second.y;
	int Z2 = node_region.second.z;

	int x1 = query_region.first.x;
	int y1 = query_region.first.y;
	int z1 = query_region.first.z;

	int x2 = query_region.second.x;
	int y2 = query_region.second.y;
	int z2 = query_region.second.z;
    return((abs(X1-x1)<=error) && 
           (abs(X2-x2)<=error) &&
           (abs(Y1-y1)<=error) &&
           (abs(Y2-y2)<=error) &&
           (abs(Z1-z1)<=error) &&
           (abs(Z2-z2)<=error)
           );
}

float Query_interface::BitmapQuery(vector<pair<int, int>> Pv, vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy, vector<pair<int, int>> Pdz)
{
	// cout<<"bitmap query:("<<Pdx[0].first<<","<<Pdy[0].first<<","<<Pdz[0].first<<")("<<Pdx[0].second<<","<<Pdy[0].second<<","<<Pdz[0].second<<")\n";	
	// cout<<"bitmap\n";
	clock_t bt = clock();		
	
	total_acccess++;
	bitmap_access++;
	// cout<<bitmap_access<<endl;
    
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
	
	// assert(false);
	//============== dimension based filtering ; lines 9-15 in 2015 paper algo.
	//cout<<"here3"<<Pdx[0].first<<","<<Pdx[0].second<<Pdy[0].first<<","<<Pdy[0].second<<endl;
	vector<size_t> transalted_pd = translate(Pdx, Pdy,Pdz); /// 20% of clock cycles
	// assert(false);
	
	//for(auto w:transalted_pd)
		//cout<<w;
	//cout<<endl;
	
	unordered_map<int,int>* count_array = new unordered_map<int,int>; // reserve!!!!
	// cout<<value_based_filtered_bins.size();

	//// bottleneck here. AND_OP takes most of the cycles!!!!!!!!!!!!!!!! 
	int NUM_THREADS = value_based_filtered_bins.size();
    omp_set_num_threads(NUM_THREADS);	
    #pragma  omp parallel 
	{	
    	size_t actual_num_threads = omp_get_num_threads(); 	
		// cout<<"requested number of threads:"<<NUM_THREADS<<"actual number of threads:" <<actual_num_threads<<endl;	
		////@@@@@@@@@@@@@@@@@@@@@@ 1. get the thread info @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    	int tid; 
		tid = omp_get_thread_num();// query the current thread id	
		int start_bin = tid*value_based_filtered_bins.size()/actual_num_threads;
		int end_bin = (tid+1)*value_based_filtered_bins.size()/actual_num_threads-1;
		if (tid == actual_num_threads-1) // last thread takes care of remaining bins
			end_bin = value_based_filtered_bins.size()-1;
		for(int bin_number = start_bin;bin_number<=end_bin;bin_number++)
		//for(int bin_number:value_based_filtered_bins)//// 80% of clock cycles
		{
			vector<size_t> t = bitmap->get_firstlevelvector(bin_number);
			vector<size_t> result = Bitops.logic_and_ref(t,transalted_pd);
			count_array->insert(pair<int, int> (bin_number,Bitops.count_ones(result)));// there should not be any race condition here since each thread work on non-overlappng bins
		}
	}
	////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float approx_sum = approximate_sum(count_array);
	bt = clock()-bt;
	// cout<<"@@@@@@@@"<<bt<<endl;
	tt+=bt;
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
vector<size_t> Query_interface::translate (vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy,vector<pair<int, int>> Pdz)
{
	boost::dynamic_bitset<> pd(bitmap->get_count());// make an output bitvector of the size of the input data
	// cout<<bitmap->get_count()<<endl;
	for(auto x_pair:Pdx )// for each partial range of x-dimension in the query
	{
		
		int x1 = x_pair.first;
		int x2 = x_pair.second;
		for(auto y_pair: Pdy)//for each partial range of ranges of y-dimension in the query
		{
			
			int y1 = y_pair.first;
			int y2 = y_pair.second;
			for(auto z_pair:Pdz)// for each partial range of z-dimension in the query
			{				
				int z1 = z_pair.first;
				int z2 = z_pair.second;
				for(int i = x1;i<=x2;i++)// sweep the x-axis
					for(int j= y1;j<=y2;j++)// sweep the y-axis
						for(int k= z1;k<=z2;k++)// sweep the z-axis
						{
							pd[i*DimY*DimZ+j*DimZ+k]=1;
							// cout<<i<<" "<<j<<" "<<k<<endl;
						}
				
			}
			
		}
	}
	// Bitops.compressBitset(pd);
	// assert(false);
	
	return Bitops.compressBitset(pd);
}
void Query_interface::print_access_log()
{
	cout<<"================ access report ===================\n";
	cout<<"number of total access to both bitmaps: "<<total_acccess<<endl;
	cout<<"Actual bitmap access ratio: "<<float(bitmap_access)<<endl;
	cout<<"Bitmap tree access ratio: "<<float(tree_access)<<endl;	
}