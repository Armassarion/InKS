#ifndef INKS_INKSARRAY_HPP__
#define INKS_INKSARRAY_HPP__

#include <type_traits>
#include <algorithm>
#include <numeric>
#include <memory>
#include <array>
#include <tuple>
		
/*Memory: Logical to Physical*/
template <uint C, uint... Cs>
class LogToPhy{
	
	template <typename T1 = void, typename T2 = void>
	static constexpr uint Convert(){
		return 0;
	}

	template <uint Coeff, uint... Coeffs, typename... Args>
	static constexpr uint Convert(const uint x, const Args... args){
		return x * Coeff + Convert<Coeffs...>(args...);
	}
	
	public:
	
	template <typename... Args>
	static constexpr uint L2P(const Args... args){
		return Convert<C, Cs...>(args...);
	}
};

template <typename T, int N, bool Modulo, bool... Modulos>
struct for_each_tuple{
	template <typename... Args>
	static constexpr size_t call(const std::array<const uint, sizeof...(Args)>& dim, const std::array<const size_t, sizeof...(Args)>& mult, const Args... args){
		return	((size_t)( Modulo ?	((std::tuple_element<N, T>::type::L2P(args...))%dim[N])
									:
									(std::tuple_element<N, T>::type::L2P(args...))
				)) * mult[N] + for_each_tuple<T, N+1, Modulos...>::call(dim, mult, args...);
	}
};



template <typename T, int N, bool Modulo>
struct for_each_tuple<T, N, Modulo>{
	template <typename... Args>
	static constexpr size_t call(const std::array<const uint, sizeof...(Args)>& dim, const std::array<const size_t, sizeof...(Args)>& mult, const Args... args){
		return	((size_t)( Modulo ?	((std::tuple_element<N, T>::type::L2P(args...))%dim[N])
									:
									(std::tuple_element<N, T>::type::L2P(args...))
				)) * mult[N];
	}
};

template <typename T, bool Modulo>
struct for_each_tuple<T, 0, Modulo>{
	template <typename... Args>
	static constexpr size_t call(const std::array<const uint, sizeof...(Args)>& dim, const std::array<const size_t, sizeof...(Args)>& mult, const Args... args){
		return Modulo ? (((std::tuple_element<0, T>::type::L2P(args...))%dim[0]))
						:
						(((std::tuple_element<0, T>::type::L2P(args...))));
	}
};

template <typename T, bool Modulo, bool... Modulos>
struct for_each_tuple<T, 0, Modulo, Modulos...>{
	template <typename... Args>
	static constexpr size_t call(const std::array<const uint, sizeof...(Args)>& dim, const std::array<const size_t, sizeof...(Args)>& mult, const Args... args){
		return	((size_t)( Modulo ?	((std::tuple_element<0, T>::type::L2P(args...))%dim[0])
									: 
									(std::tuple_element<0, T>::type::L2P(args...))
				)) + for_each_tuple< T, 1, Modulos...>::call(dim, mult, args...);
	}
};
 
template <typename T, typename Tuple_type, bool... Modulos>
class InKSArray{
	const std::array<const size_t, sizeof...(Modulos)> mult;
	const std::array<const uint, sizeof...(Modulos)> dim;
	std::unique_ptr<T[]> data;
	
	public:
	
	template<typename... Args>
	InKSArray(std::array<const size_t, sizeof...(Modulos)>&& _mult, Args&&... args) :
		mult(_mult),
		dim{{args...}},
		data(new T[accumulate(dim.begin(), dim.end(), (size_t)1, std::multiplies<size_t>())])
	{}
	
	template<typename... Args>
	T& operator()(const Args... args) const{
		return data[for_each_tuple<Tuple_type, 0, Modulos...>::call(dim, mult, args...)];
	}

};

#endif
