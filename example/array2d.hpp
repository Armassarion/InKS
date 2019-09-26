#pragma once

class Array2D
{
	int m_dim;
	
	double* m_array;
	
public:
	Array2D ( int dim ):
			m_dim(dim),
			m_array(new double[dim * 2])
	{}
	
	~Array2D () { delete[] m_array; }
	
	double& operator() ( int x, int t ) { return m_array[x + (t%2) * m_dim]; }
	
	const double& operator() ( int x, int t ) const { return m_array[x + (t%2) * m_dim]; }
	
	double* get_data(){
		return m_array;
	}
};
