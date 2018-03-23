#ifndef QUERY_INTERFACE
#define QUERY_INTERFACE

#include "Bitmap.h"
using namespace std;
struct point
{
	int x = 0;
	int y = 0;
	int z = 0;
	bool operator ==(point p)const{
		return (x == p.x) && (y == p.y) && (z == p.z);
	}
	bool operator <=(point p)const{
		return (x <= p.x) && (y <= p.y) && (z <= p.z);
	}
	void print()
	{
		cout<<"("<<x<<","<<y<<","<<z<<")\n";
	}
};
class Query_interface {
	private:
		Bitmap<size_t>* bitmap;	
		int DimX = 0;
		int DimY = 0;	
		int DimZ = 0;	
		Bitmap<size_t>* bitmapTree;//this should be size_t
		mybitops Bitops;////adds bit operations toolbox
		///###################### ACTUAL BITMAP ####################
		/*
		 *input: 
		 *	Pv: value-based predicate
		 *	Pd: dimension-based predicate
		 *	DimY: the length of the Y dimension
		 *output:
		 *	prints the query result
		 */
		float BitmapQuery(vector<pair<int, int>> Pv, vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy, vector<pair<int, int>> Pdz);
		
		/*returns true if a bin b is fully selected by the value-based predicate Pv*/
		bool rangeOverlap(pair<int, int> b, vector<pair<int,int>> Pv);
		
		/*translates the dimension based query into a bitvector predicate*/
		vector<size_t> translate (vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy,vector<pair<int, int>> Pdz);
		
		/*returns the approximate summation of the query*/
		float approximate_sum(unordered_map<int,int> * count_array);
		
		
		////###################### BITMAP TREE ######################

		vector<size_t> compressed_Bit_representator;
		boost::dynamic_bitset<> Bit_representator;
		int error = 0;
		////---------------------- methods
		int  TreeQuery(pair<point,point> query_region,int node_number, pair<point, point>  node_region);
		int  bitmap_tree_index(int node_number);
		pair<point,point>*TreeOverlap(pair<point, point> query_region,pair<point, point> node_region);
		bool match(pair<point, point> query_region,pair<point, point> node_region, int error);
		
		///=================================================
		////###################### Reporting ######################
		int total_acccess = 0;
		int tree_access = 0;
		int bitmap_access = 0;
		////###################### debugging ######################
		clock_t tt = 0; 		
	public:
		Query_interface( int DX,int DY, int DZ, int error);
		Query_interface(string dir,size_t* array,size_t items, int DX,int DY, int DZ,size_t* aggregates, size_t aggregate_items, boost::dynamic_bitset<> BR, int error);//generates bitmaps
		~Query_interface();
		int  Query(pair<point,point> query_region);
		void print_access_log();

};
#endif
//here
