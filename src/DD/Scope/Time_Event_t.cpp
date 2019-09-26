#include <const_globals.hpp>
#include <Time_Event_t.hpp>
#include <isl_utils.hpp>
#include <utils.hpp>
#include <ctime>

using std::cout;
using std::endl;

Time_Event_t::Time_Event_t() :
	read(nullptr),
	write(nullptr),
	maps(nullptr)
{}
	
Time_Event_t::Time_Event_t(int sch_size, int max_time, ISL::UMap time_to_map, ISL::UMap time_to_unmap, ISL::UMap time_to_read, ISL::UMap time_to_write, ISL::UMap time_to_readphy, ISL::UMap time_to_writephy, ISL::USet logical_areas, ISL::Set context){
	
	
	ISL::USet t1 = isl_union_set_read_from_str(ctx, "{F6d[o5, o4, o3, o2, o1, o0]; buff4_0[0, o4]}");
	ISL::UMap t2 = isl_union_map_read_from_str(ctx, "{f6d[i0, j, k, l, m, n, 0, 3] -> F6d[o5, o4, o3, o2, o1, o0] }");
	t2 = isl_union_map_union(t2, isl_union_map_read_from_str(ctx, "{f6d[i0, j, k, l, m, n, 0, 3] -> buff4_0[0, o4] }"));
	
	/*
	ISL::print(isl_union_map_intersect_range(ISL::copy(time_to_map), isl_union_map_wrap(t2)));
	ISL::print(isl_union_map_intersect_range(ISL::copy(time_to_unmap), t1));
	
	exit(0);
	*/
	
	/*
	time_to_map = isl_union_map_gist_params(time_to_map, ISL::copy(context));
	time_to_unmap = isl_union_map_gist_params(time_to_unmap, ISL::copy(context));
	time_to_read = isl_union_map_gist_params(time_to_read, ISL::copy(context));
	time_to_write = isl_union_map_gist_params(time_to_write, ISL::copy(context));
	time_to_writephy = isl_union_map_gist_params(time_to_writephy, ISL::copy(context));
	time_to_readphy = isl_union_map_gist_params(time_to_readphy, ISL::copy(context));
	*/
	ISL::UMap time_to_map_phy = isl_union_map_reverse(isl_union_set_unwrap(isl_union_map_range(isl_union_map_curry(isl_union_map_reverse(ISL::copy(time_to_map))))));
	
	//Construct gist sets
	{
		ISL::L_Set lsets = isl_union_set_get_set_list(logical_areas);
		int nb_set = isl_set_list_size(lsets);
		for(int i=0; i<nb_set; i++){
			ISL::Set logical_area = isl_set_list_get_set(lsets, i);
			std::string lid(isl_set_get_tuple_name(logical_area));
			logical_array_max[lid] = logical_area;
		}
	}
	clock_t st = clock();

	//Normalized schedule time is as this : [...timemap... / ...timeunmap...]
//Step 1 : Convert schedule time to the normalized version
	ISL::UMap normalized_map = ISL::copy(time_to_map_phy);
	date_union_map_insert_dims(time_to_map, sch_size);
	date_union_map_insert_dims(normalized_map, sch_size);
	date_union_map_insert_dims(time_to_unmap, sch_size, 0);
	
	ISL::UMap Tlife = isl_union_map_coalesce(isl_union_map_intersect(normalized_map, ISL::copy(time_to_unmap)));
	ISL::UMap sp;
	print_elapsed_time("° Initialization of PSO actions: ", st);
	st = clock();
	
//Step 2 : Was never unmap (just unmapped before)
	sp = ISL::copy(time_to_map_phy);
	date_union_map_self_cartesian_produit(sp);
	date_union_map_n_lex_lt(sp, sch_size, 0*sch_size, 1*sch_size); //tm < tm2
	date_union_map_project(sp, sch_size, 1*sch_size);
	
	ISL::UMap Tlife_never = isl_union_map_coalesce(isl_union_map_subtract(time_to_map_phy, (sp)));
	
	date_union_map_insert_dims(Tlife_never, sch_size);
	Tlife_never = isl_union_map_coalesce(isl_union_map_intersect(time_to_unmap, Tlife_never));
	date_union_map_n_lex_lt(Tlife_never, sch_size, 1*sch_size, 0*sch_size); //tm < tu
	//Set unmap time to max time
	{
		ISL::Set tunmap = isl_set_universe(isl_set_get_space(isl_set_from_union_set(isl_union_map_domain(ISL::copy(Tlife_never)))));
		for(int i=0; i<sch_size; i++){
			ISL::Constraint c = isl_constraint_alloc_equality(lspace_from_set(tunmap));
			c = isl_constraint_set_coefficient_si(c, isl_dim_set, sch_size+i, -1);
			tunmap = isl_set_add_constraint(tunmap, c);
		}
		
		set_set_dim_to_const(tunmap, max_time, sch_size);
		date_union_map_n_apply(Tlife_never, [=] (ISL::Map& nunmap) {
			nunmap = isl_map_drop_constraints_involving_dims(nunmap, isl_dim_in, sch_size, sch_size);
			nunmap = isl_map_intersect_domain(nunmap, ISL::copy(tunmap));
		});
	}

	print_elapsed_time("° Mapping never unmap: ", st);
	st = clock();
	
//Step 3 : physical available between map and unmap
	
	ISL::UMap Tlife_between = Tlife;
	date_union_map_n_lex_lt(Tlife_between, sch_size, 0, sch_size);		//tm < tu
	{
		sp = ISL::copy(Tlife_between);
		date_union_map_self_cartesian_produit(sp, sch_size, sch_size);
		date_union_map_n_lex_lt(sp, sch_size, 2*sch_size, sch_size); //tu2 < tu
		date_union_map_project(sp, sch_size, 2*sch_size);
		Tlife_between = isl_union_map_coalesce(isl_union_map_subtract(Tlife_between, sp));
	}
		
//Step 4 : Wx => where to write + for each physical write (copy), add a read
	ISL::UMap Mphy = isl_union_map_union(Tlife_between, Tlife_never);
	maps = time_to_map;
	date_union_map_intersect_range_wrapped(maps, Mphy);
		
	print_elapsed_time("° Mapping available inbetween: ", st);
	st = clock();
	
	write = ISL::copy(maps);
	date_union_map_insert_dims(write, sch_size, 0);
	date_union_map_insert_dims(time_to_write, 2*sch_size);
	
//Set [a, b, c] : a < b < c used in write[tm, tw, tu] and read[tw, tr, tu]
	ISL::USet inbetween;
	{
		ISL::UMap tmp = isl_union_map_from_domain(isl_union_set_from_set(isl_set_universe(isl_space_set_alloc(ctx, 0, 3*sch_size))));
		//a < b
		date_union_map_n_lex_lt(tmp, sch_size, 1*sch_size, 0*sch_size);
		//b < c
		date_union_map_n_lex_lt(tmp, sch_size, 0*sch_size, 2*sch_size);
		inbetween = isl_union_map_domain(tmp);
	}
	
	write = isl_union_map_intersect_domain(write, ISL::copy(inbetween));
	
	if(isl_union_map_is_empty(time_to_writephy) == isl_bool_false){
		ISL::UMap write_phy = ISL::copy(write);
		date_union_map_insert_dims(time_to_writephy, 2*sch_size);
		date_union_map_intersect_range_wrapped(write_phy, time_to_writephy);
		//Physical write => Logical read that happen before
		{
			ISL::UMap read_from_copy = empty_isl_union_map();
			foreach_map(write_phy, [sch_size, &read_from_copy] (ISL::Map map){
				ISL::Map mapping = isl_set_unwrap(isl_map_range(ISL::copy(map)));
				const char* lid = isl_map_get_tuple_name(mapping, isl_dim_in);
				int log_dims = isl_map_dim(mapping, isl_dim_in);
				int phy_dims = isl_map_dim(mapping, isl_dim_out);
				
				map = isl_map_flatten_range(map);
				map = isl_map_project_out(map, isl_dim_out, log_dims, phy_dims);
				map = isl_map_project_out(map, isl_dim_in, sch_size, 2*sch_size);
				map = isl_map_set_tuple_name(map, isl_dim_out, lid);
				read_from_copy = isl_union_map_union(read_from_copy, isl_union_map_from_map(map));
				ISL::free(mapping);
			});
			time_to_read = isl_union_map_coalesce(isl_union_map_union(time_to_read, read_from_copy));
		}
		
		date_union_map_intersect_domain_wrapped(write, time_to_write);
		write = isl_union_map_union(write, write_phy);
	}else{
		date_union_map_intersect_domain_wrapped(write, time_to_write);
	}
		
	sp = ISL::copy(write);
	date_union_map_self_cartesian_produit(sp, sch_size, sch_size);
	//tm < tm2
	date_union_map_n_lex_lt(sp, sch_size, 1*sch_size, 3*sch_size);
	//Project unusefull tm2 date
	date_union_map_project(sp, 3*sch_size, 1*sch_size);
	write = isl_union_map_coalesce(isl_union_map_subtract(write, sp));
	//Project tm as it is now not required
	date_union_map_project(write, sch_size, sch_size);

	print_elapsed_time("° Mappings & writting: ", st);
	st = clock();
	
	ISL::USet ttt = isl_union_set_read_from_str(ctx, "[MAX_DIM1, MAX_DIM2, MAX_DIM3, MAX_DIM4, MAX_DIM5, MAX_DIM6, n_iterations, shift1, shift2, shift3, shift4, shift5, shift6, n, m, k, j, i] -> { [161, n, m, k, j, i, 11, o1, o2, o3, o4, o5, o6, o7] : MAX_DIM4 > 0 and n_iterations > 0 and 2shift1 > -MAX_DIM1 and 2shift2 > -MAX_DIM2 and 2shift3 > -MAX_DIM3 and 2shift4 > -MAX_DIM4 and 2shift5 > -MAX_DIM5 and 2shift6 > -MAX_DIM6 and shift6 <= n < MAX_DIM6 + shift6 and shift5 <= m < MAX_DIM5 + shift5 and shift3 <= k < MAX_DIM3 + shift3 and shift2 <= j < MAX_DIM2 + shift2 and shift1 <= i < MAX_DIM1 + shift1 }");
	/*
	ISL::print(isl_union_map_intersect_domain(ISL::copy(write), (ttt)));
	exit(0);
	*/
//Step 5 : Rx => where to read
	read = ISL::copy(write);
	//Project tu as it is now not required
	date_union_map_project(write, sch_size, sch_size);
	//ISL::print(write);
	//exit(0);
	date_union_map_insert_dims(read, sch_size, 0);
	date_union_map_insert_dims(time_to_read, 2*sch_size);
	{
		read = isl_union_map_intersect_domain(read, inbetween);
		
		if(isl_union_map_is_empty(time_to_readphy) == isl_bool_false){
			ISL::UMap read_phy = ISL::copy(read);
			date_union_map_intersect_domain_wrapped(read, time_to_read);
		
			date_union_map_insert_dims(time_to_readphy, 2*sch_size);
			date_union_map_intersect_range_wrapped(read_phy, time_to_readphy);
			read = isl_union_map_union(read, read_phy);
		}else{
			date_union_map_intersect_domain_wrapped(read, time_to_read);
		}
		
		//Project unusefull tu date
		date_union_map_project(read, sch_size, 2*sch_size);

		sp = ISL::copy(read);
		//Keep only the log part -> one log can be mapped to multiple phy
		//Thus, we compare the tw date of all these log
		date_union_map_n_apply(sp, [] (ISL::Map& lpmap){
			ISL::Set Log = isl_map_domain(isl_set_unwrap(isl_map_range(ISL::copy(lpmap))));
			lpmap = isl_map_flatten_range(lpmap);
			
			int nb_log = isl_set_dim(Log, isl_dim_set);
			int nb_total = isl_map_dim(lpmap, isl_dim_out);
			lpmap = isl_map_project_out(lpmap, isl_dim_out, nb_log, nb_total-nb_log);
			lpmap = isl_map_set_tuple_name(lpmap, isl_dim_out, isl_set_get_tuple_name(Log));
			ISL::free(Log);
		});
		
		date_union_map_self_cartesian_produit(sp, sch_size, sch_size);
		//tw < tw2
		date_union_map_n_lex_lt(sp, sch_size, 1*sch_size, 2*sch_size);
		//Project unusefull tw2 date
		date_union_map_project(sp, sch_size, 2*sch_size);

		read = isl_union_map_coalesce(isl_union_map_subtract_domain(read, isl_union_map_domain(sp)));
		//Project tw as it is now not required
		date_union_map_project(read, sch_size, sch_size);
	}
	print_elapsed_time("° Mappings & reading: ", st);
	st = clock();
	
	/*
	write = isl_union_map_intersect_params(write, ISL::copy(context));
	read = isl_union_map_intersect_params(read, ISL::copy(context));
	*/
}

