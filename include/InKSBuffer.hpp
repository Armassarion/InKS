#ifndef INKSBUFFER_HPP
#define INKSBUFFER_HPP

#include <cstring>

namespace InKSloop{
	
	template <int M, int N=1>
	struct right_coordinate{
		template <typename... Args>
		static size_t call(size_t first, Args... args)  {
			return right_coordinate<M, N+1>::call(args...);
		}
	};
	/*Specialization: N = M => fin*/
	template <int M>
	struct right_coordinate<M, M>{
		template <typename... Args>
		static size_t call(size_t first, Args... args)  {
			return first;
		}
		
		static size_t call(size_t first)  {
			return first;
		}
	};
	
	template<int ...> struct seq {};
	template<int N, int ...S> struct gens : gens<N - 1, N - 1, S...> { };
	template<int ...S> struct gens<0, S...>{ typedef seq<S...> type; };

	template<typename T, typename T2, int ...S>
	constexpr double& tuple_to_pack(T& arr, T2& params, seq<S...>){
		return arr(std::get<S>(params) ...);
	}
	
	
	template <bool Periodic, int N, int Dim_entry, int STEP_IN>
	struct InKSBuffer{
		double* data_in;
		double* data_out;
		double* buff_in;
		double* buff_out;
		const size_t shift;
		const size_t nb_buff;
		const size_t size;
		const size_t total_size;

		InKSBuffer(size_t _size, size_t _shift, size_t _nb_block) :
			shift(_shift),
			nb_buff(_nb_block),
			size(_size),
			total_size(size+2*shift)
		{
			buff_in = (double*)malloc(total_size*nb_buff*sizeof(double)); /*shift on right and left side*/
			if(Dim_entry != -1) buff_out = (double*)malloc(total_size*nb_buff*sizeof(double)); /*shift on right and left side*/
			else				buff_out = nullptr;
			set_block(0);
		}
		
		InKSBuffer() = delete;
		InKSBuffer operator=(const InKSBuffer&) = delete;
		InKSBuffer(const InKSBuffer&) = delete;
		
		inline void set_block(int i){
			i %= nb_buff;
			set_block_in(i);
			set_block_out(i);
		}
		inline void set_block_out(int i){
			if(Dim_entry != -1)
				data_out = buff_out + total_size*i + shift;
		}
		inline void set_block_in(int i){
			data_in = buff_in + total_size*i + shift;
		}
		
		inline void copy_left_and_right() const {
			memcpy(data_in-shift, data_in-shift+size, shift*sizeof(double));
			memcpy(data_in+size, data_in, shift*sizeof(double));
		}
	
		~InKSBuffer(){
			if(buff_in  != nullptr) free(buff_in);
			if(buff_out != nullptr) free(buff_out);
			buff_in = nullptr;
			buff_out = nullptr;
			data_in = nullptr;
			data_out = nullptr;
		}
		
		template <typename... Args>
		inline double& operator()(Args... args) const {
			if(Dim_entry != -1){
				if(right_coordinate<Dim_entry>::call(args...) == STEP_IN){
					return data_in[right_coordinate<N>::call(args...)];
				}else{
					return data_out[right_coordinate<N>::call(args...)];
				}
			}else{
				return data_in[right_coordinate<N>::call(args...)];
			}
		}
		
		
		template <typename T, typename... Args>
		void copy_to_buff(T& arr, Args... args){
			auto new_coord = std::make_tuple(args...);
			auto pack = gens<sizeof...(Args)>::type();
			int initial_dim0_value = std::get<0>(new_coord);
			#pragma forceinline recursive
			for(int i = 0; i < size; ++i){
				std::get<N-1>(new_coord) = i;
				std::get<0>(new_coord) = initial_dim0_value;
				
				for(int j=0; j<nb_buff; ++j){
					buff_in[total_size*j + shift + i] = tuple_to_pack(arr, new_coord, pack);
					std::get<0>(new_coord)++;
				}
			}
			 
			if(Periodic){
				for(int j=0; j<nb_buff; ++j){
					set_block_in(j);
					copy_left_and_right();
				}
			}
		}

		template <typename T, typename... Args>
		void copy_from_buff(T& arr, Args... args){
			auto new_coord = std::make_tuple(args...);
			auto pack = gens<sizeof...(Args)>::type();
			int initial_dim0_value = std::get<0>(new_coord);
			#pragma forceinline recursive
			for(int i = 0; i < size; ++i){
				std::get<N-1>(new_coord) = i;
				std::get<0>(new_coord) = initial_dim0_value;
				
				for(int j=0; j<nb_buff; ++j){
					if(Dim_entry != -1)
						tuple_to_pack(arr, new_coord, pack) = buff_out[total_size*j + shift + i];
					else
						tuple_to_pack(arr, new_coord, pack) = buff_in[total_size*j + shift + i];
					std::get<0>(new_coord)++;
				}
			}
		}
	
		/*---------------------------------------------------------Copy to/from buffer >>>NO<<< blocking*/
		template <typename T, typename... Args>
		void copy_to_buff_no_block(T& arr, Args... args){
			auto new_coord = std::make_tuple(args...);
			auto pack = gens<sizeof...(Args)>::type();
			
			for(int i = 0; i < size; i++){
				std::get<N-1>(new_coord) = i;
				data_in[i] = tuple_to_pack(arr, new_coord, pack);
			}
			
			if(Periodic)
				copy_left_and_right();
		}
		
		template <typename T, typename... Args>
		void copy_from_buff_no_block(T& arr, Args... args){
			auto new_coord = std::make_tuple(args...);
			auto pack = gens<sizeof...(Args)>::type();
			
			for(int i = 0; i < size; ++i){
				std::get<N-1>(new_coord) = i;
				if(Dim_entry != -1)
					tuple_to_pack(arr, new_coord, pack) = data_out[i];
				else
					tuple_to_pack(arr, new_coord, pack) = data_in[i];
			}
		}
	};

}
#endif // INKSBUFFER_HPP
