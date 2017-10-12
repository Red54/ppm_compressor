#include "arithmetic_compressor.h"

ArithmeticCompressor::ArithmeticCompressor(Model* model){
	this->model = model;
}

void ArithmeticCompressor::encode(SymbolBuffer& input, BitBuffer& output){

	uint low = 0x00000000U;
	uint high = 0xFFFFFFFFU;

	uint range;
	ulong aux;
	int pending_bits = 0;
	Symbol symbol;
	Context context;
	ProbabilitiesSet prob;

	while( !input.eof() ){

		input >> symbol;
		prob = model->getSymbolProbability(context, symbol);
		int siz = prob.size();
		for(ProbabilityRange p : prob){

			std::cout << "\n" << std::endl;
			if(--siz == 0)
				std::cout << (char)symbol << " ";
			else
				std::cout << "ESC ";
			std::cout << "["  << p.low_num << "/" << p.den << ", " << p.high_num << "/" << p.den << ")" << std::endl;
			std::cout << "----------------------------------------" << std::endl;

			range = high - low;

			aux = (ulong)range * (ulong)p.high_num / (ulong)p.den;
			high = low + (uint)( aux );
			
			aux = (ulong)range * (ulong)p.low_num / (ulong)p.den;
			low =  low + (uint)( aux );

			std::cout << "low:  ";
			//output.print();
			std::cout << std::bitset<32>(low) << "\nhigh: ";
			//output.print();
			std::cout << std::bitset<32>(high) << "\n";

			while(true){
				if(high < 0x80000000U){
					output << 0;
					while ( pending_bits ){
						pending_bits--;
						output << 1;
					} 
					high <<= 1;
					low <<= 1;
					high |= 0x00000001U;
				}
				else if(low >= 0x80000000U){
					output << 1;
					while ( pending_bits ){
						output << 0;
						pending_bits--;
					}
					high <<= 1;
					low <<= 1;
					high |= 0x00000001U;
				}
				else if ( low >= 0x40000000U && high < 0xC0000000U ){
					pending_bits++;
					low <<= 1;
					low &= 0x7FFFFFFEU;
					high <<= 1;
					high |= 0x80000001U;
				}
				else{
					break;
				}
			}
			output.print();
			std::cout << std::endl;
		}
		model->updateModel(context, symbol);
		context.push_back(symbol);
		if(context.size() > model->getK()) context.pop_front();
	}
	output << 1;
}

