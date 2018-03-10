#include "Bitmap.h"
#include <vector>
#include <cmath>

using namespace std;
#define DebugMode false// generate more text
#define TRACE false // to debug the node number to child bitmap tree index
#define SET_PREC true // set_precision()
/*
 * the constructor function
 */
template <class a_type> 
Bitmap<a_type>::Bitmap(a_type* array, unsigned long items){
	itemsCount = 0;// initially set the number of the items to 0
	firstlevelvalue = new vector<struct index_bin>();// a vector of the the min and max values of the first level bins
	varvalmap = new map<int,int>();
	second_level_sums = new unordered_map<int,int>();
	first_level_sums = new unordered_map<int,int>();
	firstlevelvectors = new vector<vector<size_t>>();// high-level indices
	secondlevelvectors = new vector<vector<size_t>>();//low-level indecies 
	//####################### chunked bitmap ###########
	second_level_bitmap = new vector<vector<vector<size_t>>>();
	first_level_bitmap = new vector<vector<vector<size_t>>>();
	//##################################################
	//======================== define a bitops object ==================
	//Bitops = new bitops();
	
	//fetch the data block and initialize vector size
	a_type * data_in = array;
	//====== calculate the minimum and the maximum of the data (using a linear search)
	minvalue = data_in[0];
	maxvalue = data_in[0];
	for(int i=0; i<items; i++) {
		//if(data_in[i] < NOISE_VALUE_HIGH &&
		//  (maxvalue >= NOISE_VALUE_HIGH ||  data_in[i] > maxvalue))
		if(data_in[i] > maxvalue)
			maxvalue = data_in[i];
		//if(data_in[i] > NOISE_VALUE_LOW &&
		//  (minvalue <= NOISE_VALUE_LOW ||data_in[i] < minvalue))
		if(data_in[i] < minvalue)
			minvalue = data_in[i];
	}
	//set the current precision(total number of low-level bins) based on value ranges
	setPrecision();
	
	//Generate the dintinct value group for each variable
	
	int count = 0;	
	for(int i=0; i<items; i++) {
	//if(data_in[i] > NOISE_VALUE_LOW && data_in[i] < NOISE_VALUE_HIGH) {
		int temp = (int)round((data_in[i] * numpres));
		varvalmap->insert(pair<int, int>(temp, -1));
	//	}
	}
	for(map<int,int>::iterator it=varvalmap->begin(); it!=varvalmap->end(); it++) 
		(*it).second = count++;

	vector<vector<int> > bitvectors(varvalmap->size());
	for(int i=0; i<items; i++) {
		//if(data_in[i] > NOISE_VALUE_LOW && data_in[i] < NOISE_VALUE_HIGH) {
		  int temp = (int)round((data_in[i] * numpres));
		  bitvectors[varvalmap->find(temp)->second].push_back(i);
		//}
	}

	//============== calculate second_level_sums
	for(auto elem: *varvalmap) {
		int scaled_data = elem.first;
		int bin_number = elem.second;
		int data = scaled_data/numpres;
		int bitvector_size = bitvectors[bin_number].size();
		second_level_sums->insert(pair<int, int>(bin_number,data*bitvector_size));
	}
//////////////////////Generate the second-level(low-level) bitvectors/////////////////////
	
	for(int i=0; i<varvalmap->size(); i++) {
		boost::dynamic_bitset<> tempvector(items);
		for(int j=0; j<bitvectors[i].size(); j++) {
		  tempvector[bitvectors[i][j]] = 1;
		}
		
		boost::dynamic_bitset<> chunk(chunk_size);
		vector<vector<size_t>> compressed_bitvector;
		for(int k = 0 ; k<items;k++)
		{
			//cout<<"chSz"<<chunk_size<<"items"<<items<<endl;
			chunk[k%chunk_size] = tempvector[k];
			if((k+1)%chunk_size == 0) // if the chunk is complete
			{ // then compress it and add it to the compressed_bitvector
				compressed_bitvector.push_back(Bitops.compressBitset(chunk));
			}
		}
		//cout<<compressed_bitvector.size()<<"tttttt\n";
		second_level_bitmap->push_back(compressed_bitvector);
		
		
		secondlevelvectors->push_back(Bitops.compressBitset(tempvector));
	}
	/*int i=0;
	for(auto bitvector:(*second_level_bitmap))
	{
		cout<<"bitvector "<<i<<endl;
		int j=0;
		for(auto chunk : bitvector)
		{
			cout<<"chunk"<<j<<":"<<chunk.size()<<" ";
			j++;
		}
		cout<<endl;
		i++;
	}*/
	
////////////////////////Generate the first-level(high-level) bitvectors////////////////////
	map<int,int>::iterator curit=varvalmap->begin();
	map<int,int>::iterator preit;
	int k = 0;

	for(int i=0; i<varvalmap->size(); ) {
		struct index_bin curbin;
		curbin.min_val = (*curit).first;
		preit = curit++;
		int sum = (*second_level_sums)[i];//===
		//=========================
		vector<vector<size_t>> first_level_temp = (*second_level_bitmap)[i];///// be carefule assignment!!! 
		//cout<<"first_level_temp.size "<<first_level_temp.size()<<endl;
		//=========================
		vector<size_t> firstlevelidx = (*secondlevelvectors)[i++];
		
		for(int j=1; j<BIN_INTERVAL && i<varvalmap->size(); j++) {
		  sum+=(*second_level_sums)[i];
		  firstlevelidx = Bitops.logic_or(firstlevelidx, (*secondlevelvectors)[i++]);
		  preit = curit++;
		  //==================
		  for(int t = 0; t<first_level_temp.size();t++)
		  {
			  //cout<<first_level_temp[t].size()<<"  "<<(*second_level_bitmap)[i][t].size()<<endl;
			  first_level_temp[t]= Bitops.logic_or(first_level_temp[t], (*second_level_bitmap)[i][t]);
		  }
		  //==================
		}
		curbin.max_val = (*preit).first;
		firstlevelvalue->push_back(curbin);
		
		
		//=======================
		first_level_bitmap->push_back(first_level_temp);
		//=======================
		
		firstlevelvectors->push_back(firstlevelidx);
		first_level_sums->insert(pair<int, int>(k,sum));//===
		k++;
	}
	/*i=0;
	for(auto bitvector:(*first_level_bitmap))
	{
		cout<<"bitvector "<<i<<endl;
		int j=0;
		for(auto chunk : bitvector)
		{
			cout<<"chunk"<<j<<":"<<chunk.size()<<" ";
			j++;
		}
		cout<<endl;
		i++;
	}*/
	itemsCount = items;
	cout<<"Bitmap created"<<endl;
	cout<<"Item Counts:"<<itemsCount<<endl;
	cout<<"number of low-level bins:"<<(*secondlevelvectors).size()<<endl;
	cout<<"number of high-level bins:"<<(*firstlevelvectors).size()<<endl;
	cout<<"======================\n";
}
/*
 * The destructor function
 */
