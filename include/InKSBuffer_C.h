#ifndef INKSBUFFER_HPP
#define INKSBUFFER_HPP

typedef struct{
	const int Periodic;
	const int N;
	const int Dim_entry;
	const int STEP_IN;
	const size_t size;
	const size_t shift;
	const size_t nb_buff;
	const size_t total_size;
	double* data_in;
	double* data_out;
	double* buff_in;
	double* buff_out;	
}InKSBuffer_4_8_C;

void initialize_inksbuffer_c(InKSBuffer_4_8_C* buff){
						buff_in = 	malloc(total_size*nb_buff*sizeof(double)); /*shift on right and left side*/
	if(Dim_entry != -1) buff_out = 	malloc(total_size*nb_buff*sizeof(double)); /*shift on right and left side*/
	else				buff_out = 	nullptr;
	set_block(buff, 0);
};

inline void set_block(InKSBuffer_4_8_C* buff, int i){
	i %= buff->nb_buff;
	set_block_in(buff, i);
	set_block_out(buff, i);
}
inline void set_block_out(InKSBuffer_4_8_C* buff, int i){
	if(buff->Dim_entry != -1)
		buff->data_out = buff->buff_out + buff->total_size*i + buff->shift;
}
inline void set_block_in(InKSBuffer_4_8_C* buff, int i){
	buff->data_in = buff->buff_in + buff->total_size*i + buff->shift;
}

inline void copy_left_and_right(InKSBuffer_4_8_C* buff) const {
	memcpy(buff->data_in-buff->shift, buff->data_in-buff->shift+buff->size, buff->shift*sizeof(double));
	memcpy(buff->data_in+buff->size, buff->data_in, buff->shift*sizeof(double));
}

double* adv4_loop_buffer_access_out(InKSBuffer_4_8_C* buff, int i, int j, int k, int l, int m, int n, int t, int step){
	return &(buff.data_out[l]);
}

double* adv4_loop_buffer_access_in(InKSBuffer_4_8_C* buff, int i, int j, int k, int l, int m, int n, int t, int step){
	return &(buff.data_in[l]);
}


#endif // INKSBUFFER_HPP
