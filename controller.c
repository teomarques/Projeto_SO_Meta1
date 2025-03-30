// controller.c
#include "deichain.h"

int main() {
    FILE* log_file = fopen(LOG_FILE, "w");
    Config config;
    
    // Ler configuração
    FILE* config_file = fopen(CONFIG_FILE, "r");
    fscanf(config_file, "%d %d %d %d", 
          &config.num_miners, &config.pool_size,
          &config.transactions_per_block, &config.blockchain_blocks);
    fclose(config_file);
    
    // Criar IPC
    int shm_pool = shmget(SHM_KEY_TRANS_POOL, sizeof(TransactionPool) + config.pool_size * sizeof(Transaction), IPC_CREAT|0666);
    int shm_chain = shmget(SHM_KEY_BLOCKCHAIN, sizeof(BlockchainLedger) + config.blockchain_blocks * sizeof(Block), IPC_CREAT|0666);
    int msgq = msgget(MSGQ_KEY_STATS, IPC_CREAT|0666);
    mkfifo(NAMED_PIPE, 0666);

    // In initialization section
    int shm_config = shmget(SHM_KEY_CONFIG, sizeof(Config), IPC_CREAT|0666);
    Config* shared_config = (Config*)shmat(shm_config, NULL, 0);
    *shared_config = config;  // Copy local config to shared memory
    
    // Inicializar semáforos
    IPCSemaphores* sems = (IPCSemaphores*)shmat(shmget(SEM_KEY, sizeof(IPCSemaphores), IPC_CREAT|0666), NULL, 0);
    sem_init(&sems->pool_mutex, 1, 1);
    sem_init(&sems->ledger_mutex, 1, 1);
    sem_init(&sems->stats_mutex, 1, 1);
    
    // Iniciar processos
    pid_t miner = fork();
    if (miner == 0) execl("./miner", "miner", (char*)NULL);
    
    pid_t validator = fork();
    if (validator == 0) execl("./validator", "validator", (char*)NULL);
    
    pid_t stats = fork();
    if (stats == 0) execl("./statistics", "statistics", (char*)NULL);
    
    // Esperar término
    waitpid(miner, NULL, 0);
    waitpid(validator, NULL, 0);
    waitpid(stats, NULL, 0);
    
    // Cleanup
    shmctl(shm_pool, IPC_RMID, NULL);
    shmctl(shm_chain, IPC_RMID, NULL);
    msgctl(msgq, IPC_RMID, NULL);
    unlink(NAMED_PIPE);
    fclose(log_file);
    return 0;
}