template <class a_type>
Bitmap<a_type>::~Bitmap() {
	// TODO Auto-generated destructor stub
	delete firstlevelvalue;
	delete varvalmap;
	delete firstlevelvectors;
	delete secondlevelvectors;
}

template <class a_type> 
/*
 * Set precision(number of bins) of current indexing based on value ranges
 * In current solution, we control the total number of bins between [100, 999].
 * Users can modify this code to generate flexible bin numbers
 * In the future, we plan to provide an interface for users to specify the bin number
 */ 
void Bitmap<a_type>::setPrecision() {
  
  //data type issue
  
  if(SET_PREC)
  {
	  a_type diff = maxvalue - minvalue;
	  if(diff > 1000000)
		this->numpres = 0.0001;
	  else if(diff > 100000)
		this->numpres = 0.001;
	  else if(diff > 10000)
		this->numpres = 0.01;
	  else if(diff > 1000)
		this->numpres = 0.1;
	  else if(diff > 100)
		this->numpres = 1;
	  else if(diff > 1)
		this->numpres = 10;
	  else if(diff > 0.1)
		this->numpres = 100;
	  else if(diff > 0.01)
	   this->numpres = 1000;
	  else
	   this->numpres = 10000;
   }
   else
	this->numpres = 1;
}

template <class a_type>
void Bitmap<a_type>:: calcPreAgg()
{
	int i = 0;
	for(auto vec:*secondlevelvectors )
	{
		stat s;
		boost::dynamic_bitset<> uncompressed_bitvector = Bitops.uncompressIndex(vec,itemsCount);
		s.count = uncompressed_bitvector.count();
		s.sum = (*second_level_sums)[i];
		second_level_statistics.push_back(s);
		i++;
		//cout<<s.count<<"vv	";
		
	}
	i = 0;
	for(auto vec:*firstlevelvectors )
	{
		stat s;
		boost::dynamic_bitset<> uncompressed_bitvector = Bitops.uncompressIndex(vec,itemsCount);
		s.count = uncompressed_bitvector.count();
		s.sum = (*first_level_sums)[i];
		first_level_statistics.push_back(s);
		i++;
		//cout<<s.count<<"gg	";
		
	}
}


