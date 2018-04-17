#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "json_data_parse.h"





int get_appcc_host(char const *json_data, char *appcc_host)
{
	int		ret = JSON_DATA_OK;
	cJSON  *root, *j_appcc_host;
	
	
	root = cJSON_Parse(json_data);
    if (NULL == root)
    {
		ret = JSON_DATA_INVALIDE;
		return ret;
    }

    j_appcc_host = cJSON_GetObjectItemCaseSensitive(root, "appcc_host");
    if (json_str_nul_or_empty_chk(j_appcc_host))
	{
		fprintf(stderr, 
					    "Invalided json data: miss required paramter\n");
		ret = JSON_DATA_INVALIDE;
		cJSON_Delete(root);
		return ret;
	}
	else
	{
		strcpy(appcc_host, j_appcc_host->valuestring);
	}
	
	
	cJSON_Delete(root);
	return ret;
}
