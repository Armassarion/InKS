/*
public kernel inks_heat(DIM_X, DIM_T):(
  double Heat(2) {out: i=[0:DIM_X[ (i, DIM_T-1)}
)
#CODE (inks)
  inner ix=[1:DIM_X-1[ it=[1:DIM_T[ (ix, it) : (Heat),
  bord  it=[1:DIM_T[ { (0, it); (DIM_X-1, it) } : (Heat),
  init (DIM_X) : (Heat)
#END
*/

inks_heat(
	give double H(2, DIM_X) 
		{out: 
			 x=[0:DIM_X[ H(DIM_T-1, x) = Heat(x, DIM_T-1)},
	DIM_X, 
	DIM_T
)
{
	alloc double H(2, DIM_X)
	
	map x1=[0:DIM_X[ H(0, x1) = Heat(x1, 0)
	update Heat(0:DIM_X, 0) using init
	
	update Heat(0:DIM_X, DIM_T-1) using for(t{1:DIM_T})
	{
		map x=[0:DIM_X[ H(t mod 2, x) = Heat(x, t)
		
		update Heat(0:DIM_X, t) using for({OMP_TASKLOOP}x{0:DIM_X})
		{
			update Heat(x, t) using inner, bord
		}
		
	}
	
}

/*
void* f() 		 give
f(void *const p) keep
f(void*& p)      take
*/
