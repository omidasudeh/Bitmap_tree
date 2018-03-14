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
};
class Query_interface {
	private:
		Bitmap<int>* bitmap;	
		int DimX = 0;
		int DimY = 0;	
		int DimZ = 0;	
		Bitmap<int>* bitmapTree;//this should be size_t
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
		vector<size_t> translate (vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy);
		
		/*returns the approximate summation of the query*/
		float approximate_sum(unordered_map<int,int> * count_array);
		
		
		////###################### BITMAP TREE ######################

		vector<size_t> compressed_Bit_representator;
		boost::dynamic_bitset<> Bit_representator;
		////---------------------- methods
		int  TreeQuery(pair<point,point> query_region,int node_number, pair<point, point>  node_region);
		int  bitmap_tree_index(int node_number);
		pair<point,point>*TreeOverlap(pair<point, point> query_region,pair<point, point> node_region);
		///=================================================
		////###################### Reporting ######################
		int total_acccess = 0;
		int tree_access = 0;
		int bitmap_access = 0;
	public:
		Query_interface(int* array,unsigned long items, int DX,int DY, int DZ,int* aggregates, unsigned long aggregate_items, boost::dynamic_bitset<> BR);
		~Query_interface();
		int  Query(pair<point,point> query_region);
		void print_access_log();

};
#endif
//here