////########################### gets ######################################
template <class a_type>
float Bitmap<a_type>:: get_first_level_sum(int binNumber)
{
	//cout<<binNumber<<"  "<<first_level_statistics.size();
	//assert(false);
	return first_level_statistics[binNumber].sum;
	//return (pair<float,float>)((*first_level_statistics)[binNumber].sum,(*first_level_statistics)[binNumber].count);
}
template <class a_type>
float Bitmap<a_type>:: get_first_level_count(int binNumber)
{
	return first_level_statistics[binNumber].count;
	//return (pair<float,float>)((*first_level_statistics)[binNumber].sum,(*first_level_statistics)[binNumber].count);
}
template <class a_type>
float Bitmap<a_type>:: get_second_level_sum(int binNumber)
{
	return second_level_statistics[binNumber].sum;
	//return (pair<float,float>)((*first_level_statistics)[binNumber].sum,(*first_level_statistics)[binNumber].count);
}
template <class a_type>
float Bitmap<a_type>:: get_second_level_count(int binNumber)
{
	return second_level_statistics[binNumber].count;
	//return (pair<float,float>)((*first_level_statistics)[binNumber].sum,(*first_level_statistics)[binNumber].count);
}

template <class a_type>
float  Bitmap<a_type>:: get_numpres()
{
	return numpres;
}
/*
 * returns a pointer to a specific bin in the bitmap*/
template <class a_type>
vector<size_t> Bitmap<a_type>::get_firstlevelvector(int binNumber)
{
	return firstlevelvectors->at(binNumber);
}
template <class a_type>
vector<struct index_bin>*Bitmap<a_type>:: get_firstlevelvalue()
{
	return firstlevelvalue;
}
/* returns the value of the node 
	input: node_number
	output: the bin <min,max> values as a pair<int,int>; returns NULL if the value is not in none of the bins
*/
template <class a_type>
pair<int,int> * Bitmap<a_type>:: get_value(int node_number)
{
	pair<int,int>* result = NULL; 
	// 
	int chunk_number = node_number/chunk_size;
	int chunk_index = node_number%chunk_size;
	
	
	////naive  setbit compression
	boost::dynamic_bitset<>bit_chunk_index(chunk_size);
	bit_chunk_index.set(chunk_index);
	
	////////////////////////////// Bottle neck if chunk size is high !!!!!!!!!!!!!!!!!!!!!!!!!!!!
	vector<size_t>comp_chunk_index = Bitops.compressBitset(bit_chunk_index); 

	
	//for(auto w:comp_chunk_index)
		//cout<<hex<<w<<"	";
	//cout<<endl<<endl<<dec;	
	
	/*
    ////1. fast setbit compression=====================	
	//// make a bitvector with the chuck_index bit set and then compress it to comp_chunk_index
	
		 vector<size_t> comp_chunk_index;	 
		 int HZero = chunk_index - chunk_index%31;
		 if(HZero>0)
		 {
			size_t HZeroW = 0x80000000 + HZero;
			comp_chunk_index.push_back(HZeroW);
		 }
		 int OneW=0;
		 if(chunk_index<31)
			 OneW = 1<<chunk_index-(node_number-HZero)-1;
		 else
			 OneW = 1<<31-(node_number-HZero)-1;
		 comp_chunk_index.push_back(OneW);
		 
		 int LZero = chunk_index-HZero-31;
		 if(LZero>0)
		 {
			size_t LZeroW = 0x80000000 + LZero;
			comp_chunk_index.push_back(LZeroW);
		 }
	*/ 
	
	////2. check bitvectors ============================
	//// iterates over the bitvectors in the bitmap 
	//// and returns the bitvector statistics if the 
	//// chuck_index bit is set 
	
	
	int number_of_bins = (*first_level_bitmap).size();
	for(int bitvector_index = number_of_bins-1;bitvector_index>=0;bitvector_index--)
	{ 
		//clock_t tq = clock();
		//bool t = isInBitvector(bitvector_index,chunk_number, comp_chunk_index);
		//gtt += clock()-tq;
		if(isInBin(bitvector_index,chunk_number, comp_chunk_index))
		{
			////== retun bin.stat ==
			result = new pair<int,int>;
			result->first = (*firstlevelvalue)[bitvector_index].min_val/numpres;
			result->second= (*firstlevelvalue)[bitvector_index].max_val/numpres;
			
			return result;
			
		}
	}
	if (result == NULL) 
	{
			cout<<"Error: the node value was not found in the bitmap!\n";
			assert(false);
	}
	return result; 
}
template <class a_type>
bool Bitmap<a_type>:: isInBin(int bitvector_index, int chunk_number, vector<size_t>& comp_chunk_index)
{	
 ////  logic_and the compressed bin with the comp_chunk_index and check if result.any()
		
		////1.======== logic_and
		//cout<<comp_chunk_index.size()<<"&"<<(*first_level_bitmap)[bitvector_index][chunk_number].size()<<endl;		
		////////////////////////////// Bottle neck!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//clock_t tq = clock();
		//k0++;
		vector<size_t> comp_and_result = Bitops.logic_and_ref(comp_chunk_index,(*first_level_bitmap)[bitvector_index][chunk_number]); // do assignment by reference
		//gtt += clock()-tq;
		
		////2.======== result.any()
		for(size_t word : comp_and_result) //if(compressed_and_result.any())
		{
			int WType = Bitops.word_type(word);
			if( WType == 0 || WType == 2)// if the word is literal or ones fill word 
				return true;
				
		}
		return false;
}

