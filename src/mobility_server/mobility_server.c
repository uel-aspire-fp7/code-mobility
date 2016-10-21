/* This research is supported by the European Union Seventh Framework Programme (FP7/2007-2013), project ASPIRE (Advanced  Software Protection: Integration, Research, and Exploitation), under grant agreement no. 609734; on-line at https://aspire-fp7.eu/. */

#include "mobility_server.h"

/*

	The Code Mobility Server receives a mobile_block_request struct via STDIN
		and the payload length as the 2nd entry in argv:
	
		stdin = block index
		argv[1] = request type ['exchange' | 'send']
		argv[2] = payload length
		argv[3] = ASPIRE_APPLICATION_ID
		
	The code block is read from a file called code_X.bin where X is the block
		index and returned on the STDOUT.
*/
int main(int argc, char** argv){
	char *in_buf, out_buf[MOBILITY_SERVER_MAX_BLOCK_SIZE], file_block[255], application_id[ASPIRE_APPLICATION_ID_MAX_LENGTH];
	char *actual_revision = "00000000";
	int read_bytes = 0, ret_n, application_id_len;
	unsigned int block_index, in_buf_len;

	/* parameters check */
	if (argc < 4) {
		mobilityServerLog("Code Mobility Server", "Invalid arguments number: %d (%d expected).\n", 
			MS_ERROR, argc, 3);

		return 1;
	}

	/* check whether the specified application should be served or not */
	application_id_len = strlen(argv[3]);

	if (application_id_len > 0 && application_id_len <= ASPIRE_APPLICATION_ID_MAX_LENGTH) {
		
		#if MOBILITY_SERVER_CHECK_COMPROMISED_APPLICATION

		strcpy(application_id, argv[3]);

		if (MOBILITY_SERVER_KO == checkApplication(application_id)) {
			mobilityServerLog("Code Mobility Server", "Application ID '%s' didn't pass the RA safety check.\n",
							  MS_ERROR, application_id);

			return 1;
		}

		#endif
		
	} else {
		mobilityServerLog("Code Mobility Server", "Invalid Application ID\n",
						  MS_ERROR, argc, 3);

		return 1;
	}

	/* read payload length from command arguments */
	in_buf_len = atoi(argv[2]);
	
	if (in_buf_len <= 0 || in_buf_len > MOBILITY_SERVER_MAX_BLOCK_SIZE) {
		mobilityServerLog("Code Mobility Server", "Invalid payload size.\n", 
			MS_ERROR);

		return 1;
	}
	
	/* allocate buffer for incoming payload */
	in_buf = (char*)malloc(in_buf_len * sizeof(char));

	/* copy the payload from stdin to an internal buffer */
	while (read_bytes < in_buf_len) {
		int n = read(STDIN_FILENO, 
					in_buf + read_bytes * sizeof(char), 
					in_buf_len - read_bytes);
		
		/* cannot read from input buffer */
		if (n == -1) {
			mobilityServerLog("Code Mobility Server", "Error while reading from input buffer.\n", 
				MS_ERROR);

			free (in_buf);
			return 1;
		} else {
			read_bytes += n;
		}
		
		/* passed buffer is smaller than passed size */
		if (n == 0 && read_bytes < in_buf_len) {
			mobilityServerLog("Code Mobility Server", "Not enought data in input buffer.\n", 
				MS_ERROR);

			free (in_buf);
			return 1;
		}
	}

	/* block_index -> block requested by the client */
	memcpy(&block_index, in_buf, sizeof(unsigned int));

	mobilityServerLog("Code Mobility Server", "BLOCK_ID %d requested\n", 
		MS_INFO, block_index);

	/* compose block file name */
	sprintf(file_block, MOBILITY_SERVER_REPOSITORY_PATH, application_id, actual_revision, block_index);

	mobilityServerLog("Code Mobility Server", "BLOCK_ID %x (filename: %s) is going to be served.\n",
		MS_INFO, block_index, file_block);

	/* read actual data to the buffer */
	ret_n = (int)readMobileCodeBlock (file_block, out_buf);

	/* check for error */
	if (ret_n != MOBILITY_SERVER_ERROR) {
		mobilityServerLog("Code Mobility Server", "BLOCK_ID %d is %d bytes long.\n", 
			MS_INFO, block_index, ret_n);

		write(STDOUT_FILENO, out_buf, ret_n);

		/* release memory */
		free(in_buf);

		mobilityServerLog("Code Mobility Server", "BLOCK_ID %d correctly sent to ASPIRE Portal.\n", 
			MS_INFO, block_index);
		
		return 0;
	} else {
		mobilityServerLog("Code Mobility Server", "BLOCK_ID %d is not valid or exceeds maximum block size (%d bytes).\n", 
			MS_ERROR, block_index, MOBILITY_SERVER_MAX_BLOCK_SIZE);

		/* release memory */
		free(in_buf);
		
		return 1;
	}
}