void ArithmeticCompressor::decode(BitBuffer& input, SymbolBuffer& output, int size){

	model->clearModel();

	uint low = 0x00000000U;
	uint high = 0xFFFFFFFFU;
	uint range;
	uint value;
	Bit bit;
	bool minus_1_flag = false;
	uint count;
	uint aux_count;
	ulong aux;
	Symbol symbol;
	Context context;
	Context aux_context;
	ProbabilitiesSet prob;

	uint i = 32;
	for (; i > 0 ; i-- ) {
		if(input.eof()) break;
		input >> bit;
		value <<= 1;
		value += bit;
	}
	value <<= i;

	while(true) {
		aux_context = context;
		minus_1_flag = false;

		while(true){
			aux_count = model->getCount(aux_context);

			std::cerr << "aux_count = " << aux_count << std::endl;
			if( aux_count ) break;
			if( aux_context.empty() ){
				minus_1_flag = true;
				break;
			}
			aux_context.pop_front();
		}

		while(true){

			range = high - low;

			// std::cerr << "high: " << std::bitset<32>(high) << std::endl;
			// std::cerr << "low:  " << std::bitset<32>(low) << std::endl;
			// std::cerr << "rang: " << std::bitset<32>(range) << std::endl;
			// std::cerr << "valu: " << std::bitset<32>(value) << std::endl;
			// std::cerr << "v-l:  " << std::bitset<64>((ulong)(value - low)) << std::endl;
			// std::cerr << "ax_c: " << std::bitset<64>((ulong)aux_count) << std::endl;
			// std::cerr << "vla:  " << std::bitset<64>((ulong)(value - low) * (ulong)aux_count) << std::endl;
			// std::cerr << "rang: " << std::bitset<64>((ulong)range) << std::endl;
			// std::cerr << "vlar: " << std::bitset<64>((ulong)(value - low) * (ulong)aux_count / (ulong)range) << std::endl;
			// std::cerr << "------------------------------" << std::endl;

			if( !minus_1_flag ){
				count = (ulong)(value - low) * (ulong)aux_count / (ulong)range;
				symbol = model->getSymbol(aux_context, count);
				std::cerr << "count = " << count << std::endl;
				std::cerr << "model->getSymbol(aux_context, count) = " << symbol << std::endl;
				prob = model->getSymbolProbability(aux_context, symbol);
				std::cerr << "["  << prob[0].low_num << "/" << prob[0].den << ", " << prob[0].high_num << "/" << prob[0].den << ")" << std::endl;
				std::cerr << "----------------------------------------" << std::endl << std::endl;
			}
			else{
				std::cerr << "low:  " << std::bitset<32>(low) << std::endl;
				std::cerr << "high: " << std::bitset<32>(high) << std::endl;
				std::cerr << "rang: " << std::bitset<32>(range) << std::endl;
				std::cerr << "valu: " << std::bitset<32>(value) << std::endl;
				std::cerr << "v-l:  " << std::bitset<64>((ulong)(value - low)) << std::endl;
				std::cerr << "ax_c: " << std::bitset<64>((ulong)model->getCount(-1)) << std::endl;
				std::cerr << "vla:  " << std::bitset<64>((ulong)(value - low) * (ulong)model->getCount(-1)) << std::endl;
				std::cerr << "rang: " << std::bitset<64>((ulong)range) << std::endl;
				std::cerr << "vlar: " << std::bitset<64>((ulong)(value - low) * (ulong)model->getCount(-1) / (ulong)range) << std::endl;
				std::cerr << "------------------------------" << std::endl;

				count = (ulong)(value - low) * (ulong)model->getCount(-1) / (ulong)range;
				std::cerr << "model->getCount(-1) = " << model->getCount(-1) << std::endl;
				symbol = model->getSymbol(-1, count);
				std::cerr << "count = " << count << std::endl;
				std::cerr << "model->getSymbol(-1, count) = " << symbol << std::endl;
				prob = model->getSymbolProbability(-1, symbol);
				std::cerr << "["  << prob[0].low_num << "/" << prob[0].den << ", " << prob[0].high_num << "/" << prob[0].den << ")" << std::endl;
				std::cerr << "----------------------------------------" << std::endl << std::endl;
				minus_1_flag = false;
			}

			auto p = prob[0];
			aux = (ulong)range * (ulong)p.high_num / (ulong)p.den;
			high =  low + (uint)( aux );

			aux = (ulong)range * (ulong)p.low_num / (ulong)p.den;
			low =  low + (uint)( aux );
			

			std::cerr << "low:  " << std::bitset<32>(low) << std::endl;
			std::cerr << "high: " << std::bitset<32>(high) << std::endl;
			std::cerr << "----------------------------------------" << std::endl << std::endl;

			while(true) {
				if ( low >= 0x80000000U || high < 0x80000000U ) {
					low <<= 1;
					high <<= 1;
					high |= 1;
					value <<= 1;

					if(!input.eof()){
						input >> bit;
						value += bit;
					}
					std::cerr << "A";
				} 
				else if ( low >= 0x40000000U && high < 0xC0000000U ) {
					low <<= 1;
					low &= 0x7FFFFFFFU;
					high <<= 1;
					high |= 0x80000001U;
					value <<= 1;

					if(!input.eof()){
						input >> bit;
						value += bit;
					}
					std::cerr << "B";
				}
				else{
					break;
				}
			}

			if(symbol > 255){
				if(!aux_context.empty())
					aux_context.pop_front();
				else
					minus_1_flag = true;
			}
			else{
				break;
			}
		}
		
		output << symbol;
		if(--size == 0) break;
		model->updateModel(context, symbol);
		context.push_back(symbol);
		if(context.size() > model->getK()) context.pop_front();
	}
}