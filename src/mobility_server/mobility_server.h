/* This research is supported by the European Union Seventh Framework Programme (FP7/2007-2013), project ASPIRE (Advanced  Software Protection: Integration, Research, and Exploitation), under grant agreement no. 609734; on-line at https://aspire-fp7.eu/. */

/*
	CODE MOBILITY SERVER headers file
*/

#ifndef CODE_MOBILITY_SERVER__
#define CODE_MOBILITY_SERVER__

#ifndef MOBILITY_SERVER_CHECK_COMPROMISED_APPLICATION
	#define MOBILITY_SERVER_CHECK_COMPROMISED_APPLICATION 1
#endif

#ifndef MOBILITY_SERVER_USE_RENEWABILITY
	#define MOBILITY_SERVER_USE_RENEWABILITY 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>

#if MOBILITY_SERVER_CHECK_COMPROMISED_APPLICATION
#include <mysql/mysql.h>
#endif

#if MOBILITY_SERVER_USE_RENEWABILITY
#include <mysql/mysql.h>
#endif

/* utility functions prototypes */
size_t readMobileCodeBlock (const char* filename, char* returnbuffer);
void mobilityServerLog(const char* tag, const char* fmt, int lvl, ...);
int checkApplication(const char* application_id);
int readActualRevision(const char* application_id, char* revision);

#define MS_MSG_STRING_LEN					1024

#define MOBILITY_SERVER_LOG_FILE			"/opt/online_backends/code_mobility/mobility_server.log"
#define MS_DEBUG							0
#define MS_INFO								1
#define MS_ERROR							2

#ifndef MOBILITY_SERVER_LOG_LEVEL
	#define MOBILITY_SERVER_LOG_LEVEL 		0
#endif

#define MOBILITY_SERVER_ERROR				-1
#define CODE_MOBILITY_ASCL_SERVER_ERROR		1
#define MOBILITY_SERVER_MAX_BLOCK_SIZE		(1 << 22)
#define ASPIRE_APPLICATION_ID_MAX_LENGTH	50
#define MOBILITY_SERVER_OK					1
#define MOBILITY_SERVER_KO					0

#define MOBILITY_SERVER_MYSQL_HOST			"localhost"
#define MOBILITY_SERVER_MYSQL_DB			"RA_development"
#define MOBILITY_SERVER_MYSQL_USER			"ra_user"
#define MOBILITY_SERVER_MYSQL_PASSWORD		""

#define MOBILITY_SERVER_RENEWABILITY_MYSQL_HOST			"localhost"
#define MOBILITY_SERVER_RENEWABILITY_MYSQL_DB			"RN_development"
#define MOBILITY_SERVER_RENEWABILITY_MYSQL_USER			"rn_user"
#define MOBILITY_SERVER_RENEWABILITY_MYSQL_PASSWORD		"rn_password"

#define MOBILITY_SERVER_RA_POLLING_TIME		50000
#define MOBILITY_SERVER_RA_POLLING_RETRIES	500

#define MOBILITY_SERVER_REPOSITORY_PATH "/opt/online_backends/%s/code_mobility/%s/mobile_dump_%08x"

#endif