#if MOBILITY_SERVER_CHECK_COMPROMISED_APPLICATION

int terminateWithMysqlError(MYSQL *con) {
	mobilityServerLog("Code Mobility Server", mysql_error(con), MS_ERROR);

	if (NULL != con)
		mysql_close(con);

	return MOBILITY_SERVER_KO;
}

/*******************************************************************************
* NAME :            checkApplication
*
* DESCRIPTION :     Checks whether blocks delivery should be stopped or not
*
* INPUTS :
*       PARAMETERS:
*           const char*		Aspire Application Id
*       GLOBALS :
*           None
* OUTPUTS :
*       PARAMETERS:
*       	None
*       GLOBALS :
*           None
*       RETURN :
*            Type:   int                    Error code:
*            Values: MOBILITY_SERVER_OK No action required
*            		 MOBILITY_SERVER_KO	Code blocks delivery should be stopped
*
* PROCESS :
*       [1]  read data from a file
*/
int checkApplication(const char* application_id) {
	char query[1024];
	MYSQL *con = mysql_init(NULL);
	MYSQL_RES *result;
	MYSQL_ROW row;
	int ret = MOBILITY_SERVER_OK, application_status_unknown=1, notries=0;

	if (MOBILITY_SERVER_CHECK_COMPROMISED_APPLICATION == 0) {
		mobilityServerLog("checkApplication()", "Skipping RA checks\n", MS_INFO);
		return ret;
	}

	if (NULL == con)
		return terminateWithMysqlError (con);

	if (mysql_real_connect(con,
						   MOBILITY_SERVER_MYSQL_HOST,
						   MOBILITY_SERVER_MYSQL_USER,
						   MOBILITY_SERVER_MYSQL_PASSWORD,
						   NULL, 0, NULL, 0) == NULL)
		return terminateWithMysqlError (con);

	// database selection
	sprintf(query, "USE %s", MOBILITY_SERVER_MYSQL_DB);

	if (mysql_query(con, query))
		return terminateWithMysqlError (con);

	// application verification query
	sprintf(query, "SELECT RS.name FROM ra_application A INNER JOIN ra_reaction R ON A.id = R.application_id INNER JOIN ra_reaction_status RS ON RS.id = R.reaction_status_id WHERE A.AID = '%s'", application_id);

	while (1 == application_status_unknown) {

		if (mysql_query(con, query))
			return terminateWithMysqlError (con);

		if ((result = mysql_store_result(con)) == NULL)
			return terminateWithMysqlError(con);

		// if no result set -> increment number of tries
		if (0 == mysql_num_rows(result)) {
			notries++;
		}

		while ((row = mysql_fetch_row(result))) {

			// the application is tampered with -> deny block service
			if ((strcmp(row[0], "COMPROMISED") == 0) ||
						(strcmp(row[0], "TAMPERED") == 0))		{
				application_status_unknown = 0;

				ret = MOBILITY_SERVER_KO;

				mobilityServerLog("checkApplication()", "Tampering detected.\n", MS_ERROR);
			}

			// poll the database every 50ms if the application status is still unknown
			if (strcmp(row[0], "UNKNOWN") == 0)		{
				usleep(MOBILITY_SERVER_RA_POLLING_TIME);

				notries++;
			}

			// now we can serve the block
			if (strcmp(row[0], "NONE") == 0)		{
				application_status_unknown = 0;
			}
		}

		mysql_free_result(result);

		/* too much tries */
		if (notries == MOBILITY_SERVER_RA_POLLING_RETRIES) {
			application_status_unknown = 0;

			ret = MOBILITY_SERVER_KO;

			mobilityServerLog("checkApplication()", "Timeout while waiting for RA input.\n", MS_ERROR);
		}
	}

	mysql_close(con);

	return ret;
}

#endif

