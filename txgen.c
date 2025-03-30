// txgen.c
#include "deichain.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <reward> <sleep_time_ms>\n", argv[0]);
        exit(1);
    }
    
    int reward = atoi(argv[1]);
    int sleep_time = atoi(argv[2]);
    FILE* log_file = fopen(LOG_FILE, "a");
    
    TransactionPool* pool = (TransactionPool*)shmat(shmget(SHM_KEY_TRANS_POOL, 0, 0), NULL, 0);
    IPCSemaphores* sems = (IPCSemaphores*)shmat(shmget(SEM_KEY, 0, 0), NULL, 0);
    
    int tx_count = 0;
    while (1) {
        sem_wait(&sems->pool_mutex);
        
        for (int i = 0; i < pool->size; i++) {
            if (pool->transactions[i].empty) {
                pool->transactions[i].empty = 0;
                pool->transactions[i].transaction = (Transaction){
                    .transaction_id = getpid() * 1000 + tx_count++,
                    .reward = reward,
                    .sender_id = getpid(),
                    .receiver_id = rand() % 1000,
                    .value = rand() % 100,
                    .timestamp = time(NULL)
                };
                log_message(log_file, "Generated TX %d\n", pool->transactions[i].transaction.transaction_id);
                break;
            }
        }
        
        sem_post(&sems->pool_mutex);
        usleep(sleep_time * 1000);
    }
    return 0;
}