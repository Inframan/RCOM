#define OK 0
#define ABORT -1

#define ZERO 0
#define ONE 1

#define MAX_SIZE 255
#define MAX_SIZE_WITH_NULL 256

#define FILE_CHUNK_SIZE 255
#define FTP_SLEEP 200000
#define FILE_SLEEP 150000

#define FTP_PORT 21

#define STR_USER "USER "
#define STR_PASSWORD "PASS "
#define STR_CWD "CWD "
#define STR_PASV "PASV"
#define STR_SIZE "SIZE "
#define STR_RETR "RETR "
#define STR_QUIT "QUIT"
#define BLANK ""

#define CODE_USER "220"
#define CODE_PASSWORD "331"
#define CODE_USER_LOGGED "230"
#define CODE_CWD "250"
#define CODE_PASV "227"
#define CODE_SIZE "213"
#define CODE_QUIT "226"

#define USER_ANONYMOUS "anonymous"
#define PASSWORD_ANONYMOUS "anonymous"

#define SLASH '/'
#define AT '@'
#define COLON ':'
#define NEWLINE '\n'
#define NULL_CHAR '\0'

#define FTP_CODE_NO_FILE "550"
#define FTP_CODE_WRONG_CREDENTIALS "430"
