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
	dom xdom=[0:DIM_X[
	
	alloc double H(2, DIM_X)
	map x=[0:DIM_X[ H(0, x) = Heat(x, 0)
	
	update Heat(xdom, 0) using init
	
	update Heat(0:DIM_X, DIM_T-1) using for(t{1:DIM_T})
	{
		map H(t mod 2, xdom) = Heat(xdom, t)
		update Heat(xdom, t) using inner, bord
	}
	
}

/*
void* f() 		 give
f(void *const p) keep
f(void*& p)      take
*/
