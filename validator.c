// validator.c
#include "deichain.h"

IPCSemaphores* sems;
TransactionPool* pool;
BlockchainLedger* ledger;
FILE* log_file;

void update_transaction_aging() {
    for (int i = 0; i < pool->size; i++) {
        if (!pool->transactions[i].empty) {
            pool->transactions[i].age++;
            if (pool->transactions[i].age % 50 == 0) {
                pool->transactions[i].transaction.reward++;
            }
        }
    }
}

int main() {
    log_file = fopen(LOG_FILE, "a");
    pool = (TransactionPool*)shmat(shmget(SHM_KEY_TRANS_POOL, 0, 0), NULL, 0);
    ledger = (BlockchainLedger*)shmat(shmget(SHM_KEY_BLOCKCHAIN, 0, 0), NULL, 0);
    sems = (IPCSemaphores*)shmat(shmget(SEM_KEY, 0, 0), NULL, 0);
    
    int fd = open(NAMED_PIPE, O_RDONLY);
    Block block;
    
    while (read(fd, &block, sizeof(Block)) > 0) {
        sem_wait(&sems->ledger_mutex);
        
        // Validar PoW
        if ((block.nonce + block.block_id) % 10 != 0) continue;
        
        // Atualizar blockchain
        if (ledger->num_blocks < ledger->capacity) {
            memcpy(&ledger->blocks[ledger->num_blocks++], &block, sizeof(Block));
        }
        
        // Remover transações do pool
        sem_wait(&sems->pool_mutex);
        for (int i = 0; i < block.num_transactions; i++) {
            for (int j = 0; j < pool->size; j++) {
                if (pool->transactions[j].transaction.transaction_id == 
                    block.transactions[i].transaction_id) {
                    pool->transactions[j].empty = 1;
                    break;
                }
            }
        }
        update_transaction_aging();
        sem_post(&sems->pool_mutex);
        
        sem_post(&sems->ledger_mutex);
    }
    close(fd);
    return 0;
}