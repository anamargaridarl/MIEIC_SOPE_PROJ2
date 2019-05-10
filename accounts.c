#include "accounts.h"

#include <string.h>
#include <stdio.h>
#include "crypto.h"



void insert_account(bank_account_t account)
{
    accounts[account.account_id] = account;
    account_ids[account.account_id] = 1;
}


//TODO: add possibility to be differrent id and not incremented automatically--done
//Need to verify if it was already used
ret_code_t create_account(char *password, int balance, int new_id, int account_create_id) 
{
    char salt[SALT_LEN +1];
    create_salt(salt);
    char hash[HASH_LEN+1];
    create_hash(password,salt,hash); //TODO:fix bug in sha256sum

    bank_account_t account;

    if(account_create_id == 0)
        return RC_OP_NALLOW;

    if(account_ids[new_id] == 1)
        return RC_ID_IN_USE;

    account.account_id = new_id;
    account.balance = balance;
    strcpy(account.salt, salt);
    strcpy(account.hash, password); //TODO:add hash

    insert_account(account);

    //RC_OTHER

    //add log here
    return RC_OK;
}

ret_code_t transfer_money(uint32_t sender_id, uint32_t receiver_id, uint32_t value) 
{
    // check if either of the accounts doesn't exist (the sender has to exist so it might not be
    // necessary to check if the sender exists)
    if (account_ids[sender_id] == 0 || account_ids[receiver_id] == 0) {
        return RC_ID_NOT_FOUND;
    }

    // check if accounts are the same
    if (account_ids[sender_id] == account_ids[receiver_id]) {
        return RC_SAME_ID;
    }

    // check if sender's balance would be too low
    if (accounts[sender_id].balance - value < MIN_BALANCE) {
        return RC_NO_FUNDS;
    }

    // check if receiver's balance would be too high
    if (accounts[sender_id].balance + value > MAX_BALANCE) {
        return RC_TOO_HIGH;
    }

    accounts[sender_id].balance -= value;
    accounts[receiver_id].balance += value;

    return RC_OK;
}

int getHash(char* password, char* salt, char* hash)
{
    int fd[2];
    int stdout_copy;
    int status;
    pid_t pid;
    char stringToHash[strlen(password) + 1];
    sprintf(stringToHash, "%s%s", password, salt);

    if (pipe(fd) != 0) {
        perror("Error creating pipe");
        return 1;
    }

    stdout_copy = dup(STDOUT_FILENO);

    pid = fork();
    if (pid > 0) {
        close(fd[WRITE]);
        
        if (read(fd[READ], hash, HASH_LEN) == 0) {
            perror("Error reading hash");
            return 1;
        }

        wait(&status);
        if(WEXITSTATUS(status) == 1) return 1;
    }
    else if (pid == 0) {
        close(fd[READ]);
        dup2(fd[WRITE], STDOUT_FILENO);
        execlp("sha256sum", "sha256sum", stringToHash, NULL);
        perror("Error obtaining hash");
        exit(1);
    }
    else {
        perror("Error doing fork");
        return 1;
    }

    dup2(stdout_copy, STDOUT_FILENO);
    close(stdout_copy);

    return 0;
}


//test
void show_bank_account(int id)
{
    bank_account_t test = accounts[id];
    printf("%d\n", test.account_id);
    printf("%d\n", test.balance);
    printf("%s\n", test.hash);
    printf("%s\n", test.salt);
}

//handle balance request functions
bank_account_t* getAccount(uint32_t account_id){
    if(account_ids[account_id]!=1) return NULL;
    else return &accounts[account_id];
}

void opDelay(int delayMS){
    usleep(delayMS*1000);
}

tlv_reply_t newReply(){
    tlv_reply_t reply;

    reply.type = -1;
    reply.value.header.account_id = -1;
    reply.value.header.ret_code = 0;
    reply.value.balance.balance = 0;
    reply.value.transfer.balance = 0;
    reply.value.shutdown.active_offices = 0;
    reply.length =-1;

    return reply;
}

tlv_reply_t makeErrorReply(int retCode, tlv_reply_t request){
    tlv_reply_t reply=newReply();

    reply.type = request.type;
    reply.value.header.account_id = request.value.header.account_id;
    reply.value.header.ret_code = retCode;
    reply.length=sizeof(reply.value.header);

    return reply;
}

tlv_reply_t makeBalanceReply(int accountId, int balance){
    tlv_reply_t reply;

    reply.type = OP_BALANCE;
    reply.value.header.account_id = accountId;
    reply.value.balance.balance = balance;
    reply.value.header.ret_code = RC_OK;
    reply.length=sizeof(reply.value.header) + sizeof(rep_balance_t);

    return reply;
}

tlv_reply_t handleBalanceRequest(tlv_request_t request){
    opDelay(request.value.header.op_delay_ms);
    if(request.value.account_id != ADMIN_ACCOUNT_ID){
        bank_account_t* account = getAccount(request.value.header.account_id);
        return makeBalanceReply(account->account_id,account->balance);
    }
    else return makeErrorReply(RC_OP_NALLOW, request);
}
