// deichain.h
#ifndef DEICHAIN_H
#define DEICHAIN_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define CONFIG_FILE "config.cfg"
#define LOG_FILE "DEIChain_log.txt"
#define NAMED_PIPE "VALIDATOR_INPUT"
#define MAX_TRANSACTIONS 10000
#define MAX_BLOCKS 50000

// IPC Keys
#define SHM_KEY_TRANS_POOL 0x1234
#define SHM_KEY_BLOCKCHAIN 0x5678
#define MSGQ_KEY_STATS 0x9ABC
#define SEM_KEY 0xDEF0
#define SHM_KEY_CONFIG 0x9BBA  // New key for config

typedef struct {
    int transaction_id;
    int reward;
    pid_t sender_id;
    int receiver_id;
    int value;
    time_t timestamp;
} Transaction;

typedef struct {
    int block_id;
    int miner_id;
    int prev_block_id;
    time_t timestamp;
    int nonce;
    int num_transactions;
    Transaction transactions[];
} Block;

typedef struct {
    int current_block_id;
    int size;
    struct {
        int empty;
        int age;
        Transaction transaction;
    } transactions[];
} TransactionPool;

typedef struct {
    int num_blocks;
    int capacity;
    Block* blocks;
} BlockchainLedger;

typedef struct {
    int num_miners;
    int pool_size;
    int transactions_per_block;
    int blockchain_blocks;
} Config;

typedef struct {
    long mtype;
    int miner_id;
    int block_id;
    int is_valid;
    int credits;
} StatisticsMessage;

typedef struct {
    sem_t pool_mutex;
    sem_t ledger_mutex;
    sem_t stats_mutex;
} IPCSemaphores;

// Funções de log
void log_message(FILE* log_file, const char* format, ...);

#endif