/*
 * Find the cardinality of high-level bitmap indices (Omid: or number of high-level bins)
 */
template <class a_type> 
unsigned long Bitmap<a_type>::getL1Size() {
  return firstlevelvectors->size();
}

/*
 * Find the cardinality of low-level bitmap indices(Omid: or number of low-level bins)
 */
template <class a_type>
unsigned long Bitmap<a_type>::getL2Size() {
  return secondlevelvectors->size();
}

template <class a_type>
int Bitmap<a_type>::get_max()
{return maxvalue;}

template <class a_type>
int Bitmap<a_type>::get_min()
{return minvalue;}

template <class a_type>
long Bitmap<a_type>::get_count()
{return itemsCount;}
////########################### prints ###################################
template <class a_type>
void Bitmap<a_type>::print_min_max()
{
	cout<<"========================"<<endl;
	int i=0;
	for(auto elem: *firstlevelvalue)
	{
		cout<<"bin#:"<<i<<" min: "<<elem.min_val/numpres<<"	max:"<<elem.max_val/numpres<<endl;
		i++;
	}
	cout<<endl;
}

template <class a_type>
void Bitmap<a_type>:: print_first_level_uncompressed_vectors()
{
	int i=0;
	for(auto vec:*firstlevelvectors )
	{
		for(auto w: vec)
		{
			boost::dynamic_bitset<> x (32,w);
			cout<<x<<" ";
		}
		cout<<dec<<"<=== b"<<i<<endl;
		i++;
	}		
}

template <class a_type>
void Bitmap<a_type>:: print_varvalmap()
{
	for(auto elem: *varvalmap)
		cout<<"key:"<<elem.first<<", val: "<<elem.second<<endl;
}
	
template <class a_type>
void Bitmap<a_type>:: print_second_level_uncompressed_vectors()
{
	int i=0;
	//cout<<(*secondlevelvectors).size()<<endl;
	for(auto vec:*secondlevelvectors )
	{
		for(auto w: vec)
		{
			boost::dynamic_bitset<> x (32,w);
			cout<<x<<" ";
		}
		cout<<dec<<"<=== b"<<i<<endl;
		i++;
	}
	
}

template <class a_type>
void Bitmap<a_type>:: print_stat()
{
	
	cout<<"===========second level stats============="<<endl;
	int i = 0;
	for(auto s:second_level_statistics)
	{
		cout<<"bin#: "<<i<<" count: "<<s.count<<" sum: "<<s.sum<<endl;
		i++;
	}
	cout<<"===========first level stats============="<<endl;
	i = 0;
	//cout<<"common!"<<first_level_statistics[0].min;
	for(auto s:first_level_statistics)
	{
		cout<<"bin#: "<<i<<" count: "<<s.count<<" sum: "<<s.sum<<endl;
		i++;
	}
}
template class Bitmap<double>;
template class Bitmap<float>;
template class Bitmap<int>;
template class Bitmap<long>;