/*******************************************************************************
* NAME :            readcode
*
* DESCRIPTION :     Read mobile code block from a file into a memory buffer
*
* INPUTS :
*       PARAMETERS:
*           const char*   filename	name of the file containing mobile block*           
*       GLOBALS :
*           None
* OUTPUTS :
*       PARAMETERS:
*	    	char* returnbuffer buffer in which the code block will be returned
*       GLOBALS :
*            None
*       RETURN :
*            Type:   int                    Error code:
*            Values: >= 0 the length in bytes of the mobile block
*                    MOBILITY_SERVER_ERROR
* PROCESS :
*       [1]  read data from a file
*/	
size_t readMobileCodeBlock (const char* filename, char* returnbuffer) {
	struct stat s;
	size_t totalsize = 0, readsize = 0;
	
	/* obtain a file descriptor to filename */
	int fd = open(filename, O_RDONLY);

	/* if file open succeed */
	if (-1 != fd) {
		/* read file size */
		fstat (fd, &s);
	  	totalsize = s.st_size;

	  	/* if the size is greater than allowed size return error*/
	  	if (totalsize > MOBILITY_SERVER_MAX_BLOCK_SIZE) {
	  		close(fd);

	  		return MOBILITY_SERVER_ERROR;
	  	}
	
		/* read the file content into return buffer */
		readsize = (size_t)read(fd, returnbuffer, totalsize);

		close (fd);

		if (readsize == totalsize)
			return readsize;
		else
			return MOBILITY_SERVER_ERROR;
	}

	return MOBILITY_SERVER_ERROR;
}

void mobilityServerLog(const char* tag, const char* fmt, int lvl, ...) {
	FILE * fp;
	time_t now;
	
	va_list ap;                                /* special type for variable */
	char format[MS_MSG_STRING_LEN];       		/* argument lists            */
	int count = 0;
	int i, j;                                  /* Need all these to store   */
	char c;                                    /* values below in switch    */
	double d;
	unsigned u;
	char *s;
	void *v;
	
	if (lvl < MOBILITY_SERVER_LOG_LEVEL)
		return;

	fp = fopen (MOBILITY_SERVER_LOG_FILE, "a");
	
	if (NULL == fp) {
		fprintf(stderr, "ERROR: Unable to log to file '%s'\n", MOBILITY_SERVER_LOG_FILE);
		return;	
	}
	
	if ((time_t)-1 == time(&now)) {
		fprintf(stderr, "ERROR: Unable to retrieve current time.log\n");
		return;	
	}

  	fprintf(fp, "%.24s [%s] ", ctime(&now), tag);

	va_start(ap, lvl);                         /* must be called before work */
  
	while (*fmt) {
		for (j = 0; fmt[j] && fmt[j] != '%'; j++)
			format[j] = fmt[j];                    /* not a format string          */
		
		if (j) {
			format[j] = '\0';
			count += fprintf(fp, format, "");    /* log it verbatim              */
			fmt += j;
		} else {
			for (j = 0; !isalpha(fmt[j]); j++) {   /* find end of format specifier */
				format[j] = fmt[j];
				if (j && fmt[j] == '%')              /* special case printing '%'    */
				break;
			}
		
			format[j] = fmt[j];                    /* finish writing specifier     */
			format[j + 1] = '\0';                  /* don't forget NULL terminator */
			fmt += j + 1;
			
			switch (format[j]) {                   /* cases for all specifiers     */
			case 'd':
			case 'i':                              /* many use identical actions   */
				i = va_arg(ap, int);                 /* process the argument         */
				count += fprintf(fp, format, i); /* and log it                 */
				break;
			case 'o':
			case 'x':
			case 'X':
			case 'u':
				u = va_arg(ap, unsigned);
				count += fprintf(fp, format, u);
				break;
			case 'c':
				c = (char) va_arg(ap, int);          /* must cast!  */
				count += fprintf(fp, format, c);
				break;
			case 's':
				s = va_arg(ap, char *);
				count += fprintf(fp, format, s);
				break;
			case 'f':
			case 'e':
			case 'E':
			case 'g':
			case 'G':
				d = va_arg(ap, double);
				count += fprintf(fp, format, d);
				break;
			case 'p':
				v = va_arg(ap, void *);
				count += fprintf(fp, format, v);
				break;
			case 'n':
				count += fprintf(fp, "%d", count);
				break;
			case '%':
				count += fprintf(fp, "%%");
				break;
			default:
				fprintf(stderr, "Invalid format specifier in acclLOG().\n");
			}
		}
	}
	
	fprintf(fp, "\n");
	
	va_end(ap);  // clean up
		
	fclose(fp);
}
