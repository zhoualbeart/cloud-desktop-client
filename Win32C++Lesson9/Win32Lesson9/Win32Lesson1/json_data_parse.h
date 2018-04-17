#ifndef _JSON_DATA_PARSE_H_
#define _JSON_DATA_PARSE_H_

#include "cJSON.h"


#define	 JSON_DATA_OK						0
#define	 JSON_DATA_INVALIDE					-1

/* check if a json node is null or it's string is empty */
#define  json_str_nul_or_empty_chk(json)							\
	((!cJSON_IsString(json)) || (NULL ==  json->valuestring)		\
	  || ('\0' ==  json->valuestring[0]))	


int get_appcc_host(char const *json_data, char *appcc_host);


#endif /* _JSON_DATA_PARSE_H_ */