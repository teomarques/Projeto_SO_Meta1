// miner.c
#include "deichain.h"

IPCSemaphores* sems;
TransactionPool* pool;
BlockchainLedger* ledger;
Config* config;  // Changed to dedicated config shared memory
FILE* log_file;

void* miner_thread(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&sems->pool_mutex);
        
        Block block;
        block.miner_id = id;
        block.num_transactions = 0;
        
        // Select transactions using config parameter
        for (int i = 0; i < config->pool_size && block.num_transactions < config->transactions_per_block; i++) {
            if (!pool->transactions[i].empty) {
                memcpy(&block.transactions[block.num_transactions++], 
                      &pool->transactions[i].transaction, sizeof(Transaction));
            }
        }
        
        // Simulate PoW
        block.nonce = 0;
        while ((block.nonce + block.block_id) % 10 != 0) block.nonce++;
        
        // Send to validator
        int fd = open(NAMED_PIPE, O_WRONLY);
        write(fd, &block, sizeof(Block));
        close(fd);
        
        sem_post(&sems->pool_mutex);
        usleep(100000);
    }
    return NULL;
}

int main() {
    log_file = fopen(LOG_FILE, "a");
    
    // Access config from dedicated shared memory
    config = (Config*)shmat(shmget(SHM_KEY_CONFIG, 0, 0), NULL, 0);
    pool = (TransactionPool*)shmat(shmget(SHM_KEY_TRANS_POOL, 0, 0), NULL, 0);
    sems = (IPCSemaphores*)shmat(shmget(SEM_KEY, 0, 0), NULL, 0);
    
    pthread_t threads[config->num_miners];
    int ids[config->num_miners];
    
    for (int i = 0; i < config->num_miners; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, miner_thread, &ids[i]);
    }
    
    pause();
    return 0;
}