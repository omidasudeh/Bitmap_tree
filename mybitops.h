#ifndef MYBITOPS_H
#define MYBITOPS_H
//#include <vector>
using namespace std;
class mybitops{
	public:
	mybitops(){}
	~mybitops(){}
	////###################### Bit Operation ###################################
	/*
	 *Perform logic AND operation directly based on compressed bitvectors 
	 */
	vector<size_t> logic_and(vector<size_t> vector1, vector<size_t> vector2);
	/*
	 *Perform logic AND operation directly based on compressed bitvectors 
	 */
	vector<size_t> logic_and_ref(vector<size_t> &vector1, vector<size_t> &vector2);

	/*
	 * Perform logic OR operation directly based on compressed bitvectors
	 */ 
	vector<size_t> logic_or(vector<size_t> vector1, vector<size_t> vector2);
	/*
	 * Use WAH compression algorithm to compress one bitvector
	 */ 
	vector<size_t> compressBitset(const boost::dynamic_bitset<> dbitset);
	/*
	 * Use WAH decompression algorithm to decompress all the bitvector
	 */ 
	boost::dynamic_bitset<> uncompressIndex(vector<size_t> cvector, size_t uncompressed_size);
	/*
	 * Counts the number of setbits upto a certain position
	 */
	 int CountOnesUpto(vector<size_t> &compresed_vector, int position);
	 /*
	 * Returns the word type:
	 * 0: literal 
	 * 1: zeros
	 * 2: ones
	 */
	 /*
	 * Part of the WAH algorithm
	 * Check if the current word is a fill word or a literal word
	 */ 
	int count_ones(vector<size_t> &vec);
	int ismyfill(size_t word1);
	 /*
	  * return the word type
	  * 0:
	  * 1:
	  * 2:
	  */
	 int word_type(size_t word1);
	/*
	 * just for the purpose of testing the CountOnesUpto function
	 */
	void test_count();
	
	
	
	};
#endif