ISL::UMap Time_Event_t::map_from_time_to_write(ISL::Set sch) const{
	ISL::UMap map = isl_union_set_unwrap(isl_union_set_apply(isl_union_set_from_set(ISL::copy(sch)), ISL::copy(write)));
	this->gist_logical_area(map);
	return isl_union_map_coalesce(map);
}
	
ISL::UMap Time_Event_t::map_from_time_to_read(ISL::Set sch) const{
	ISL::UMap map = isl_union_set_unwrap(isl_union_set_apply(isl_union_set_from_set(ISL::copy(sch)), ISL::copy(read)));
	ISL::UMap m2 = ISL::copy(map);
	this->gist_logical_area(map);
	if(isl_union_map_is_equal(map, m2) == isl_bool_false){
	//	ISL::print(m2);
	//	ISL::print(map);
		date_union_map_n_apply(map, [&] (ISL::Map& m){
			std::string log_name(isl_map_get_tuple_name(m, isl_dim_in));
	//		ISL::print(logical_array_max.at(log_name));
		});
	//	std::cout << "\n\n";
	}
	return isl_union_map_coalesce(map);
}

void Time_Event_t::gist_logical_area(ISL::UMap& umap) const{
	if(! debug_mode){
		date_union_map_n_apply(umap, [&] (ISL::Map& map){
			std::string log_name(isl_map_get_tuple_name(map, isl_dim_in));
			map = isl_map_gist_domain(map, ISL::copy(logical_array_max.at(log_name)));
		});
	}
}
	
void Time_Event_t::print() const{
	ISL::print(maps, "Maps: ");
	ISL::print(read, "Read: ");
	ISL::print(write, "Write: ");	
}