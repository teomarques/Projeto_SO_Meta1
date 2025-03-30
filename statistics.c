// statistics.c
#include "deichain.h"

typedef struct {
    int valid_blocks;
    int invalid_blocks;
    int total_credits;
} Stats;

Stats stats = {0};
FILE* log_file;

void sig_handler(int sig) {
    log_message(log_file, "Statistics:\nValid Blocks: %d\nInvalid Blocks: %d\nTotal Credits: %d\n",
               stats.valid_blocks, stats.invalid_blocks, stats.total_credits);
}

int main() {
    log_file = fopen(LOG_FILE, "a");
    signal(SIGUSR1, sig_handler);
    
    int msgq = msgget(MSGQ_KEY_STATS, 0);
    StatisticsMessage msg;
    
    while (msgrcv(msgq, &msg, sizeof(msg), 1, 0) > 0) {
        if (msg.is_valid) {
            stats.valid_blocks++;
            stats.total_credits += msg.credits;
        } else {
            stats.invalid_blocks++;
        }
    }
    return 0;
}