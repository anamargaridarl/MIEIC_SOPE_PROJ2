#ifndef _USERFLAG_H_
#define _USERFLAG_H_

#include "constants.h"
#include "types.h"
#include <string.h>

#define WRONG_ARGUMENTS 1

typedef struct {
    uint32_t id;
    char password[MAX_PASSWORD_LEN + 1];
    uint32_t delay;
    uint32_t opnumber;
    char arguments[BUFFER_SIZE];

} User_flag;

int addflag(char* argv[], User_flag* flag);
void printTest(User_flag flag);

int verify_id(uint32_t id);
int verify_pass_len(char* pass);
int verify_opnumber(op_type_t opnumber);
int verify_balance(uint32_t balance);
int verify_new_account_args(char* arguments);
int verify_transfer_arguments(char* arguments);

#endif /* _USERFLAG_H_ */