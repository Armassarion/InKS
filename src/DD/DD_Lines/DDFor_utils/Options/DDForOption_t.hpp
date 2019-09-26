#ifndef DDFOROPTION_T_HPP
#define DDFOROPTION_T_HPP

#include <string>

class DDForOption_t{
	
public:
		
	/* Il peut y avoir maaaaasse de types d'options possibles ...
		modification du scheduling, 
		ajout de textes avant/après etc etc .., 
		je ne peux pas penser à tout mais il suffit d'ajouter une méthode virtuel ici avec un corps vide
	*/
	
	virtual std::string add_str_before() {return "";};
	
};

#endif // DDFOROPTION_T_HPP
