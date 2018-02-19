#ifndef QUERY_INTERFACE
#define QUERY_INTERFACE

#include "Bitmap.h"
using namespace std;
class Query_interface {
	private:
		Bitmap<int>* bitmap;	
		int DimX = 0;
		int DimY = 0;	
		Bitmap<int>* bitmapTree;
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
		float BitmapQuery(vector<pair<int, int>> Pv, vector<pair<int, int>> Pdx,vector<pair<int, int>> Pdy);
		
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
		int  TreeQuery(int x1, int y1, int x2, int y2,int node_number,pair<pair<int,int>, pair<int,int>>* node_region);
		int  bitmap_tree_index(int node_number);
		pair<pair<int,int>, pair<int,int>>*TreeOverlap(int x1, int y1, int x2, int y2,pair<pair<int,int>, pair<int,int>>* node_region);
		///=================================================
	public:
		Query_interface(int* array,unsigned long items, int DX,int DY,int* aggregates, unsigned long aggregate_items, boost::dynamic_bitset<> BR);
		~Query_interface();
		int  Query(int x1, int y1, int x2, int y2);

};
#endif
